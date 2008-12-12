// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// --------------------------------------------------------------------------

#include <list>
#include <cstdio>

#include "smartSoft.hh"
#include "commSpeechOutputMessage.hh"
#include "commPropertySet.hh"

// -------------------------------------------------------------------
//  Combined Thread and Handler Class
// -------------------------------------------------------------------

class SpeechOutputWorker:
  public CHS::SmartTask,
  public CHS::SendServerHandler<Smart::CommSpeechOutputMessage>,
  public CHS::QueryServerHandler<Smart::CommSpeechOutputMessage,Smart::CommPropertySet>
{
public:
  SpeechOutputWorker(const CHS::SmartParameter &parameters)
  : _text2speech_executable("/bin/echo"), // dummy value...
    _queue_sem(0)
  {
    parameters.getString("text2speech","player",_text2speech_executable);
  }
  
  int svc()
  {
    while(true)
    {
      // wait for queue entry
      _queue_sem.acquire();

      // pop queue entry
      _queue_mutex.acquire();
      QueueEntry entry = _queue.front();
      _queue.pop_front();
      _queue_mutex.release();
      
      // process queue entry:

      Smart::CommPropertySet answer;
      
      // check time stamp
      if(entry.msg.get_discard_time().is_before(Smart::CommTimeStamp::now()))
      {
        answer.set("success",false);
        answer.set("discarded",true);
      }
      else
      {
        // open pipe to text2speech program
        FILE *text2speech_pipe = ::popen(_text2speech_executable.c_str(), "w");
        if(text2speech_pipe)
        {
          ::fprintf(text2speech_pipe, "%s\n", entry.msg.get_message().c_str());
          if(::pclose(text2speech_pipe)<0)
          {
            answer.set("success",false);
            answer.set("discarded",false);
            answer.set("error",std::string("Failed to close \"") + _text2speech_executable + "\"");
          }
          else
          {
            answer.set("success",true);
            answer.set("discarded",false);
          }
        }
        else
        {
          // pipe open failed
          answer.set("success",false);
          answer.set("discarded",false);
          answer.set("error",std::string("Failed to open \"") + _text2speech_executable + "\"");
        }
      }
      
      // answer query, unless request was plain "send"
      if(entry.query_server)
      {
        entry.query_server->answer(entry.id, answer);
      }
    }
    return 0;
  }
  
  virtual void handleSend(const Smart::CommSpeechOutputMessage& msg) throw()
  {
    _queue_mutex.acquire();
    _queue.push_back(QueueEntry(msg));
    _queue_mutex.release();
    _queue_sem.release();
  }
  
  virtual void handleQuery(CHS::QueryServer<Smart::CommSpeechOutputMessage,Smart::CommPropertySet> &server, const CHS::QueryId id, const Smart::CommSpeechOutputMessage &msg) throw()
  {
    _queue_mutex.acquire();
    _queue.push_back(QueueEntry(msg,server,id));
    _queue_mutex.release();
    _queue_sem.release();
  }

private:

  std::string _text2speech_executable;

  struct QueueEntry
  {
    QueueEntry(const Smart::CommSpeechOutputMessage &m) 
    : msg(m), query_server(0) {}
    QueueEntry(const Smart::CommSpeechOutputMessage &m, CHS::QueryServer<Smart::CommSpeechOutputMessage,Smart::CommPropertySet> &s, CHS::QueryId i)
    : msg(m), query_server(&s), id(i) {}
    Smart::CommSpeechOutputMessage msg;
    CHS::QueryServer<Smart::CommSpeechOutputMessage,Smart::CommPropertySet> *query_server;
    CHS::QueryId id;
  };
  std::list<QueueEntry> _queue;
  CHS::SmartMutex _queue_mutex;
  CHS::SmartSemaphore _queue_sem;
};
  

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter parameters;
  try {
    parameters.addFile(argc,argv,"ini");
  } catch(CHS::ParameterError e) {
    std::cerr << "WARNING: " << e.what() << std::endl;
    try {
      parameters.addFile("smartSpeechOutputServer.ini");
    } catch(CHS::ParameterError e) {
      std::cerr << "WARNING: " << e.what() << std::endl;
    }
  }

  std::string component_name = "smartSpeechOutputServer";
  std::string send_name = "text2speech";
  std::string query_name = "text2speech";

  parameters.getString("component","name",component_name);
  parameters.getString("component","send",send_name);
  parameters.getString("component","query",query_name );

  try 
  {
    CHS::SmartComponent component(component_name,argc,argv);

    SpeechOutputWorker speech_output_worker(parameters);

    CHS::SendServer<Smart::CommSpeechOutputMessage> send_servant(&component,send_name,speech_output_worker);
    CHS::QueryServer<Smart::CommSpeechOutputMessage,Smart::CommPropertySet> query_servant(&component,query_name,speech_output_worker);

    speech_output_worker.open();

    component.run();
  } 
  catch (const CORBA::Exception &) 
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

