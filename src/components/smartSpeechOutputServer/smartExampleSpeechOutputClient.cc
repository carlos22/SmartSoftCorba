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
#include <iostream>
#include <sstream>

#include "smartSoft.hh"

#include "commSpeechOutputMessage.hh"

// -------------------------------------------------------------
//
// -------------------------------------------------------------
class ClientThread: public CHS::SmartTask
{
public:
  ClientThread(CHS::SendClient<Smart::CommSpeechOutputMessage> &send_client)
  : _send_client(send_client) 
  {
  }
  
  int svc()
  {
    std::cout << "Enter your text:" << std::endl;
    while(true)
    {
      char ch;
      std::ostringstream oss;
      while(true)
      {
        std::cin.get(ch);
        if(ch=='\n') break;
        oss.put(ch);
      }
      const std::string txt = oss.str();
      if(txt=="quit") break;
      _send_client.send(Smart::CommSpeechOutputMessage(txt));
    }
    return 0;
  }

private:
  CHS::SendClient<Smart::CommSpeechOutputMessage> &_send_client;
};

// -------------------------------------------------------------
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  std::string server_name = "smartSpeechOutputServer";
  std::string send_name = "text2speech";

  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component("smartExampleSpeechOutputClient",argc,argv);

    CHS::SendClient<Smart::CommSpeechOutputMessage> send_client(&component, server_name, send_name);

    ClientThread client_thread(send_client);
    client_thread.open();

    component.run();
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

