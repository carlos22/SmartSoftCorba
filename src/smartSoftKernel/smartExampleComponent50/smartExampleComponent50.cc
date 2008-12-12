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

#include "commExamplePrint.hh"

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;


//
// handler of the send service of this component
//
//
class PrintHandler : public CHS::SendServerHandler<CHS::CommExamplePrint>
{
public:
  void handleSend(const CHS::CommExamplePrint& r) throw()
    {
      CHS::CommExamplePrint a;
      std::cout << "print service received time and message: ";
      r.print();
    }
};



// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  component = new CHS::SmartComponent("exampleComponent50",argc,argv);

  try {
    // Create an object
    PrintHandler sendHandler;
    CHS::SendServer<CHS::CommExamplePrint> printServant(component,"print",sendHandler);

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

