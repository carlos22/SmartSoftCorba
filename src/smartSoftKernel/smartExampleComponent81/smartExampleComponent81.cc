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
private:
  CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime>     *timeClient;
  CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult> *calcClient;
public:
  UserThreadA() {};
  ~UserThreadA() {};
  int svc(void);
};

int UserThreadA::svc(void)
{
  CHS::QueryId    id1, id2;
  Smart::CommExampleTime q1,q2;
  Smart::CommExampleTime a1,a2;

  CHS::StatusCode status1, status2;

  int counter = 0;

  //
  //
  //
  try {
    timeClient = new CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime>(component);
  } catch (...) {
    std::cout << "thread A : error in constructor <timeClient>" << std::endl;   
  }
  try {
    calcClient = new CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult>(component);
  } catch (...) {
    std::cout << "thread A : error in constructor <calcClient>" << std::endl;   
  }

  time_t time_now;
  struct tm *time_p;

  while(1) {
    if (counter == 0) {
      std::cout << "thread A connect to <exampleComponent80> ..." << std::endl;
      status1 = timeClient->connect("exampleComponent80","time");
      status2 = calcClient->connect("exampleComponent80","calc");
      std::cout << "thread A connect <exampleComponent80> (time) : " << CHS::StatusCodeConversion(status1) << " " << id1 << std::endl;
      std::cout << "thread A connect <exampleComponent80> (calc) : " << CHS::StatusCodeConversion(status2) << " " << id1 << std::endl;
    }
    if (counter == 10) {
      std::cout << "thread A connect to <exampleComponent82> ..." << std::endl;
      status1 = timeClient->connect("exampleComponent82","time");
      status2 = calcClient->connect("exampleComponent82","calc");
      std::cout << "thread A connect <exampleComponent82> (time) : " << CHS::StatusCodeConversion(status1) << " " << id1 << std::endl;
      std::cout << "thread A connect <exampleComponent82> (calc) : " << CHS::StatusCodeConversion(status2) << " " << id1 << std::endl;
    }

    // interleaved queries
    time_now = time(0);
    time_p   = gmtime(&time_now);

    q1.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status1 = timeClient->queryRequest(q1,id1);

    sleep(1);

    time_now = time(0);
    time_p   = gmtime(&time_now);

    q2.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status2 = timeClient->queryRequest(q2,id2);

    std::cout << "thread A (status1, id1): " << CHS::StatusCodeConversion(status1) << " " << id1 << std::endl;
    std::cout << "thread A (status2, id2): " << CHS::StatusCodeConversion(status2) << " " << id2 << std::endl;

    status1 = timeClient->queryReceiveWait(id1,a1);
    status2 = timeClient->queryReceiveWait(id2,a2);

    if (status1 == CHS::SMART_CANCELLED) timeClient->queryDiscard(id1);
    if (status2 == CHS::SMART_CANCELLED) timeClient->queryDiscard(id2);

    std::cout << "thread A (status1, id1): " << CHS::StatusCodeConversion(status1) << " " << id1 << " "; a1.print();
    std::cout << "thread A (status2, id2): " << CHS::StatusCodeConversion(status2) << " " << id2 << " "; a2.print();

    counter++;
    if (counter > 20) counter = 0;
  }

  return 0;
};


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadB : public CHS::SmartTask
{
private:
  CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime>     *timeClient;
  CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult> *calcClient;
public:
  UserThreadB() {};
  ~UserThreadB() {};
  int svc(void);
};

int UserThreadB::svc(void)
{
  int i=0;
  std::list<int> l;

  Smart::CommExampleValues q;
  Smart::CommExampleResult r;
 
  CHS::QueryId    id;
  CHS::StatusCode status;

  //
  // this thread is always connected to the exampleComponent80
  //
  timeClient = new CHS::QueryClient<Smart::CommExampleTime,Smart::CommExampleTime>(component,"exampleComponent80","time");
  calcClient = new CHS::QueryClient<Smart::CommExampleValues,Smart::CommExampleResult>(component,"exampleComponent80","calc");

  while(1) {
    //
    // first query
    //
    l.clear();
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    q.set(l);

    std::cout << "thread B first query " << i++ << std::endl;

    status = calcClient->query(q,r);

    std::cout << "thread B first query (status): " << CHS::StatusCodeConversion(status) << " result ";r.print();

    //
    // second query
    //
    l.clear();
    l.push_back(10);
    l.push_back(11);
    l.push_back(12);

    q.set(l);

    std::cout << "thread B second query " << i++ << std::endl;

    status = calcClient->queryRequest(q,id);

    std::cout << "thread B second query request (status): " << CHS::StatusCodeConversion(status) << std::endl;
    std::cout << "thread B second query : ... request done ... " << std::endl;

    status = calcClient->queryReceiveWait(id,r);

    std::cout << "thread B second query result (status): " << CHS::StatusCodeConversion(status) << " result ";r.print();
  }

  delete timeClient;
  delete calcClient;

  return 0;
}


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

    component = new CHS::SmartComponent("exampleComponent81",argc,argv);

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

  delete component;

  return 0;
}

