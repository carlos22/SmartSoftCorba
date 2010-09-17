// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//		  brich@mail.hs-ulm.de (2010)
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// --------------------------------------------------------------------------

#include "smartSoft.hh"

#include <qapplication.h>
#include <qimage.h>
#include <qpainter.h>

#include "commVideoImage.hh"
#include "smartVideoClientGuiThread.hh"
#include "smartQtPushNewestClient.hh"
#include "smartQtPushTimedClient.hh"
#include "smartState.hh"

// -------------------------------------------------------------
//   GUI Thread
// -------------------------------------------------------------
int GuiThread::svc() {
	std::cout << "Debug: Begin SVC()" << std::endl;
	QApplication app(_argc, _argv);
	std::cout << "Debug: Created QApp" << std::endl;

	_video_widget = new QWidget;
	_pixmap = new QPixmap;

	_query_image[0] = new Smart::CommResizableVideoImage(_width, _height, _format);
	_query_image[1] = new Smart::CommResizableVideoImage(_width, _height, _format);

	_push_image = new Smart::CommResizableVideoImage(_width, _height, _format);

	_state_client.setWaitState("active");
	if (_portActive == 0) {
		QObject::connect(&_image_client, SIGNAL(new_data()), this, SLOT(new_image()));
		std::cout << "Debug: Query active" << std::endl;
	} else if (_portActive == 1) {
		QObject::connect(&_newest_client, SIGNAL(new_data()), this, SLOT(new_image()));
		_newest_client.subscribe();
		std::cout << "Debug: Push Newest active" << std::endl;
	} else if (_portActive == 2) {
		QObject::connect(&_timed_client, SIGNAL(new_data()), this, SLOT(new_image()));
		_timed_client.subscribe(1);
		std::cout << "Debug: Push Timed active" << std::endl;
	}

	_video_widget->installEventFilter(this);

	app.setMainWidget(_video_widget);

	_video_widget->resize(_width, _height);
	_video_widget->show();

	CHS::StatusCode status;

	_query_index = 0;
	status = _image_client.queryRequest(*_query_image[0], _query_id[0]);

	int ret = 0;
	if (status == CHS::SMART_OK) {
		ret = app.exec();
	} else {
		std::cerr << "ERROR: query request failed (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
		ret = -1;
	}

	_image_client.cancel();
	_newest_client.unsubscribe();

	delete _query_image[0];
	_query_image[0] = 0;

	delete _query_image[1];
	_query_image[1] = 0;

	delete _push_image;
	_push_image = 0;

	delete _pixmap;
	_pixmap = 0;

	delete _video_widget;
	_video_widget = 0;

	return ret;
}

void GuiThread::new_image() {

	static unsigned long seq_count = 0;

	static unsigned char *buffer = 0;
	static unsigned int buffer_size = 0;

	CHS::StatusCode req_status, ans_status;
	Smart::CommVoid dummy;

	if (_portActive == 0) {
		// Query
		unsigned int next_query_index = 1 - _query_index;
		//Needed otherwise client will flood the cpu with requests;
		usleep(20000);
		req_status = _image_client.queryRequest(*_query_image[next_query_index], _query_id[next_query_index]);
		ans_status = _image_client.queryReceive(_query_id[_query_index], dummy);
		if (ans_status == CHS::SMART_OK) {
			if (_query_image[_query_index]->is_data_valid()) {
				if (_verbose) {
					const unsigned long new_seq_count = _query_image[_query_index]->get_sequence_counter();
					if (seq_count != 0) {
						if (new_seq_count > seq_count + 1) {
							std::cerr << std::endl << "dropped " << new_seq_count - seq_count - 1 << " frames" << std::endl;
						}
					}
					seq_count = new_seq_count;
				}

				const unsigned int size = _query_image[_query_index]->get_size_as_rgb32();
				if (size > buffer_size) {
					if (buffer)
						delete[] buffer;
					buffer = new unsigned char[size];
					buffer_size = size;
				}
				_query_image[_query_index]->get_as_rgb32(buffer);

				QImage image(buffer, _query_image[_query_index]->get_width(), _query_image[_query_index]->get_height(), 32, 0, 0,
						QImage::LittleEndian);
				_pixmap->convertFromImage(image, 0);
				this->_refresh();
			} else {
				std::cerr << "WARNING: invalid data, format mismatch?" << std::endl;
			}
		} else {
			std::cerr << "ERROR: failed to receive query answer (" << CHS::StatusCodeConversion(ans_status) << ")" << std::endl;
		}
		if (req_status != CHS::SMART_OK) {
			std::cerr << "ERROR: failed to send query request (" << CHS::StatusCodeConversion(req_status) << ")" << std::endl;
		}
		_query_index = next_query_index;
	} else if (_portActive == 1) {
		//newest
		req_status = _newest_client.getUpdate(*_push_image);

		if (req_status != CHS::SMART_OK) {
			std::cout << "NEWEST -- ERROR: query request failed (" << CHS::StatusCodeConversion(req_status) << ")" << std::endl;
		} else {
			if (_push_image->is_data_valid()) {

				const unsigned int size = _push_image->get_size_as_rgb32();
				if (size > buffer_size) {
					if (buffer)
						delete[] buffer;
					buffer = new unsigned char[size];
					buffer_size = size;
				}
				_push_image->get_as_rgb32(buffer);

				QImage image(buffer, _push_image->get_width(), _push_image->get_height(), 32, 0, 0, QImage::LittleEndian);
				_pixmap->convertFromImage(image, 0);
				this->_refresh();
			} else {
				std::cerr << "WARNING: invalid data, format mismatch?" << std::endl;
			}
		}
	} else if (_portActive == 2) {
		//timed
		req_status = _timed_client.getUpdate(*_push_image);

		if (req_status != CHS::SMART_OK) {
			std::cout << "TIMED -- ERROR: query request failed (" << CHS::StatusCodeConversion(req_status) << ")" << std::endl;
		} else {
			if (_push_image->is_data_valid()) {

				const unsigned int size = _push_image->get_size_as_rgb32();
				if (size > buffer_size) {
					if (buffer)
						delete[] buffer;
					buffer = new unsigned char[size];
					buffer_size = size;
				}
				_push_image->get_as_rgb32(buffer);

				QImage image(buffer, _push_image->get_width(), _push_image->get_height(), 32, 0, 0, QImage::LittleEndian);
				_pixmap->convertFromImage(image, 0);
				this->_refresh();
			} else {
				std::cerr << "WARNING: invalid data, format mismatch?" << std::endl;
			}
		}
	}
}

