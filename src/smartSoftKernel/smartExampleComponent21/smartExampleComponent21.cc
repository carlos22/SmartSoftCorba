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


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;

CHS::SmartStateClient *stateClient;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThread : public CHS::SmartTask
{
public:
  UserThread() {};
  ~UserThread() {};
  int svc(void);
};

int UserThread::svc(void)
{
  std::list<std::string> mainstates;
  std::list<std::string> substates;

  CHS::StatusCode status;

  while(1) {
    //
    // first get list of main states and check whether component is
    // already activated
    //
    status = stateClient->getWaitMainStates(mainstates);
    while (status != CHS::SMART_OK) {
      std::cout << "status still " << CHS::StatusCodeConversion(status) << " ... wait another second" << std::endl;
      sleep(1);
      status = stateClient->getWaitMainStates(mainstates);
    }

    std::cout << std::endl;
    std::cout << "list of states of the slave component : " << std::endl;

    for (std::list<std::string>::iterator iterator1=mainstates.begin();iterator1 != mainstates.end(); ++iterator1) {
      std::cout << "   mainstate : " << *iterator1 << std::endl;
      status = stateClient->getWaitSubStates(*iterator1,substates);
      for (std::list<std::string>::iterator iterator2=substates.begin();iterator2 != substates.end(); ++iterator2) {
	std::cout << "      substate : " << *iterator2 << std::endl;
      }
    }

    std::cout << std::endl;
    std::cout << "Now wait 5 seconds ..." << std::endl;

    sleep(5);

    //
    // now set main state
    //
    std::cout << "set mainstate <both> ..."; std::cout.flush();
    status = stateClient->setWaitState("both");
    if (status == CHS::SMART_OK) {
      std::cout << "... mainstate <both> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <both> : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    sleep(20);

    std::cout << "set mainstate <odd> ..."; std::cout.flush();
    status = stateClient->setWaitState("odd");
    if (status == CHS::SMART_OK) {
      std::cout << "... mainstate <odd> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <odd> : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    sleep(20);

    std::cout << "set mainstate <even> ..."; std::cout.flush();
    status = stateClient->setWaitState("even");
    if (status == CHS::SMART_OK) {
      std::cout << "... mainstate <even> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <even> : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    sleep(20);

    std::cout << "ordered component deactivation ..."; std::cout.flush();
    status = stateClient->setWaitState("neutral");
    if (status == CHS::SMART_OK) {
      std::cout << "... component deactivated (neutral state)" << std::endl;
    } else {
      std::cout << "... error deactivate component : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    sleep(20);

    std::cout << "set mainstate <both> ..."; std::cout.flush();
    status = stateClient->setWaitState("both");
    if (status == CHS::SMART_OK) {
      std::cout << "... mainstate <both> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <both> : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    sleep(20);

    std::cout << "forced component deactivation ..."; std::cout.flush();
    status = stateClient->setWaitState("deactivated");
    if (status == CHS::SMART_OK) {
      std::cout << "... forced deactivation done (neutral)" << std::endl;
    } else {
      std::cout << "... error forced deactivation : " << CHS::StatusCodeConversion(status) << " " << status << std::endl;
    }
  }

  return 0;
};


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

    component = new CHS::SmartComponent("exampleComponent21",argc,argv);

    stateClient = new CHS::SmartStateClient(component,"exampleComponent20");

    //
    //
    //
    UserThread user;

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

