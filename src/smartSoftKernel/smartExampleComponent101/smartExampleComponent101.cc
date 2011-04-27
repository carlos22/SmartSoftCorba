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


//
// handler class for the first query service of this component
//
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
    }
};

//
// handler of the second query service of this component
//
// receives a variable list of values which are summed up and the
// result is returned to the caller
//

class SumQueryHandler :  public CHS::QueryServerHandler<Smart::CommExampleValues,Smart::CommExampleResult>
{
public:
  void handleQuery(CHS::QueryServer<Smart::CommExampleValues,Smart::CommExampleResult> & server,
		     const CHS::QueryId id,
		     const Smart::CommExampleValues& r) throw()

    {
      Smart::CommExampleResult a;
      std::list<int>    l;
      int               result;

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


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{

  try {
    component = new CHS::SmartComponent("exampleComponent101",argc,argv);

    // Create time query and its handler
    TimeQueryHandler timeHandler;
    CHS::QueryServer<Smart::CommExampleTime,Smart::CommExampleTime> timeServant(component,"time",timeHandler);

    // Create sum servant and its handler
    SumQueryHandler calcHandler;
    // register the threaded calc handler
    CHS::QueryServer<Smart::CommExampleValues,Smart::CommExampleResult> calcServant(component,"calc",calcHandler);

    component->run();
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

