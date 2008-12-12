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

#include <sstream>

#include "commExamplePrint.hh"

// -------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------
CHS::SmartComponent *component;


// -------------------------------------------------------------
// Send messages from a timer
// -------------------------------------------------------------

//
// Demonstrates how to use a timer to do some work later on
// See documentation of TimerThread class on how to use
// timers
//
class TimedSender : public CHS::TimerHandler
{
private:
  CHS::SendClient<CHS::CommExamplePrint> sendClient;

public:
  TimedSender() : sendClient(component,"exampleComponent50","print") {};

  virtual void timerExpired(const ACE_Time_Value & absolute_time, const void * arg) {
    std::cout << "timerExpired:" << absolute_time << std::endl;
    CHS::CommExamplePrint m;
    std::ostringstream oss;
    oss << "Timer message: " << absolute_time;
    m.set(0, 0, 0, oss.str());
    sendClient.send(m);
  }
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

    component = new CHS::SmartComponent("exampleComponent52",argc,argv);

    TimedSender periodicSender;

    // create a periodic timer and have it call the periodicSender Object,
    // in 10 seconds from now, and every 2nd second after that
    component->getTimerThread().scheduleTimer(periodicSender, (void*) 0,
						       ACE_Time_Value(10,0),
						       ACE_Time_Value(2,0));

    // add a one shot timer using the same Handler,
    // will fire in 5 sec.
    component->getTimerThread().scheduleTimer(periodicSender, (void*) 0,
						       ACE_Time_Value(5,0));

    // register another timer
    int t3 = component->getTimerThread().scheduleTimer(periodicSender, (void*) 0,
						       ACE_Time_Value(4,0));
    // cancel timer 3.
    component->getTimerThread().cancelTimer(t3);

    component->run();
    threadManager->wait();
  } catch  (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    delete component;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    delete component;
    return 1;
  }

  delete component;
  return 0;
}

