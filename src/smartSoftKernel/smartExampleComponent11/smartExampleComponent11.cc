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


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;

CHS::PushNewestClient<Smart::CommExampleTime> *timeClient;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadA : public CHS::SmartTask
{
public:
  UserThreadA() {};
  ~UserThreadA() {};
  int svc(void);
};

int UserThreadA::svc(void)
{
  Smart::CommExampleTime a;
  CHS::StatusCode status;

  int counter=0;

  while(1) {
    if (counter==0) {
      std::cout << "connect status    : " << CHS::StatusCodeConversion(timeClient->connect("exampleComponent10","time")) << std::endl;
    }
    if (counter==1000) {
      std::cout << "subscribe  status : " << CHS::StatusCodeConversion(timeClient->subscribe()) << std::endl;
    }
    if (counter==1007) {
      std::cout << "unsubscribe status: " << CHS::StatusCodeConversion(timeClient->unsubscribe()) << std::endl;
    }
    if (counter==2000) {
      std::cout << "subscribe  status : " << CHS::StatusCodeConversion(timeClient->subscribe()) << std::endl;
    }
    if (counter==2007) {
      std::cout << "disconnect status : " << CHS::StatusCodeConversion(timeClient->disconnect()) << std::endl;
    }
    if (counter > 3000) {
      counter = -1;
    }

    status = timeClient->getUpdateWait(a);
    if (status != CHS::SMART_OK) {
      std::cout << "blocking wait  status " << CHS::StatusCodeConversion(status) << " not ok => retry ..." << std::endl;
    } else {
      std::cout << "blocking wait  status " << CHS::StatusCodeConversion(status) << " ";a.print();
    }

    counter++;
  }

  return 0;
}


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  //
  //
  //
  try {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    component = new CHS::SmartComponent("exampleComponent11",argc,argv);

    timeClient = new CHS::PushNewestClient<Smart::CommExampleTime>(component);

    //
    //
    //
    UserThreadA user;

    //
    //
    //
    user.open();

    component->run();
    threadManager->wait();
  } catch  (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

