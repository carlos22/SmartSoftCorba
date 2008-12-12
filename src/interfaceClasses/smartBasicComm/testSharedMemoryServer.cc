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

//
// handler class for the first query service of this component
//
//
class ShmDataQueryHandler: 
  public CHS::QueryServerHandler < Smart::CommSharedMemory<Smart::ShmData>, Smart::CommVoid >
{
public:
  ShmDataQueryHandler(bool interactive) : _interactive(interactive) {}

  void handleQuery
  (
    CHS::QueryServer<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> &server,
    const CHS::QueryId id,
    const Smart::CommSharedMemory<Smart::ShmData> &r
  ) throw()
  {
    std::cout << "server received " << r << std::endl;

    if(r.is_attached())
    {
      // get a mutable (non-const) copy of the request, which will 
      // give us a mutable reference to the shared memory
      Smart::CommSharedMemory<Smart::ShmData> shm = r;

      if(_interactive)
      {
        // fill in the answer interactively:
        
        // get the name of the user, ...
        std::string name;
        std::cout << "name = ";
        std::cin >> name;
        snprintf(shm->name, 199, "%s", name.c_str());
        shm->name[199] = '\0';
        
        // ... his/her size, ...
        std::cout << "size = ";
        std::cin >> shm->size;
        
        // ... and age.
        std::cout << "age  = ";
        std::cin >> shm->age;
      }
      else
      {
        // fill in the answer with random values
        
        const unsigned int len = 10 + (rand() % 50);
        for(unsigned int i=0; i<len; ++i)
        {
          shm->name[i] = 'a' + (rand() % 26);
        }
        shm->name[len] = '\0';
        
        shm->size = 0.001 * (rand() % 20000);
        
        shm->age = (rand() % 90);
      }

      std::cout << "server filled " << *shm << std::endl;
    }
    else
    {
      std::cerr << "Got invalid shared memory." << std::endl;
      sleep(1);
    }

    // signal the client that we're done with dummy data
    server.answer(id,Smart::CommVoid());
  }

private:
  bool _interactive;
};

// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter param;
  param.addCommandLine("cmd",argc,argv);
  bool interactive = false;
  param.getTruthValue("cmd","interactive",interactive);
  
  try 
  {
    CHS::SmartComponent component("testSharedMemoryServer",argc,argv);

    ShmDataQueryHandler handler(interactive);
    CHS::ThreadQueueQueryHandler<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> threadedHandler(handler);
    CHS::QueryServer<Smart::CommSharedMemory<Smart::ShmData>,Smart::CommVoid> servant(&component,"data",threadedHandler);

    component.run();
  } 
  catch (const CORBA::Exception &) 
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
