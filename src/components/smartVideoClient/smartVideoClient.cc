// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
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

// -------------------------------------------------------------
//   GUI Thread
// -------------------------------------------------------------
int GuiThread::svc()
{
	std::cout << "Debug: Begin SVC()" << std::endl;
  QApplication app(_argc, _argv);
	std::cout << "Debug: Created QApp" << std::endl;

  _video_widget = new QWidget;
  _pixmap = new QPixmap;

  _query_image[0] = new Smart::CommResizableVideoImage(_width,_height,_format);
  _query_image[1] = new Smart::CommResizableVideoImage(_width,_height,_format);

  QObject::connect(&_image_client, SIGNAL(new_data()), this, SLOT(new_image()));
  _video_widget->installEventFilter(this);

  app.setMainWidget(_video_widget);

  _video_widget->resize(_width,_height);
  _video_widget->show();

  CHS::StatusCode status;

  _query_index = 0;
  status = _image_client.queryRequest(*_query_image[0], _query_id[0]);

  int ret = 0;
  if(status==CHS::SMART_OK)
  {
    ret = app.exec();
  }
  else
  {
    std::cerr << "ERROR: query request failed (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
    ret = -1;
  }

  _image_client.cancel();

  delete _query_image[0];
  _query_image[0] = 0;

  delete _query_image[1];
  _query_image[1] = 0;

  delete _pixmap;
  _pixmap = 0;

  delete _video_widget;
  _video_widget = 0;

  return ret;
}

void GuiThread::new_image()
{
  static unsigned long seq_count = 0;

  static unsigned char *buffer = 0;
  static unsigned int buffer_size = 0;

  CHS::StatusCode req_status, ans_status;
  Smart::CommVoid dummy;
  unsigned int next_query_index = 1 - _query_index;
  req_status = _image_client.queryRequest(*_query_image[next_query_index],_query_id[next_query_index]);
  ans_status = _image_client.queryReceive(_query_id[_query_index],dummy);
  if(ans_status==CHS::SMART_OK)
  {
    if(_query_image[_query_index]->is_data_valid())
    {
      if(_verbose)
      {
        const unsigned long new_seq_count = _query_image[_query_index]->get_sequence_counter();
        if(seq_count!=0)
        {
          if(new_seq_count > seq_count + 1)
          {
            std::cerr << std::endl << "dropped " << new_seq_count - seq_count - 1 << " frames" << std::endl;
          }
        }
        seq_count = new_seq_count;
      }

      const unsigned int size = _query_image[_query_index]->get_size_as_bgr32();
      if(size>buffer_size)
      {
        if(buffer) delete[] buffer;
        buffer = new unsigned char[size];
        buffer_size = size;
      }
      _query_image[_query_index]->get_as_bgr32(buffer);

      QImage image(buffer,_query_image[_query_index]->get_width(),_query_image[_query_index]->get_height(),32,0,0,QImage::LittleEndian);
      _pixmap->convertFromImage(image,0);
      this->_refresh();
    }
    else
    {
      std::cerr << "WARNING: invalid data, format mismatch?" << std::endl;
    }
  }
  else
  {
    std::cerr << "ERROR: failed to receive query answer (" << CHS::StatusCodeConversion(ans_status) << ")" << std::endl;
  }
  if(req_status!=CHS::SMART_OK)
  {
    std::cerr << "ERROR: failed to send query request (" << CHS::StatusCodeConversion(req_status) << ")" << std::endl;
  }
  _query_index = next_query_index;
}

bool GuiThread::eventFilter(QObject *target, QEvent *event)
{
  if(target==_video_widget)
  {
    QPaintEvent *pe = dynamic_cast<QPaintEvent*>(event);
    if(pe)
    {
      this->_refresh(pe);
      return true;
    }
  }
  return false;
}


void GuiThread::_refresh(QPaintEvent *pe)
{
  if(_verbose) std::cerr << ".";
  if(_pixmap)
  {
    QPainter painter(_video_widget);
    if(pe)
    {
      painter.drawPixmap(QPoint(0,0), *_pixmap, pe->rect());
    }
    else
    {
      painter.drawPixmap(0,0,*_pixmap);
    }
  }
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter parameters;
  parameters.addCommandLine("cmdline", argc, argv);

  std::string client_name = "smartVideoClient";
  std::string video_server_name = "smartVideo4LinuxServer";
  std::string video_service_name = "image";

  std::string image_format_name = "rgb24";
//  int image_width = 756;
//  int image_height = 576;
  int image_width = 160;
  int image_height = 120;
  bool verbose = false;

//  parameters.getString("cmdline","name",client_name);
//  parameters.getString("cmdline","service",video_service_name);
//  if(!parameters.getString("cmdline","server",video_server_name))
//  {
//    std::cerr << std::endl << "Use -server=<name of video server> as argument to indicate the video server component." << std::endl;
//    std::cerr << "Using default \"" << video_server_name << "\"." << std::endl << std::endl;
//  }

//  parameters.getString("cmdline","format",image_format_name);
  Smart::CommVideoImage::Format image_format = Smart::CommVideoImage::RGB24;
  Smart::CommVideoImage::string2format(image_format_name,image_format);

//  parameters.getInt("cmdline","width",image_width);
//  parameters.getInt("cmdline","height",image_height);
//
//  parameters.getTruthValue("cmdline","verbose",verbose);

  try
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component(client_name,argc,argv);

    Smart::QtQueryClient<Smart::CommMutableVideoImage,Smart::CommVoid> image_client(&component,video_server_name,video_service_name);

    GuiThread gui_thread(argc, argv, image_width, image_height, image_format, image_client, verbose);

    gui_thread.open();

    component.run();

    threadManager->wait();
  }
  catch  (const CORBA::Exception &)
  {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

