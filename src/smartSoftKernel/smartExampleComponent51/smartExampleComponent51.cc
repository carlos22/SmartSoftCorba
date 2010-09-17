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

#include "commExamplePrint.hh"


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;


// -------------------------------------------------------------
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
  CHS::SendClient<Smart::CommExamplePrint> *printClient;
  printClient = new CHS::SendClient<Smart::CommExamplePrint>(component,"exampleComponent50","print");

  Smart::CommExamplePrint m;

  time_t time_now;
  struct tm *time_p;

  CHS::StatusCode status;

  while(1) {
    // send message
    time_now = time(0);
    time_p   = gmtime(&time_now);

    m.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec,"Thread A");
    status = printClient->send(m);
  }

  return 0;
};




class UserThreadB : public CHS::SmartTask
{
public:
  UserThreadB() {};
  ~UserThreadB() {};
  int svc(void);
};

int UserThreadB::svc(void)
{
  CHS::SendClient<Smart::CommExamplePrint> *printClient;
  printClient = new CHS::SendClient<Smart::CommExamplePrint>(component,"exampleComponent50","print");

  Smart::CommExamplePrint m;

  time_t time_now;
  struct tm *time_p;

  CHS::StatusCode status;

  while(1) {
    // send message
    time_now = time(0);
    time_p   = gmtime(&time_now);

    m.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec,"Thread B");
    status = printClient->send(m);
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

    component = new CHS::SmartComponent("exampleComponent51",argc,argv);

    //
    //
    //
    UserThreadA user1;
    UserThreadB user2;

    //
    //
    //
    user1.open();
    user2.open();

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

