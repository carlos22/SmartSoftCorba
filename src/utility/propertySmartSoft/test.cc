//--------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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

#include <boost/test/included/unit_test_framework.hpp>

#include <string>

#include <Property.hh>
#include "Parameter2Property.hh"

using namespace boost::unit_test_framework;
using namespace UTIL;
using namespace std;


void Parameter2Property_test()
{
  PropertySet root("root");
  root.addChild(new PropertyItem("test","value"));
  // read and add parameters read from an ini file
  CHS::SmartParameter parameter;
  parameter.addFile("parameter.ini");
  UTIL::addParameter2Property(root,parameter);
  
  BOOST_CHECK_EQUAL(root.getItem("robot").getString(),"RWI B21");
  BOOST_CHECK_EQUAL(root.getItem("laser.type").getString(),"SICK LMS200");
  BOOST_CHECK_EQUAL(root.getItem("laser.baudrate").getInteger(),57600);
  BOOST_CHECK_EQUAL(root.getItem("laser.scanner_a").getDouble(),0.0);
}


test_suite*
init_unit_test_suite( int, char** )
{
  test_suite* test= BOOST_TEST_SUITE( "CHS::UTIL::Property tests" );
  test->add(BOOST_TEST_CASE(&Parameter2Property_test));

  return test;
}
