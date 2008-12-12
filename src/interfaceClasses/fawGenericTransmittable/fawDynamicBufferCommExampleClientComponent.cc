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

#include <iostream>

#include "smartSoft.hh"
#include "fawDynamicBufferComm.hh"

#include "fawMemoryStreamBuf.hh"

// Create a user thread. 
// In this thread, we will repeatedly query the user to input some data.
// Then, this data is put into an ExampleData object and sent to the server
// defined in fawExampleServerComponent.cc
class UserThread: public CHS::SmartTask
{
public:
  UserThread(CHS::SmartComponent &component) : _component(component) {}
  int svc();

private:
  CHS::SmartComponent &_component;
};

int UserThread::svc()
{
  // Create a client object for the servers "print" service
  CHS::SendClient<Faw::DynamicBufferComm> printClient(&_component,"fawDynamicBufferCommExampleServerComponent","print");

  // object to hold the data that is to be sent
  Faw::DynamicBufferComm buf;

  // Status code returned from the send method.
  // We will ignore it...
  CHS::StatusCode status;

  std::cout << "Enter your text here:" << std::endl;
  while(true) 
  {
    std::string data;

    // Query the user...
    std::cout << ">";
    std::cin >> data;

    // ...put the received data into the communication object...
    Faw::DynamicBufferComm buf;
    Faw::MemoryStreamBuf sb(buf);
    std::ostream os(&sb);

    // ...writing anything you want to be sent into this stream...
    os << data << std::endl << std::flush;

    // ...and send it to the server.
    status = printClient.send(buf);
  }

  return 0;
};



// The main function.
int main (int argc, char *argv[])
{
  // create a component instance for this SmartSoft component
  // with an awfully long name.
  CHS::SmartComponent component("fawDynamicBufferCommExampleClient",argc,argv);

  try 
  {
    // get a reference to the thread manager.
    // we will need this later in order not to exit main() before all
    // child threads have exited.
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    // create the user thread...
    UserThread user(component);

    // ...and start it.
    user.open();

    // Hand over control to SmartSoft. 
    component.run();

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

