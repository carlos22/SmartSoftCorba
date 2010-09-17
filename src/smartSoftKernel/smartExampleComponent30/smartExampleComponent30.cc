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
#include "commExampleValues.hh"
#include "commExampleResult.hh"


// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;

CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult> *calcClient;


//
// handler class for the time query service
//
class TimeQueryHandler : public CHS::QueryServerHandler<Smart::CommExampleTime,Smart::CommExampleTime>
{
public:
  void handleQuery(CHS::QueryServer<Smart::CommExampleTime, Smart::CommExampleTime> & server,
		   const CHS::QueryId id,
		   const Smart::CommExampleTime& r) throw()
    {
      Smart::CommExampleTime a;

      std::cout << "time service " << id << " received time: ";
      r.print();

      time_t time_now = time(0);
      struct tm *time_p = gmtime(&time_now);

      a.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);

      std::cout << "time service " << id << " sent answer time: ";
      a.print();

      server.answer(id,a);
    };
};

// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThread : public CHS::SmartTask
{
public:
  UserThread() {};
  ~UserThread() {};
  int svc(void);
};

int UserThread::svc(void)
{
  int i=0;
  std::list<int> l;

  Smart::CommExampleValues q;
  Smart::CommExampleResult r;

  CHS::StatusCode status;

  while(1) {
    l.clear();
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    q.set(l);

    std::cout << "thread " << i++ << std::endl;

    status = calcClient->query(q,r);

    std::cout << "thread (status): " << CHS::StatusCodeConversion(status) << " result ";r.print();
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
  //
  //
  //
  try {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    component = new CHS::SmartComponent("exampleComponent30",argc,argv);

    //
    // IMPORTANT: first create the servants and then the clients since otherwise
    //            not all services might be available when components are trying
    //            to access the servers. This would result in nonfinite waits.
    //
    TimeQueryHandler timeHandler;
    CHS::QueryServer<Smart::CommExampleTime,Smart::CommExampleTime> timeServant(component,"time",timeHandler);

    calcClient = new CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult>(component,"exampleComponent31","calc");

    //
    //
    //
    UserThread user;

    //
    //
    //
    user.open();

    component->run();
    threadManager->wait();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete component;

  return 0;
}

