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

#include <list>

#include <qapplication.h>

#include "smartSoft.hh"

#include "commMobileLaserScan.hh"

#include "fawDrawingWidget.hh"
#include "fawDrawingWidgetRegionZoom.hh"

#include "smartExampleLaserClientGuiThread.hh"
#include "smartDefaultPaintMethods.hh"

#include "smartQtPushNewestClient.hh"

#define INI_PARAMETERS_FILE "smartExampleLaserClient.ini"

using namespace Faw;
// -------------------------------------------------------------
// WARNING:
// THIS IS NOT A GOOD EXAMPLE ON HOW TO VISUALIZE COMPONENTS,
// BUT A QUICK AND DIRTY ONE! BETTER EXAMPLES ARE ON THEIR WAY.
// -------------------------------------------------------------

// -------------------------------------------------------------
//   GUI Thread
// -------------------------------------------------------------
int GuiThread::svc()
{
  QApplication app(_argc, _argv);
  Faw::DrawingWidget drawing_widget;

  Faw::DrawingWidgetRegionZoom *region_zoom = new Faw::DrawingWidgetRegionZoom(&drawing_widget);
  region_zoom->activate(Qt::MidButton | Qt::ControlButton);

  QObject::connect(&_scan_push_newest, SIGNAL(new_data()), this, SLOT(new_data()));
  QObject::connect(this, SIGNAL(model_changed()), &drawing_widget, SLOT(model_changed()));
  QObject::connect(&drawing_widget, SIGNAL(redraw(Faw::Painter&)), this, SLOT(redraw(Faw::Painter&)));

  app.setMainWidget(&drawing_widget);

  drawing_widget.resize(500, 600);
  drawing_widget.fit_view_port(0, 4.5, -3.0, 3.0, 1.0);
  drawing_widget.show();
  
  int ret = 0;
  CHS::StatusCode status;

  status = _scan_push_newest.subscribe();
  if(status==CHS::SMART_OK)
  {
    ret = app.exec();
  }
  else
  {
    ret = -1;
    std::cerr << "ERROR: failed to subscribe (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
  }

  _scan_push_newest.unsubscribe();
  _scan_push_newest.cancel();

  return ret;
}

void GuiThread::new_data()
{
  _scan_push_newest.getUpdate(_scan);
  emit model_changed();
}

void GuiThread::redraw(Faw::Painter &painter)
{
  // the actual commands for drawing a scan are defined in the 
  // DefaultPaintMethod<Smart::CommMobileLaserScan> struct,
  // see file src/utility/smartDefaultPaintMethods/smartDefaultPaintMethods.hh
  painter.draw(_scan);
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{



  try 
  {

    CHS::SmartParameter parameter;
    std::string laser_server_name;
    std::string laser_service_name;

    // read parameters
    try
    {
      parameter.addFile(argc, argv, "ini");
    }
    catch(CHS::ParameterError e)
    {
      try
      {
        parameter.addFile(INI_PARAMETERS_FILE);
      }
      catch(CHS::ParameterError e)
      {
        std::cerr << "No parameter file found." << std::endl;
        return 1;
      }
    }
    laser_server_name = parameter.getString("","laser_server");
    laser_service_name = parameter.getString("","laser_service");


    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    //component 
    CHS::SmartComponent component("smartExampleLaserClient",argc,argv);

    //std::string service_name = "smartLaserServer";
    std::string service_name = "smartHokuyoGalgenLaserServer";
    if(argc >= 2) service_name = argv[1];

    Smart::QtPushNewestClient<Smart::CommMobileLaserScan> scan_push_newest(&component, service_name, "scan");

    GuiThread gui_thread(argc, argv, scan_push_newest);

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

