//--------------------------------------------------------------------------
//
//  Copyright (C) 2000/2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft positon collect component".
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
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "smartSoft.hh"

#include "commBaseState.hh"

#include "fawXfig.hh"

class MyComponent;


MyComponent *component;

CHS::PushTimedClient<Smart::CommBaseState> *baseClient;

FawXfigClass           drawing;
double                 diffDist;


class MyComponent : public CHS::SmartComponent
{
public:
  MyComponent(const std::string &server, int &argc, char **argv) : SmartComponent(server, argc, argv) {};
  ~MyComponent() {};

protected:
  int handle_signal (int signum, siginfo_t *, ucontext_t *) 
  {
    printf("user exit\n");
    baseClient->unsubscribe();
    drawing.close();
    exit(0);
    return 0;
  }
};



class UserThreadClass : public CHS::SmartTask
{
public:
  UserThreadClass () {};
  ~UserThreadClass () {};
  int svc(void);
private:
  CHS::StatusCode         status;
  Smart::CommBaseState    state;
  Smart::CommBasePosition pos;
};

int UserThreadClass::svc(void)
{
  std::string            file("position.fig");
  double                 x,y,a;
  double                 x_old,y_old,a_old;
  double                 distance;
  int                    count = 0;

  // --------------------------------------------------------------
  // set position of robot / initialisation
  // --------------------------------------------------------------

  baseClient->getUpdateWait(state);
  pos = state.get_base_position();
  x_old = pos.get_x();
  y_old = pos.get_y();
  a_old = pos.get_base_alpha();

  drawing.open(file);
  while(1) {
    baseClient->getUpdateWait(state);
    pos = state.get_base_position();
    x = pos.get_x();
    y = pos.get_y();
    a = pos.get_base_alpha();

    distance = sqrt((x-x_old)*(x-x_old)+(y-y_old)*(y-y_old));
  
    if (distance > diffDist) {
      printf("Neues Ziel gespeicher: %d\n", count );
      drawing.drawLine(x_old,y_old,x,y,DASH_LINE,1,RED);
      x_old = x;
      y_old = y;
      a_old = a;
      count++;
    }
  }
  drawing.close(); // will never be reached
  return 0;
}

// ---------------------------------------------------------
//
// main programm
//
// ---------------------------------------------------------
int main (int argc, char **argv)
{       
  try
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    diffDist = 200;

    component = new MyComponent("positionCollect",argc,argv);

    baseClient = new CHS::PushTimedClient<Smart::CommBaseState>(component,"smartPioneerBaseServer","basestate");
    baseClient->subscribe(1);

    UserThreadClass collector;

    collector.open();

    component->run();

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

