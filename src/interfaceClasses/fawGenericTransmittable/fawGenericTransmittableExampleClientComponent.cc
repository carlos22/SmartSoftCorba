//--------------------------------------------------------------------------
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
//  This file is part of the "FAW Stream Classes Package".
//  It provides some iostream, streambuf and buffer classes.
//  See file README for more information.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------


#include "smartSoft.hh"
#include "fawGenericTransmittable.hh"

#include "fawGenericTransmittableExampleData.hh"


// Global variables:
// The component instance for this SmartSoft component
CHS::SmartComponent *component;

// Typedef communication class.
// The template is defined such that CommExampleData inherits from
// Faw::ExampleData, adding the set(CORBA::Any) and get(CORBA::Any) methods
// required by SmartSoft. The second template argument ("true") means that
// compressed transmission is to be used.
typedef Faw::GenericTransmittable<Faw::ExampleData,true> CommExampleData;

// Create a user thread. 
// In this thread, we will repeatedly query the user to input some data.
// Then, this data is put into an ExampleData object and sent to the server
// defined in fawExampleServerComponent.cc
class UserThread: public CHS::SmartTask
{
public:
  UserThread() {}
  int svc();
};

int UserThread::svc()
{
  // Create a client object for the servers "print" service
  CHS::SendClient<CommExampleData> *printClient;
  printClient = new CHS::SendClient<CommExampleData>(component,"fawGenericTransmittableExampleServerComponent","print");

  // object to hold the data that is to be sent
  CommExampleData d;

  // Status code returned from the send method.
  // We will ignore it...
  CHS::StatusCode status;

  while(true) 
  {
    std::string name;
    int age;
    double size;

    // Query the user...
    std::cout << "Enter your first name:";
    std::cin >> name;
    std::cout << "Enter your age:";
    std::cin >> age;
    std::cout << "Enter your size [m]:";
    std::cin >> size;

    // ...put the received data into the communication object...
    d.set_string(name);
    d.set_double(size);
    d.set_int(age);

    // ...and send it to the server.
    status = printClient->send(d);

    std::cout << "Your data was sent." << std::endl << std::endl;
  }

  return 0;
};



// The main function.
int main (int argc, char *argv[])
{
  try 
  {
    // get a reference to the thread manager.
    // we will need this later in order not to exit main() before all
    // child threads have exited.
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    // create a component instance for this SmartSoft component
    // with an awfully long name.
    component = new CHS::SmartComponent("genericTransmittableExampleClient",argc,argv);

    // create the user thread...
    UserThread user;

    // ...and start it.
    user.open();

    // Hand over control to SmartSoft. 
    component->run();

    // SmartSoft exited, wait for user threads to terminate
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

