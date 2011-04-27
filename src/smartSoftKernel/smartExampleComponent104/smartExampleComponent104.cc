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

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;


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
  CHS::WiringMaster wiringmaster(component);
  CHS::StatusCode   status;

  std::cout << "Demonstrate dynamic wiring" << std::endl;
  std::cout << std::endl;
  std::cout << "Does not use the state class and therefore rewiring is" << std::endl;
  std::cout << "done without making sure that the client is in a suitable" << std::endl;
  std::cout << "state for changing the server connections." << std::endl;
  std::cout << std::endl;

  while (1) {
    std::cout << "slave ports are not wired ... " << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);

    std::cout << "CONnect <calcPort> of <exampleComponent102> to <exampleComponent101,calc> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","calcPort","exampleComponent101","calc");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);

    std::cout << "CONnect <timePort> of <exampleComponent102> to <exampleComponent101,time> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","timePort","exampleComponent101","time");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);

    std::cout << "CONnect <calcPort> of <exampleComponent102> to <exampleComponent103,calc> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","calcPort","exampleComponent103","calc");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);

    std::cout << "CONnect <timePort> of <exampleComponent102> to <exampleComponent103,time> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","timePort","exampleComponent103","time");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);
    
    std::cout << "DISconnect <calcPort> of <exampleComponent102> ..." << std::endl;
    status = wiringmaster.disconnect("exampleComponent102","calcPort");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    sleep(5);

    std::cout << "DISconnect <timePort> of <exampleComponent102> ..." << std::endl;
    status = wiringmaster.disconnect("exampleComponent102","timePort");
    std::cout << "... : " << CHS::StatusCodeConversion(status) << std::endl;
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

    component  = new CHS::SmartComponent("exampleComponent104",argc,argv);

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

