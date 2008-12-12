//--------------------------------------------------------------------------
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
//  This file is part of the "FAW Stream Classes Package".
//  It provides some iostream, streambuf and buffer classes.
//  See file README for more information.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------


#include <iostream>

#include "smartSoft.hh"
#include "fawDynamicBufferComm.hh"
#include "fawBufferInputStream.hh"

// Handler of the send service of this component.
// We simply print received data to stdout.
class PrintHandler: public CHS::SendServerHandler<Faw::DynamicBufferComm>
{
public:
  void handleSend(CHS::SendServer<Faw::DynamicBufferComm> &server, const Faw::DynamicBufferComm& buf) throw()
  {
    Faw::BufferInputStream<Faw::DynamicBuffer::const_iterator> is(buf.begin(), buf.end());
    
    // read anything from the istream that was written to it:
    char ch;
    while(is.get(ch).good()) std::cout.put(ch);
  }
};



// The main function.
int main (int argc, char *argv[])
{
  // Create the component instance.
  CHS::SmartComponent component("fawDynamicBufferCommExampleServerComponent",argc,argv);

  try 
  {
    // Create the handler that will deal with the reception of sent data.
    PrintHandler sendHandler;

    // Create the server object (communication end point) that will receive data
    // which is sent to this component's send service carrying the name "print",
    // and hand it over to the handler object created in the line above.
    CHS::SendServer<Faw::DynamicBufferComm> printServant(&component,"print",sendHandler);

    // Hand over control to SmartSoft. 
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

