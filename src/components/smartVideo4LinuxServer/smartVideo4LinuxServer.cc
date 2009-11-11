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


#include "smartSoft.hh"

#include "commVoid.hh"
#include "commVideoImage.hh"

#include "smartVideo4LinuxThread.hh"


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
  std::string component_name = "smartVideo4LinuxServer";
  std::string query_name = "image";

  CHS::SmartParameter parameters;
  bool ini_file_read = false;
  try
  {
    parameters.addFile(argc,argv,"ini");
    ini_file_read = true;
  }
  catch(CHS::ParameterError e)
  {
    std::cerr << "WARNING: " << e.what() << std::endl;
  }
  try
  {
    if(!ini_file_read) parameters.addFile("smartVideo4LinuxServer.ini");
  }
  catch(CHS::ParameterError e)
  {
    std::cerr << "WARNING: " << e.what() << std::endl;
  }

  parameters.getString("component","name",component_name);
  parameters.getString("query","name",query_name);

  try
  {
    CHS::SmartComponent component(component_name,argc,argv);

    Smart::Video4LinuxThread v4l_thread(component, parameters);

    CHS::QueryServer<Smart::CommMutableVideoImage,Smart::CommVoid> image_servant(&component,query_name,v4l_thread);

    v4l_thread.open();

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

