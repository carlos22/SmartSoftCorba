// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Christian Schlegel, Andreas Steck, Matthias Lutz
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft SpeechOutput component".
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

// --------------------------------------------------------------------------
//
// CREDITS:
//
// The code for the communication with the mary speech output server was 
// taken from the mary project, which is distributed under there own license, 
// and you can find at http://mary.dfki.de/ .
//
//
// Copyright 2000-2006 DFKI GmbH.
// All Rights Reserved.  Use is subject to license terms.
//
// Permission is hereby granted, free of charge, to use and distribute
// this software and its documentation without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of this work, and to
// permit persons to whom this work is furnished to do so, subject to
// the following conditions:
//
// 1. The code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 2. Any modifications must be clearly marked as such.
// 3. Original authors' names are not deleted.
// 4. The authors' names are not used to endorse or promote products
//    derived from this software without specific prior written
//    permission.
//
// DFKI GMBH AND THE CONTRIBUTORS TO THIS WORK DISCLAIM ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DFKI GMBH NOR THE
// CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
// ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
// THIS SOFTWARE.
//
// --------------------------------------------------------------------------


#include <list>
#include <cstdio>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string>
#include <iostream>
#include "mary_client.hh"

#include "smartSoft.hh"
#include "commSpeechOutputMessage.hh"
#include "commPropertySet.hh"

#define INI_PARAMETERS_FILE  "smartSpeechMaryOutputServer.ini"

std::string inputType;
std::string outputType;
std::string audioType;
std::string voiceType;

// -------------------------------------------------------------------
//  Combined Thread and Handler Class
// -------------------------------------------------------------------

class SpeechOutputWorker:
  public CHS::SmartTask,
  public CHS::SendServerHandler<Smart::CommSpeechOutputMessage>,
  public CHS::QueryServerHandler<Smart::CommSpeechOutputMessage,Smart::CommPropertySet>
{

private:

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

public:
  //contructor
  SpeechOutputWorker(): _queue_sem(0) { }
  
  int svc()
  {
    //address of the mary speechoutput server 
    std::string server_host = "localhost";
    int server_port         = 59125;
    char filename[255];
   
    //temporary file for speechoutput via aplay
    sprintf(filename,"tmp.wav");
    //audio player for system call
    std::string audioplayer = "aplay";

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
        std::string result;
        //get audio stream from mary_server
        mary_query(server_host, server_port, entry.msg.get_message() , inputType, outputType, audioType, voiceType,result);

        std::ofstream myFile ( filename , std::ios::out | std::ios::binary);

        myFile<<result;

        myFile.close();

        // call aplay to play wave file
        std::string audio_string = audioplayer + " "+filename;
        if(system (audio_string.c_str())==0)
        {
          answer.set("success",true);
          answer.set("discarded",false);
        }
        else
        {
          // file open failed
          answer.set("success",false);
          answer.set("discarded",false);
          answer.set("error",std::string("Could not play sound!\n"));
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

};
  

// -------------------------------------------------------------------
// Main
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter parameters;

  try {
    parameters.addFile(argc,argv,"ini");
  } catch(CHS::ParameterError e) {
    try {
      parameters.addFile(INI_PARAMETERS_FILE);
    } catch(CHS::ParameterError e) {
      std::cerr << "WARNING: " << e.what() << std::endl;
    }
  }

  std::string component_name = "smartSpeechOutputServer";
  std::string send_name = "text2speech";
  std::string query_name = "text2speech";

  inputType = "TEXT_DE";
  outputType = "AUDIO";
  audioType = "WAVE";
  voiceType = "hmm-bits1";
 
 
  parameters.getString("component","name",component_name);
  parameters.getString("component","send",send_name);
  parameters.getString("component","query",query_name );

  parameters.getString("mary","inputType",inputType);
  parameters.getString("mary","outputType",outputType);
  parameters.getString("mary","audioType",audioType );
  parameters.getString("mary","voiceType",voiceType );


  try 
  {
    CHS::SmartComponent component(component_name,argc,argv);

    SpeechOutputWorker speech_output_worker;

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

