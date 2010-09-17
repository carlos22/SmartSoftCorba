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
// derive the state class and provide handler functions
//
// -------------------------------------------------------------------
class PrintStateChangeHandler : public CHS::StateChangeHandler
{
public:
  void handleEnterState(const std::string & s) throw()
    {
      std::cout << "    enterHandler  activate   " << s << std::endl;
    };
  void handleQuitState(const std::string & s) throw()
    {
      std::cout << "    quitHandler   deactivate " << s << std::endl;
    };
};


// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;
CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime> *timeClient;

PrintStateChangeHandler *stateHandler;
CHS::SmartStateServer *state;


// -------------------------------------------------------------------
//
// thread A requires substate "odd" to run ...
//
// -------------------------------------------------------------------
class UserThreadA : public CHS::SmartTask
{
public:
  UserThreadA() {};
  ~UserThreadA() {};
  int svc(void);
};

int UserThreadA::svc(void)
{
  CHS::StatusCode status;
  int v = 1;

  while(1) {
    status = state->acquire("odd");
    if (status == CHS::SMART_OK) {
      v += 2;
      std::cout << "odd " << v << std::endl;
      sleep(1);
    }
    status = state->release("odd");
  }

  return 0;
}


// -------------------------------------------------------------------
//
// thread B requires substate "even" to run ...
//
// -------------------------------------------------------------------
class UserThreadB : public CHS::SmartTask
{
public:
  UserThreadB() {};
  ~UserThreadB() {};
  int svc(void);
};

int UserThreadB::svc(void)
{
  CHS::StatusCode status;
  int v = 0;

  while(1) {
    status = state->acquire("even");
    if (status == CHS::SMART_OK) {
      v += 2;
      std::cout << "even " << v << std::endl;
      sleep(1);
    }
    status = state->release("even");
  }

  return 0;
}


// -------------------------------------------------------------------
//
// thread C runs as long as we are not in the neutral state
//
// since this thread uses a communication pattern, it demonstrates
// how blocking method calls are cancelled if the neutral state is
// enforced and how the state management waits until all substates
// are released if the state management requests a new mainstate.
//
// -------------------------------------------------------------------
class UserThreadC : public CHS::SmartTask
{
public:
  UserThreadC() {};
  ~UserThreadC() {};
  int svc(void);
};

int UserThreadC::svc(void)
{
  Smart::CommExampleTime r,a;
  CHS::StatusCode status;

  r.set(0,0,0);

  while(1) {
    status = state->acquire("nonneutral");
    if (status == CHS::SMART_OK) {
      //
      // we use two blocking queries within the protected section
      // to demonstrate that both blocking queries are cancelled
      // if the neutral state is enforced. In that case the user
      // activity rushes as fast as possible over all blocking
      // communication methods to reach the substate release in
      // an ordered manner as fast as possible
      //

      //
      // blocking query
      //
      std::cout << "time : 1st call : ask for current time ..." << std::endl;

      status = timeClient->query(r,a);

      if (status == CHS::SMART_OK) {
        std::cout << "time : received answer : "; a.print();
      } else {
        std::cout << "time : no answer, query status : " << CHS::StatusCodeConversion(status) << std::endl;
      }

      //
      // blocking query
      //
      std::cout << "time : 2nd call : ask for current time ..." << std::endl;

      status = timeClient->query(r,a);

      if (status == CHS::SMART_OK) {
        std::cout << "time : received answer : "; a.print();
      } else {
        std::cout << "time : no answer, query status : " << CHS::StatusCodeConversion(status) << std::endl;
      }
    
    }
    status = state->release("nonneutral");
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

  try {
    component = new CHS::SmartComponent("exampleComponent20",argc,argv);
    
    // Create an object
    timeClient = new CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime>(component,"exampleComponent22","time");

    stateHandler = new PrintStateChangeHandler();
    state = new CHS::SmartStateServer(component,*stateHandler);

    if (state->defineStates("both","odd")  != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("both","even") != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("odd","odd")   != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("even","even") != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;

    if (state->activate() != CHS::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;

    //
    //
    //
    UserThreadA user1;
    UserThreadB user2;
    UserThreadC user3;

    //
    //
    //
    user1.open();
    user2.open();
    user3.open();

    component->run();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete state;
  delete stateHandler;
  delete component;

  return 0;
}

