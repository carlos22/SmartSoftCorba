// --------------------------------------------------------------------------
//
//  Copyright (C) 2002 Christian Schlegel
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

#include <CommProperty.hh>
#include <PropertyFactory.hh>
#include <PropertyServerHandler.hh>
#include <Parameter2Property.hh>

// my change handler
class PropListener : public UTIL::Property::ChangeHandler
{
public:
  virtual void propertyChanged(UTIL::Property & origin, const std::string & key)
    {
      std::cerr << "Property changed, origin: " << origin.getCompleteKey()
                << " key: " << key << std::endl;
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
    CHS::SmartComponent component("PropertyComponent",argc,argv);

    // read properties from xml-file
    UTIL::PropertySet * root = 0;
    root = UTIL::PropertyFactory::readXMLFile("properties.xml");
    if (!root) {
      std::cerr << "Could not read properties from xml file" << std::endl;
      return 1;
    }

    // read and add parameters read from an ini file
    CHS::SmartParameter parameter;
    parameter.addFile("parameter.ini");
    UTIL::addParameter2Property(*root,parameter);

    std::cout << "Initial properties: " << std::endl;
    root->print(std::cout);

    PropListener propertyListener;
    root->setChangeHandler(&propertyListener);

    // Create a propertyHandler
    UTIL::PropertyServerHandler propertyHandler(*root);

    // create a send server to recieve properties
    CHS::SendServer<UTIL::CommProperty> propertyWriter(&component,"property",propertyHandler);

    // create a query server that can return our properties
    CHS::QueryServer<UTIL::CommPropertyName,UTIL::CommProperties> propertyExporter(&component,"property",propertyHandler);

    component.run();
  } catch (const CORBA::Exception &) {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