bool GuiThread::eventFilter(QObject *target, QEvent *event) {
	if (target == _video_widget) {
		QPaintEvent *pe = dynamic_cast<QPaintEvent*> (event);
		if (pe) {
			this->_refresh(pe);
			return true;
		}
	}
	return false;
}

void GuiThread::_refresh(QPaintEvent *pe) {
	if (_verbose)
		std::cerr << ".";
	if (_pixmap) {
		QPainter painter(_video_widget);
		if (pe) {
			painter.drawPixmap(QPoint(0, 0), *_pixmap, pe->rect());
		} else {
			painter.drawPixmap(0, 0, *_pixmap);
		}
	}
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main(int argc, char *argv[]) {
	CHS::SmartParameter parameters;
	parameters.addCommandLine("cmdline", argc, argv);

	std::string client_name = "smartVideoClient";
	std::string image_server_name = "SmartUnicapImageServer";
	std::string image_query_service_name = "imageQuery";
	std::string image_push_newest_service_name = "imageNewest";
	std::string image_push_timed_service_name = "imageTimed";
	std::string image_state_service_name = "stateServer";

	std::string image_format_name;
	parameters.getString("cmdline", "imageformat", image_format_name);

	int image_width;
	int image_height;
	parameters.getInt("cmdline", "imagewidth", image_width);
	parameters.getInt("cmdline", "imageheight", image_height);
	bool verbose = false;

	// port Active 0 = query, 1 = newest, 2 = timed
	int portActive = 0;
	parameters.getInt("cmdline", "portActive", portActive);

	Smart::CommVideoImage::Format image_format;
	Smart::CommVideoImage::string2format(image_format_name, image_format);

	try {
		CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

		CHS::SmartComponent component(client_name, argc, argv);

		Smart::QtQueryClient<Smart::CommMutableVideoImage, Smart::CommVoid> query_client(&component, image_server_name,
				image_query_service_name);
		Smart::QtPushNewestClient<Smart::CommMutableVideoImage> newest_client(&component, image_server_name,
				image_push_newest_service_name);
		Smart::QtPushTimedClient<Smart::CommMutableVideoImage> timed_client(&component, image_server_name,
				image_push_timed_service_name);
		CHS::SmartStateClient state_client(&component, image_server_name, image_state_service_name);

		GuiThread gui_thread(argc, argv, image_width, image_height, image_format, query_client, newest_client, timed_client,
				state_client, verbose, portActive);

		gui_thread.open();

		component.run();

		threadManager->wait();
	} catch (const CORBA::Exception &) {
		std::cerr << "Uncaught CORBA exception" << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Uncaught exception" << std::endl;
		return 1;
	}

	return 0;
}

