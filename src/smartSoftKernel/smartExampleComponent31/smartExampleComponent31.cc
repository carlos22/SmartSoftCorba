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

#include "commExampleTime.hh"
#include "commExampleValues.hh"
#include "commExampleResult.hh"


// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
CHS::SmartComponent *component;

CHS::QueryClient<CHS::CommExampleTime,CHS::CommExampleTime> *timeClient;


//
// handler of the calculation query service of this component
//
// receives a variable list of values which are summed up and the result is returned to
// the caller
//

class SumQueryHandler : public CHS::QueryServerHandler<CHS::CommExampleValues,CHS::CommExampleResult>
{
  void handleQuery(CHS::QueryServer<CHS::CommExampleValues,CHS::CommExampleResult> & server, 
		   const CHS::QueryId id, 
		   const CHS::CommExampleValues& r) throw()
    {
      CHS::CommExampleResult a;
      std::list<int>         l;
      int                    result;

      std::cout << "calc service " << id << std::endl;

      r.get(l);
      result = 0;
      for (std::list<int>::iterator i=l.begin();i!=l.end();++i) {
	result += *i;
      }
      a.set(result);

      std::cout << "calc service " << id << " sent answer " << result << std::endl;

      server.answer(id,a);
    }
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
  CHS::QueryId         id1, id2;
  CHS::CommExampleTime q1,q2;
  CHS::CommExampleTime a1,a2;

  CHS::StatusCode status1, status2;

  time_t time_now;
  struct tm *time_p;

  while(1) {
    //
    // interleaved queries
    //
    time_now = time(0);
    time_p   = gmtime(&time_now);

    q1.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status1 = timeClient->queryRequest(q1,id1);

    time_now = time(0);
    time_p   = gmtime(&time_now);

    q2.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status2 = timeClient->queryRequest(q2,id2);

    std::cout << "thread (status1, id1): " << CHS::StatusCodeConversion(status1) << " " << id1 << std::endl;
    std::cout << "thread (status2, id2): " << CHS::StatusCodeConversion(status2) << " " << id2 << std::endl;

    status1 = timeClient->queryReceiveWait(id1,a1);
    status2 = timeClient->queryReceiveWait(id2,a2);

    if (status1 == CHS::SMART_CANCELLED) timeClient->queryDiscard(id1);
    if (status2 == CHS::SMART_CANCELLED) timeClient->queryDiscard(id2);
   
    std::cout << "thread (status1, id1): " << CHS::StatusCodeConversion(status1) << " " << id1 << " "; a1.print();
    std::cout << "thread (status2, id2): " << CHS::StatusCodeConversion(status2) << " " << id2 << " "; a2.print();
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

    component = new CHS::SmartComponent("exampleComponent31",argc,argv);

    //
    // IMPORTANT: first create the servants and then the clients since otherwise
    //            not all services might be available when componentes are trying
    //            to access the servers. This would result in nonfinite waits.
    //
    SumQueryHandler calcHandler;
    CHS::QueryServer<CHS::CommExampleValues,CHS::CommExampleResult> calcServant(component,"calc",calcHandler);

    timeClient = new CHS::QueryClient<CHS::CommExampleTime,CHS::CommExampleTime>(component,"exampleComponent30","time");


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
  } catch  (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete component;

  return 0;
}

