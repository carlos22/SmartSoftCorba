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
#include "fawGenericTransmittable.hh"

#include "fawGenericTransmittableExampleData.hh"

// Global variables:
// The component instance for this SmartSoft component
CHS::SmartComponent *component;

// Typedef communication class.
// The template is defined such that CommExampleData inherits from 
// Faw::ExampleData, adding the set(CORBA::Any) and get(CORBA::Any) methods
// required by SmartSoft. The second template argument ("true") means that
// compressed transmission is to be used.
typedef Faw::GenericTransmittable<Faw::ExampleData,true> CommExampleData;

// Handler of the send service of this component.
// We simply print received data to stdout.
class PrintHandler: public CHS::SendServerHandler<CommExampleData>
{
public:
  void handleSend(CHS::SendServer<CommExampleData> &server, const CommExampleData& d) throw()
  {
    std::cout << "Server: received " << d << std::endl;
  }
};



// The main function.
int main (int argc, char *argv[])
{
  // Create the component instance.
  component = new CHS::SmartComponent("fawGenericTransmittableExampleServerComponent",argc,argv);

  try 
  {
    // Create the handler that will deal with the reception of sent data.
    PrintHandler sendHandler;

    // Create the server object (communication end point) that will receive data
    // which is sent to this component's send service carrying the name "print",
    // and hand it over to the handler object created in the line above.
    CHS::SendServer<CommExampleData> printServant(component,"print",sendHandler);

    // Hand over control to SmartSoft. 
    component->run();
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

  delete component;

  return 0;
}

