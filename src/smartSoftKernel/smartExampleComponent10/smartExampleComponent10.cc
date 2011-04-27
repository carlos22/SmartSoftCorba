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
//
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;
CHS::PushNewestServer<Smart::CommExampleTime> *timeUpdate;


class UserThreadA : public CHS::SmartTask
{
public:
  UserThreadA() {};
  ~UserThreadA() {};
  int svc(void);
};

int UserThreadA::svc(void)
{
  time_t time_now;
  struct tm *time_p;
  CHS::StatusCode status;
  int i=0;

  Smart::CommExampleTime a;

  while(1) {
    time_now = time(0);
    time_p   = gmtime(&time_now);

    a.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status = timeUpdate->put(a);

    std::cout << "thread A <push new data> " << i++ << " status: " << CHS::StatusCodeConversion(status) << " ";a.print();

    sleep(2);
  }
  return 0;
}

// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  component = new CHS::SmartComponent("exampleComponent10",argc,argv);

  try {
    // Create an object
    timeUpdate = new CHS::PushNewestServer<Smart::CommExampleTime>(component,"time");

    //
    //
    //
    UserThreadA user1;

    //
    //
    //
    user1.open();

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

