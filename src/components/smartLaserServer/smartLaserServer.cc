//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License 
//  as published by the Free Software Foundation; either version 2.1 
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along 
//  with this library; if not, write to the Free Software Foundation, Inc., 
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#include "smartSoft.hh"

#include "commVoid.hh"
#include "commMobileLaserScan.hh"

#include "smartLaserThread.hh"

//---------------------------------------------------
// global objects
//---------------------------------------------------

// none :-)

//---------------------------------------------------
// handler classes
//---------------------------------------------------

class LaserScanQueryHandler: public CHS::QueryServerHandler<Smart::CommVoid,Smart::CommMobileLaserScan>
{
public:
  LaserScanQueryHandler
  (
    Smart::CommMobileLaserScan &global_scan, 
    CHS::SmartMutex &global_scan_mutex
  )
  : _global_scan(global_scan), _global_scan_mutex(global_scan_mutex)
  {
  }

  void handleQuery(CHS::QueryServer<Smart::CommVoid, Smart::CommMobileLaserScan> & server,
                   const CHS::QueryId id,
                   const Smart::CommVoid&) throw()
  {
    Smart::CommMobileLaserScan local_scan;
    _global_scan_mutex.acquire();
    local_scan = _global_scan;
    _global_scan_mutex.release();
    server.answer(id,local_scan);
  }

private:
  Smart::CommMobileLaserScan &_global_scan;
  CHS::SmartMutex &_global_scan_mutex;
};

// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  Smart::CommMobileLaserScan global_scan;
  CHS::SmartMutex global_scan_mutex;

  CHS::SmartParameter parameters;
  try {
    parameters.addFile(argc,argv,"ini");
  } catch(CHS::ParameterError e) {
    std::cerr << "WARNING: " << e.what() << std::endl;
    try {
      parameters.addFile("smartLaserServer.ini");
    } catch(CHS::ParameterError e) {
      std::cerr << "WARNING: " << e.what() << std::endl;
    }
  }

  try 
  {
    CHS::SmartComponent component("smartLaserServer",argc,argv);

    // create query servant
    LaserScanQueryHandler 
      laser_scan_query_handler(global_scan, global_scan_mutex);
    CHS::ThreadQueueQueryHandler<Smart::CommVoid,Smart::CommMobileLaserScan> 
      threaded_laser_scan_query_handler(laser_scan_query_handler);    
    CHS::QueryServer<Smart::CommVoid,Smart::CommMobileLaserScan>
      laser_scan_query_servant(&component, "scan", threaded_laser_scan_query_handler);
    
    // create laser thread.
    // the laser thread will create and serve a push newest servant
    Smart::LaserThread laser_thread(component, parameters, global_scan, global_scan_mutex);

    //
    //
    //
    laser_thread.open();

    component.run();
  } 
  catch (const CORBA::Exception &) 
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

