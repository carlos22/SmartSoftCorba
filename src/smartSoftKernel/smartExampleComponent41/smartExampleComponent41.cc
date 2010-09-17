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

#include "commExampleEvent1.hh"
#include "commExampleEvent2.hh"

// -------------------------------------------------------------
//
//
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;

CHS::EventClient<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result> *eventClient1;
CHS::EventClient<Smart::CommExampleEvent2Parameter,Smart::CommExampleEvent2Result> *eventClient2;


// -------------------------------------------------------------
//
// activates continuous "More Than" event
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
  Smart::CommExampleEvent1Parameter parameter;
  Smart::CommExampleEvent1Result    result;
  CHS::EventId               id;

  CHS::StatusCode status;
  int a = 20;
  int b;

  //
  // continuous event
  //
  parameter.set(a);

  status = eventClient1->activate(CHS::continuous, parameter, id);

  while(1) {
    status = eventClient1->getEvent(id, result);
    if (status == CHS::SMART_OK) {
      result.get(b);
      std::cout << "event MORETHAN id: " << id << " event mode: continuous, > " << a << " fired: " << b << "\n";
    }
  }

  status = eventClient1->deactivate(id);

  return 0;
};


// -------------------------------------------------------------
//
// activates / deactivates single "More Than" event
//
// -------------------------------------------------------------
class UserThreadB : public CHS::SmartTask
{
  public:
    UserThreadB() {};
    ~UserThreadB() {};
    int svc(void);
};

int UserThreadB::svc(void)
{
  Smart::CommExampleEvent1Parameter parameter;
  Smart::CommExampleEvent1Result    result;
  CHS::EventId               id;

  CHS::StatusCode status;
  int a = 30;
  int b;

  //
  // single event
  //
  parameter.set(a);

  status = eventClient1->activate(CHS::single, parameter, id);

  status = eventClient1->getEvent(id, result);
  if (status == CHS::SMART_OK) {
    result.get(b);
    std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " fired: " << b << "\n";
  }

  status = eventClient1->getEvent(id, result);
  std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " status getEvent: " << CHS::StatusCodeConversion(status) << "\n";

  status = eventClient1->getNextEvent(id, result);
  std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " status getNextEvent: " << CHS::StatusCodeConversion(status) << "\n";

  status = eventClient1->deactivate(id);
  std::cout << "event MORETHAN id: " << id << " deactivated: " << CHS::StatusCodeConversion(status) << std::endl;

  return 0;
}


// -------------------------------------------------------------
//
// activates continuous interval event in [25,40]
//
// -------------------------------------------------------------
class UserThreadC : public CHS::SmartTask
{
  public:
    UserThreadC() {};
    ~UserThreadC() {};
    int svc(void);
};

int UserThreadC::svc(void)
{
  Smart::CommExampleEvent2Parameter parameter;
  Smart::CommExampleEvent2Result    result;
  CHS::EventId               id;

  int status;
  int l = 25;
  int u = 40;
  std::string b;

  //
  // continuous event
  //
  parameter.set(l,u);

  status = eventClient2->activate(CHS::continuous, parameter, id);

  while(1) {
    status = eventClient2->getEvent(id, result);
    if (status == CHS::SMART_OK) {
      result.get(b);
      std::cout << "event INTERVAL id: " << id << " event mode: continuous, [" << l << " " << u << "] fired: " << b << "\n";
    }
  }

  status = eventClient2->deactivate(id);

  return 0;
};



// -------------------------------------------------------------
//
// demonstrates event handler usage
//
// -------------------------------------------------------------

// -------------------------------------------------------------
// event handler service method
// -------------------------------------------------------------

class MoreThanEventHandler:
  public CHS::EventHandler<Smart::CommExampleEvent1Result>
{
public:
  void handleEvent(const CHS::EventId id, const Smart::CommExampleEvent1Result& e) throw()
    {
      int a;

      e.get(a);
      std::cout << "event handler MORE THAN: id " << id << " event value: " << a << std::endl;
    };
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

    component = new CHS::SmartComponent("exampleComponent41",argc,argv);

    eventClient1 = new CHS::EventClient<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result>(component,"exampleComponent40","eventMoreThan");
    eventClient2 = new CHS::EventClient<Smart::CommExampleEvent2Parameter,Smart::CommExampleEvent2Result>(component,"exampleComponent40","eventInterval");

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

    //
    // demonstrate event handler object
    //
    // activate continuous "more than" event with threshold 40 with event handler
    // event handler is used only for eventClient3 (and not for eventClient1 which is the same event on the server side !)
    //
    Smart::CommExampleEvent1Parameter parameter;
    CHS::EventId               id;

    CHS::EventClient<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result> *eventClient3;
    MoreThanEventHandler eventHandler;

    CHS::StatusCode status;
    int v = 40;

    eventClient3 = new CHS::EventClient<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result>(component,"exampleComponent40","eventMoreThan",eventHandler);
    parameter.set(v);
    status = eventClient3->activate(CHS::continuous, parameter, id);


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

