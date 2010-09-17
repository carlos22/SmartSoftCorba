// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2010 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#include "smartSoft.hh"

#include "commExampleTime.hh"

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;


//
// handler class for time queries.
//
class TimeQueryHandler : public CHS::QueryServerHandler<Smart::CommExampleTime,Smart::CommExampleTime>
{
public:
  void handleQuery(CHS::QueryServer<Smart::CommExampleTime,Smart::CommExampleTime> & server,
		     const CHS::QueryId id,
		     const Smart::CommExampleTime& r) throw()
    {
      Smart::CommExampleTime a;

      std::cout << "time service " << id << " received time: ";
      r.print();

      //
      // very slow time service to show how a blocking client
      // communication method can be aborted by the state pattern
      //
      std::cout << "now wait for 6 seconds ..." << std::endl;

      sleep(6);

      time_t time_now = time(0);
      struct tm *time_p = gmtime(&time_now);

      a.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);

      std::cout << "time service " << id << " sent answer time: ";
      a.print();

      server.answer(id,a);
    };
};


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{

  try {
    component = new CHS::SmartComponent("exampleComponent22",argc,argv);
    // Create an object
    TimeQueryHandler timeHandler;
    CHS::QueryServer<Smart::CommExampleTime,Smart::CommExampleTime> timeServant(component,"time", timeHandler);

    component->run();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete component;

  return 0;
}

