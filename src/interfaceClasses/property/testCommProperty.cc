// --------------------------------------------------------------------------
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
// --------------------------------------------------------------------------


#include <boost/test/included/unit_test_framework.hpp>

#include <tao/corba.h>

#include "CommProperty.hh"
#include "PropertyFactory.hh"

#include <vector>
#include <string>

using namespace boost::unit_test_framework;
using namespace UTIL;
using namespace std;

void CommProperty_test()
{
  CommProperties props;

  PropertySet * set = PropertyFactory::readXMLFile("test.xml");
  BOOST_CHECK(set);
  props.setProperties(*set);
  std::cerr << "after set" << std::endl;
  // test accessor methods.
  PropertySet * copied_set = props.getProperties();
  BOOST_CHECK(copied_set);
  copied_set->print(std::cerr);
  BOOST_CHECK_EQUAL(copied_set->getItem("value1").getString(),"1");
  BOOST_CHECK_EQUAL(copied_set->getSubProperty("sub_2").getKey(), "sub_2");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.value2").getString(),"2");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.value3").getString(),"3");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.sub_sub_2.value4").getString(),"4");

  // copy though an any.
  CORBA::Any any;
  props.get(any);
  CommProperties props2;
  props2.set(any);

  delete copied_set;
  copied_set = props2.getProperties();
  BOOST_CHECK(copied_set);
  BOOST_CHECK_EQUAL(copied_set->getItem("value1").getString(),"1");
  BOOST_CHECK_EQUAL(copied_set->getSubProperty("sub_2").getKey(), "sub_2");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.value2").getString(),"2");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.value3").getString(),"3");
  BOOST_CHECK_EQUAL(copied_set->getItem("sub_2.sub_sub_2.value4").getString(),"4");

  // change values
  set->getItem("value1").setString("not 1");
  set->getItem("sub_2.value2").setString("not 2");
  set->getItem("sub_2.value3").setString("not 3");
  set->getSet("sub_2").addChild(new PropertyItem("newProperty","newValue"));
  // and make update with the older values
  props2.updateProperties(*set);
  BOOST_CHECK_EQUAL(set->getItem("value1").getString(),"1");
  BOOST_CHECK_EQUAL(set->getItem("sub_2.value2").getString(),"2");
  BOOST_CHECK_EQUAL(set->getItem("sub_2.value3").getString(),"3");
  BOOST_CHECK_EQUAL(set->getItem("sub_2.sub_sub_2.value4").getString(),"4");
  BOOST_CHECK_EQUAL(set->getItem("sub_2.newProperty").getString(),"newValue");

  delete set;

};
test_suite*
init_unit_test_suite( int argc, char** argv)
{
  // should be in the test suite, not here
//  CORBA::ORB_ptr orb = CORBA::ORB_init(argc, argv, "myORB");
  
  test_suite* test= BOOST_TEST_SUITE( "CHS::UTIL::Property tests" );
  test->add(BOOST_TEST_CASE(&CommProperty_test));
  return test;
}

