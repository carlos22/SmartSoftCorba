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
// --------------------------------------------------------------------------

#include <list>

#include <qapplication.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include "smartSoft.hh"

#include "fawDrawingWidget.hh"
#include "fawDrawingWidgetRegionZoom.hh"
#include "fawBoundingBoxComputationPainter.hh"

#include "smartVisualizationServerGuiThread.hh"

#include "fawTransmittablePainter.hh"

#include "smartQtTemporalSignalMerger.hh"

using namespace Faw;

// -------------------------------------------------------------
//   GUI Thread
// -------------------------------------------------------------
GuiThread::GuiThread
(
  int argc, char **argv,
  const CHS::SmartParameter &parameters,
  Smart::QtSendServerHandler<Smart::CommPropertySet> &registration_handler,
  Smart::QtSendServerHandler<Faw::TransmittablePainter> &drawing_handler
)
// <asteck>
/*
// TODO .ini-file
: _argc(argc), _argv(argv), 
  _view_min_x(-3), _view_max_x(3), _view_min_y(-3), _view_max_y(3),
  _win_width(600), _win_height(600),
  _update_interval(100),
  _registration_handler(registration_handler),
  _drawing_handler(drawing_handler)
*/
: _argc(argc), _argv(argv), 
  _view_min_x(-4), _view_max_x(4), _view_min_y(-4), _view_max_y(4),
  _win_width(1000), _win_height(1000),
  _update_interval(100),
  _registration_handler(registration_handler),
  _drawing_handler(drawing_handler)

// </asteck>
{
  parameters.getInt("cmdline","w",_win_width);
  parameters.getInt("cmdline","h",_win_height);
  parameters.getDouble("cmdline","min_x",_view_min_x);
  parameters.getDouble("cmdline","max_x",_view_max_x);
  parameters.getDouble("cmdline","min_y",_view_min_y);
  parameters.getDouble("cmdline","max_y",_view_max_y);
  parameters.getInt("cmdline","interval",_update_interval);
}


int GuiThread::svc()
{
  QApplication app(_argc, _argv);
  QMainWindow main_window;
/*
  QToolBar *toolbar = new QToolBar(&main_window);
  QToolButton *tool_button;

  tool_button = new QToolButton(Qt::UpArrow, toolbar);
  QObject::connect(tool_button, SIGNAL(clicked()), this, SLOT(view_all()));
  tool_button = new QToolButton(Qt::UpArrow, toolbar);
  tool_button = new QToolButton(Qt::UpArrow, toolbar);
*/
  Faw::DrawingWidget *drawing_widget = new Faw::DrawingWidget(&main_window);

  // draw new frame at most every _update_interval milliseconds
  Smart::QtTemporalSignalMerger signal_merger(_update_interval);

  Faw::DrawingWidgetRegionZoom *region_zoom = new Faw::DrawingWidgetRegionZoom(drawing_widget);
  region_zoom->activate(Qt::MidButton | Qt::ControlButton);

  QObject::connect(&_registration_handler, SIGNAL(new_send()), this, SLOT(handle_registration()));
  QObject::connect(&_drawing_handler, SIGNAL(new_send()), this, SLOT(handle_drawing()));

  QObject::connect(this, SIGNAL(model_changed()), &signal_merger, SLOT(single_activate()));
  QObject::connect(&signal_merger, SIGNAL(merged_activate()), drawing_widget, SLOT(model_changed()));
  QObject::connect(drawing_widget, SIGNAL(redraw(Faw::Painter&)), this, SLOT(redraw(Faw::Painter&)));
  QObject::connect(this, SIGNAL(fit_view_port(double,double,double,double)), drawing_widget, SLOT(fit_view_port(double,double,double,double)));

  drawing_widget->fit_view_port(_view_min_x, _view_max_x, _view_min_y, _view_max_y, 1.0);

  main_window.setCentralWidget(drawing_widget);
  app.setMainWidget(&main_window);

  main_window.resize(_win_width,_win_height);
  main_window.show();
  
  return app.exec();
}

void GuiThread::handle_registration()
{
  Smart::CommPropertySet m;
  while(_registration_handler.pop(m))
  {
    std::string name;
    std::string cmd;
    if(m.tryGetString("name",name) && m.tryGetString("cmd",cmd))
    {
      if(cmd=="add")
      {
        _data.insert(std::make_pair(name,Faw::DrawingStoreReplayPainter(name)));
        std::cout << "Client \"" << name << "\" signed on" << std::endl;
      }
      else if(cmd=="remove")
      {
        _data.erase(name);
        std::cout << "Client \"" << name << "\" signed off" << std::endl;
        emit model_changed();
      }
      else
      {
        std::cerr << "WARNING: unknown command \"" << cmd << "\"" << std::endl;
      }
    }
  }
}

void GuiThread::handle_drawing()
{
  Faw::TransmittablePainter tp;
  while(_drawing_handler.pop(tp))
  {
    const std::string name = tp.get_label();
    std::map<std::string, Faw::DrawingStoreReplayPainter>::iterator iter = _data.find(name);
    if(iter!=_data.end())
    {
      (*iter).second = tp;
      emit model_changed();
    }
    else
    {
      std::cerr << "WARNING: unknown client \"" << name << "\"" << std::endl;
    }
  }
}

void GuiThread::redraw(Faw::Painter &painter)
{
  std::map<std::string,Faw::DrawingStoreReplayPainter>::iterator iter = _data.begin();
  while(iter!=_data.end())
  {
    (*iter).second.replay(painter);
    ++iter;
  }
}

void GuiThread::freeze(bool toggle)
{
}

void GuiThread::save()
{
}

void GuiThread::view_all()
{
  Faw::BoundingBoxComputationPainter bbcp;
  this->redraw(bbcp);
  Faw::BoundingBox box = bbcp.get_bounding_box();
  emit fit_view_port(box.left(), box.right(), box.bottom(), box.top());
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

  std::string server_name = "smartVisualizationServer";
  parameters.getString("cmdline","name",server_name);

  try
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component(server_name,argc,argv);

    Smart::QtSendServerHandler<Smart::CommPropertySet> registration_handler;
    Smart::QtSendServerHandler<Faw::TransmittablePainter> drawing_handler;

    GuiThread gui_thread(argc, argv, parameters, registration_handler, drawing_handler);

    CHS::SendServer<Smart::CommPropertySet> registration_server(&component, "registration", registration_handler);
    CHS::SendServer<Faw::TransmittablePainter> drawing_server(&component, "drawing", drawing_handler);

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

