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


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;

CHS::PushTimedClient<CHS::CommExampleTime> *timeClient;


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
  CHS::CommExampleTime a;
  CHS::StatusCode status;
  double cycle;
  bool   running;

  status = timeClient->getServerInfo(cycle,running);  
  if (status != CHS::SMART_OK) {
    std::cout << "getServerInfo  status " << CHS::StatusCodeConversion(status) << " not ok => abort" << std::endl;
    exit(1);
  } else {
    std::cout << "getServerInfo  status " << CHS::StatusCodeConversion(status) << " cycleTime [seconds] " << cycle << " server started " << running << std::endl;
  }

  int counter=0;

  while(1) {
    if (counter==0) {
      std::cout << "subscribe  status : " << CHS::StatusCodeConversion(timeClient->subscribe(2)) << std::endl;
    }
    if (counter==7) {
      std::cout << "unsubscribe status: " << CHS::StatusCodeConversion(timeClient->unsubscribe()) << std::endl;
    }
    if (counter > 1000) {
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

    component = new CHS::SmartComponent("exampleComponent71",argc,argv);

    timeClient = new CHS::PushTimedClient<CHS::CommExampleTime>(component,"exampleComponent70","time");

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

