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
// --------------------------------------------------------------------------

#include "smartSoft.hh"

#include "smartJoystickThread.hh"

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter parameters;
  try {
    parameters.addFile("smartJoystickServer.ini");
  } catch(CHS::ParameterError e) {
    std::cerr << "WARNING: " << e.what() << std::endl;
  }

  try 
  {
    CHS::SmartComponent component("smartJoystickServer",argc,argv);

    Smart::JoystickThread joystick_thread(component, parameters);

    joystick_thread.open();

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

