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


// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
CHS::SmartComponent *component;






// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  component = new CHS::SmartComponent("exampleComponent60",argc,argv);

  CHS::SmartParameter parameter;

  try {
    //
    // first read parameter file
    parameter.addFile("exampleComponent60.ini");

    // than add command line arguments to allow overwriting of parameters
    // from file
    parameter.addCommandLine("",argc,argv);

    // print all known parameters
    parameter.print();

  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (const CHS::ParameterError & e) {
    std::cerr << "Exception from parameter handling: " << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  std::cout << "\nExtracting some parameters as example:\n";

  try {
    std::cout << "roboter    : " << parameter.getString("","robot") << std::endl;
    std::cout << "laser type : " << parameter.getString("laser","type") << std::endl;
    std::cout << "baudrate   : " << parameter.getInt("laser","baudrate") << std::endl;
    std::cout << "use laser  : " << parameter.getTruthValue("laser","use") << std::endl;
  } catch (const CHS::ParameterError & e) {
    std::cerr << "Exception from parameter handling:" << e << std::endl;
  }

  delete component;

  return 0;
}

