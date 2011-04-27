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

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;

CHS::EventServer<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result,Smart::ExampleEventState> *exampleEvent1;
CHS::EventServer<Smart::CommExampleEvent2Parameter,Smart::CommExampleEvent2Result,Smart::ExampleEventState> *exampleEvent2;


//
// defining test event function
//
class MoreThanTestHandler :
  public CHS::EventTestHandler<Smart::CommExampleEvent1Parameter, Smart::CommExampleEvent1Result, Smart::ExampleEventState>
{
public:
  bool testEvent(Smart::CommExampleEvent1Parameter& p, Smart::CommExampleEvent1Result& e, const Smart::ExampleEventState& s) throw()
  {
    //
    // fire if current counter value is greater or equal to the event parameter
    // when fired return sum of current counter value and event threshold
    //
    int pa, st;

    p.get(pa);
    s.get(st);

    if ( st > pa) {
      e.set ( st );
      return true;
    }
    return false;
  }
};

class IntervalTestHandler :
  public CHS::EventTestHandler<Smart::CommExampleEvent2Parameter, Smart::CommExampleEvent2Result, Smart::ExampleEventState>
{
  bool testEvent(Smart::CommExampleEvent2Parameter& p, Smart::CommExampleEvent2Result& e, const Smart::ExampleEventState& s) throw()
    {
      //
      // report when current counter enters / leaves specified interval
      //
      int pl, pu, ps;
      int st;

      bool result;

      std::string inside("inside");
      std::string outside("outside");

      p.get(pl,pu);
      p.get(ps);
      s.get(st);

      switch (ps) {
      case 0:
	// old state: unknown
	if ((st >= pl) && (st <= pu)) {
	  // now inside => fire inside, new state: inside
	  p.set(1);
	  e.set(inside);
	  result = true;
	} else {
	  // now outside => fire outside, new state: outside
	  p.set(2);
	  e.set(outside);
	  result = true;
	}
	break;
      case 1:
	// old state: inside, fire only if now outside
	if ((st < pl) || (st > pu)) {
	  // now outside
	  p.set(2);
	  e.set(outside);
	  result = true;
	} else {
	  // same state as before
	  result = false;
	}
	break;
      case 2:
	// old state: outside, fire only if now inside
	if ((st >= pl) && (st <= pu)) {
	  // now inside
	  e.set(inside);
	  p.set(1);
	  result = true;
	} else {
	  // same state as before
	  result = false;
	}
	break;
      default:
	// wrong state
	result = false;
	break;
      }

      return result;
    }
};

//
// thread which performs computations which feed the event
//
class EventThread : public CHS::SmartTask
{
public:
  EventThread() {};
  ~EventThread() {};
  int svc(void);
};

int EventThread::svc(void)
{
  Smart::ExampleEventState st;
  int cnt = 0;

  while(1) {
    // compute new state
    cnt++;
    cnt %= 50;

    // put current state into event server object
    std::cout << "internal counter value : " << cnt << std::endl;

    st.set( cnt );
    exampleEvent1->put(st);
    exampleEvent2->put(st);

    sleep(1);
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
  component = new CHS::SmartComponent("exampleComponent40",argc,argv);

  EventThread  userThread;

  try {
    // Create an object

    MoreThanTestHandler moreThanTester;
    exampleEvent1 = new CHS::EventServer<Smart::CommExampleEvent1Parameter,Smart::CommExampleEvent1Result,Smart::ExampleEventState>(component,"eventMoreThan",moreThanTester);

    IntervalTestHandler intervallTester;
    exampleEvent2 = new CHS::EventServer<Smart::CommExampleEvent2Parameter,Smart::CommExampleEvent2Result,Smart::ExampleEventState>(component,"eventInterval",intervallTester);

    userThread.open();

    component->run();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete exampleEvent1;
  delete exampleEvent2;
  delete component;

  return 0;
}

