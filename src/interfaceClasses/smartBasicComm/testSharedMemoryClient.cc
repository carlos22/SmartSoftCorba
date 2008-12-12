// --------------------------------------------------------------------------
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
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------



#include "smartSoft.hh"

#include "commSharedMemory.hh"
#include "commVoid.hh"

#include "testSharedMemoryData.hh"

// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThread : public CHS::SmartTask
{
public:
  UserThread(CHS::QueryClient<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> &client) 
  : _client(client)
  {
  }
  
  int svc()
  {
    Smart::CommVoid dummy;
    CHS::QueryId id;
    while(true)
    {
      // In a real application, you will want to move the alloc() 
      // and the preceeding declaration out of the loop (see below),
      // but here we want to test allocation / deallocation:

      Smart::CommSharedMemory<Smart::ShmData> data;
      data.alloc();

      _client.queryRequest(data,id);
      _client.queryReceiveWait(id,dummy);
      std::cout << *data << std::endl;

      // Now there are two ways of the shared memory being removed:
      //
      // (1) the server already destroyed its reference to the shared memory
      //     and the shared memory itself will be remove from the destructor
      //     of the data instance.
      //
      // (2) the server is somewhat slower than the client and still has its
      //     reference to the shared memory. Then the destructor of the data
      //     instance won't remove the shared memory but will merely decrease
      //     the reference counter. The shared memory will be removed as soon
      //     as the server destroys its reference to it.
      //
      // Using ipcs on my machine, I can sometimes see two segments of shared 
      // memory allocated by this test. That means that this client allocates
      // the next shared memory segment before the server destroys its reference
      // to the previous one (which is case (2) in the list above).
    }


// This is how a "real" application would use the shared memory class:
//
//  CHS::QueryId id;
//  Smart::CommSharedMemory<Smart::ShmData> data;
//  Smart::CommVoid dummy;
//  if(data.alloc()==0)
//  {
//    while(true)
//    {
//      _client.queryRequest(data,id);
//      _client.queryReceiveWait(id,dummy);
//      std::cout << *data << std::endl;
//    }
//  }
//  else
//  {
//    std::cerr << "Failed to allocate shared memory" << std::endl;
//    return -1;
//  }

    return 0;
  }

private:
  CHS::QueryClient<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> &_client;
};

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component("testSharedMemoryClient",argc,argv);

    CHS::QueryClient<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> client(&component,"testSharedMemoryServer","data");

    UserThread user(client);

    user.open();

    component.run();
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

