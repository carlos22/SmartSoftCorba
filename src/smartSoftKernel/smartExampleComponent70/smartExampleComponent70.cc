// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004 Christian Schlegel
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
//
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;
CHS::PushTimedServer<CHS::CommExampleTime> *timeUpdate;


// -------------------------------------------------------------------
//
// callback method of pushTimed service
//
// -------------------------------------------------------------------
class TimePushHandler : public CHS::PushTimedHandler<CHS::CommExampleTime>
{
  void handlePushTimer(CHS::PushTimedServer<CHS::CommExampleTime> & server) throw()
    {
      time_t time_now;
      struct tm *time_p;
      CHS::StatusCode status;
      int i=0;

      CHS::CommExampleTime a;

      time_now = time(0);
      time_p   = gmtime(&time_now);

      a.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
      status = server.put(a);

      std::cout << "<push timed data> " << i++ << " status: " << CHS::StatusCodeConversion(status) << " ";a.print();
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
    component = new CHS::SmartComponent("exampleComponent70",argc,argv);

    TimePushHandler timeHandler;
    timeUpdate = new CHS::PushTimedServer<CHS::CommExampleTime>(component,"time",timeHandler, 2.0);

    timeUpdate->start();

    component->run();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;

    timeUpdate->stop();

    delete timeUpdate;
    delete component;

    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;

    timeUpdate->stop();

    delete timeUpdate;
    delete component;
    return 1;
  }

  timeUpdate->stop();

  delete timeUpdate;
  delete component;

  return 0;
}

