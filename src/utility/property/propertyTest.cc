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

#include "Property.hh"
#include "PropertyFactory.hh"

#include <vector>
#include <string>

using namespace boost::unit_test_framework;
using namespace UTIL;
using namespace std;


class ChangeRecorder : public Property::ChangeHandler
{
public:
  virtual ~ChangeRecorder() {};
  virtual void propertyChanged(Property & origin, const std::string & path)
    {
      changes.push_back(make_pair(origin.getKey(),path));
    }
  vector<pair<string,string> > changes;
};

void Property_test()
{
  vector<pair<string,string> > changes;

  // property cant do much alone. it just provides the notifications
  ChangeRecorder ch;
  PropertyItem p("key", "0", &ch);
  p.setString("1");
  changes.push_back(make_pair(string("key"),string("")));
  bool change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);

  p.setChangeHandler(0);
  p.setString("2");
  change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);

  p.setChangeHandler(&ch);
  p.setString("3");
  changes.push_back(make_pair(string("key"),string("")));
  change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);

};

void PropertyItem_test()
{
  // constructor
  PropertyItem p("key1","value1",0);
  BOOST_CHECK_EQUAL(p.getKey(),"key1");
  BOOST_CHECK_EQUAL(p.getString(),"value1");

  // copy constructor
  PropertyItem cp(p);
  BOOST_CHECK_EQUAL(cp.getKey(),"key1");
  BOOST_CHECK_EQUAL(cp.getString(),"value1");

  // assignment
  PropertyItem as("1","2");
  as = p;
  BOOST_CHECK_EQUAL(as.getKey(),"key1");
  BOOST_CHECK_EQUAL(as.getString(),"value1");

  PropertyItem not_as("1","2");
  // compare
  BOOST_CHECK(as == p);
  BOOST_CHECK(!(as  == not_as));

  p.setString("144");
  // test extraction
  BOOST_CHECK_EQUAL(p.getString(),"144");
  BOOST_CHECK_EQUAL(p.getInteger(),144);
  BOOST_CHECK_EQUAL(p.getDouble(),144.0);
  BOOST_CHECK_THROW(p.getBool(), PropertyItem::ConversionFailed);

  p.setString("3.1415");
  BOOST_CHECK_EQUAL(p.getInteger(),3);
  BOOST_CHECK_EQUAL(p.getDouble(),3.1415);
  BOOST_CHECK_THROW(p.getBool(), PropertyItem::ConversionFailed);

  p.setString("-3.1415");
  BOOST_CHECK_EQUAL(p.getInteger(),-3);
  BOOST_CHECK_EQUAL(p.getDouble(),-3.1415);
  BOOST_CHECK_THROW(p.getBool(), PropertyItem::ConversionFailed);

  // exceptions
  p.setString("text");
  BOOST_CHECK_THROW(p.getInteger(), Property::ConversionFailed);
  BOOST_CHECK_THROW(p.getDouble(), Property::ConversionFailed);
  BOOST_CHECK_THROW(p.getBool(), PropertyItem::ConversionFailed);

  p.setString("0");
  BOOST_CHECK_EQUAL(p.getBool(),false);
  p.setString("1");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("true");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("TRUE");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("false");
  BOOST_CHECK_EQUAL(p.getBool(),false);
  p.setString("true ");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("false  ");
  BOOST_CHECK_EQUAL(p.getBool(),false);
  p.setString("  true ");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("  false  ");
  BOOST_CHECK_EQUAL(p.getBool(),false);
  p.setString("  yes ");
  BOOST_CHECK_EQUAL(p.getBool(),true);
  p.setString("  no ");
  BOOST_CHECK_EQUAL(p.getBool(),false);

}

void PropertySet_test()
{
  PropertySet root("root");
  BOOST_CHECK_EQUAL(root.getKey(),"root");
  root.addChild(new PropertyItem("1_1","1.1"));
  BOOST_CHECK_EQUAL(root.getItem("1_1").getString(),"1.1");
  BOOST_CHECK_THROW(root["1_2"].getString(), PropertySet::NoSuchProperty);
  root.addChild(new PropertyItem("1_2","1.2"));
  BOOST_CHECK_EQUAL(root["1_1"].getString(),"1.1");
  BOOST_CHECK_EQUAL(root["1_2"].getString(),"1.2");
  BOOST_CHECK_THROW(root["nothing.here"].getString(), PropertySet::NoSuchProperty);
  BOOST_CHECK_THROW(root.getItem("1_2.impossible").getString(), PropertySet::WrongType);

  // operator==
  PropertySet copy_root("root");
  copy_root.addChild(new PropertyItem("1_1","1.1"));
  copy_root.addChild(new PropertyItem("1_2","1.2"));
//  BOOST_CHECK(root == copy_root);

  PropertySet * d1 = new PropertySet("depth_1");
  d1->addChild(new PropertyItem("2_1","2.1"));
  d1->addChild(new PropertyItem("2_2","2.2"));
  root.addChild(d1);
  BOOST_CHECK_EQUAL(root["1_1"].getString(),"1.1");
  BOOST_CHECK_EQUAL(root["depth_1.2_2"].getString(),"2.2");
  BOOST_CHECK_EQUAL(root["depth_1.2_2"].getString(),"2.2");
  BOOST_CHECK_THROW(root["depth_1.blackhole"].getString(), PropertySet::NoSuchProperty);

  PropertySet * d2 = new PropertySet("depth_2");
  d2->addChild(new PropertyItem("3_1","3.1"));
  d2->addChild(new PropertyItem("3_2","3.2"));
  d1->addChild(d2);
  BOOST_CHECK_EQUAL(root["depth_1.depth_2.3_1"].getString(),"3.1");
  BOOST_CHECK_EQUAL(root["depth_1.depth_2.3_2"].getString(),"3.2");
  BOOST_CHECK_THROW(root["depth_1.depth_2.blackhole"].getString(), PropertySet::NoSuchProperty);
  BOOST_CHECK_THROW(root["depth_1.depth_2"].getString(), PropertySet::WrongType);

  BOOST_CHECK_EQUAL(root.getSubProperty("depth_1.depth_2").getCompleteKey(), "root.depth_1.depth_2");
  BOOST_CHECK_EQUAL(root["depth_1.depth_2.3_2"].getCompleteKey(), "root.depth_1.depth_2.3_2");
  
  root.createSubPropertyItem("root.test.item").setString("value");
  root.createSubPropertyItem("depth_1.depth_2.3_1").setString("value2");
  BOOST_CHECK_EQUAL(root.getItem("root.test.item").getString(), "value");
  BOOST_CHECK_EQUAL(root.getItem("depth_1.depth_2.3_1").getString(), "value2");

  // test hierarchical change propagation
  ChangeRecorder ch;
  vector<pair<string,string> > changes;
  root.setChangeHandler(&ch);
  root["1_1"].setString("1.1.1");
  changes.push_back(make_pair(string("root"),string("1_1")));

  bool change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);

  root["depth_1.2_2"].setString("2.2.1");
  changes.push_back(make_pair(string("root"),string("depth_1.2_2")));
  root["depth_1.depth_2.3_2"].setString("3.2.1");
  changes.push_back(make_pair(string("root"),string("depth_1.depth_2.3_2")));
  change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);

  d1->setChangeHandler(&ch);
  root["depth_1.depth_2.3_2"].setString("3.2.2");
  changes.push_back(make_pair(string("depth_1"),string("depth_2.3_2")));
  changes.push_back(make_pair(string("root"),string("depth_1.depth_2.3_2")));
  change_handler = changes == ch.changes;
  BOOST_CHECK(change_handler);
};

void FileReader_test()
{
  PropertySet * root = PropertyFactory::readXMLFile("test.xml");
  BOOST_CHECK(root != 0);

  BOOST_CHECK_EQUAL((*root)["friend"].getString(), "Asterix");
  BOOST_CHECK_EQUAL((*root)["favorite food.comment"].getString(),
		    "yummy");
  BOOST_CHECK_EQUAL((*root)["favorite food.roasted wild pig.ingredient"].getString(),
		    "wild pigs");
  BOOST_CHECK_EQUAL((*root)["favorite food.roasted wild pig.number_pigs"].getInteger(),
		    3);
  // hmm should write a test for this too..
//  root->print(cout);
//  root->printXML(cout);
};

void Property_Key_test()
{
  // constructors
  PropertySet::Key a("1a");
  BOOST_CHECK_EQUAL(a.getKey(),"1a");
  BOOST_CHECK(a.getSubkey() == 0);
  BOOST_CHECK_EQUAL(a.toString(),"1a");

  PropertySet::Key * b = new PropertySet::Key("2a.2b");
  BOOST_CHECK_EQUAL(b->getKey(),"2a");
  BOOST_CHECK_EQUAL(b->getSubkey()->getKey(),"2b");
  BOOST_CHECK(b->getSubkey()->getSubkey() == 0);
  BOOST_CHECK_EQUAL(b->toString(),"2a.2b");

  PropertySet::Key copy("2a.2b");
  // operator==
  BOOST_CHECK(*b == copy);
  BOOST_CHECK(! (a == copy));

  // operator=
  a = *b;
  BOOST_CHECK(a == *b);
  delete b;
  BOOST_CHECK(a == copy);

  PropertySet::Key k(a);
  BOOST_CHECK_EQUAL(a.toString(),"2a.2b");

  PropertySet::Key c(".3b");
  BOOST_CHECK_EQUAL(c.getKey(),"");
  BOOST_CHECK_EQUAL(c.getSubkey()->getKey(),"3b");
  BOOST_CHECK(c.getSubkey()->getSubkey() == 0);
  BOOST_CHECK_EQUAL(c.toString(),".3b");

  PropertySet::Key d("4a.");
  BOOST_CHECK_EQUAL(d.getKey(),"4a");
  BOOST_CHECK_EQUAL(d.getSubkey()->getKey(),"");
  BOOST_CHECK(d.getSubkey()->getSubkey() == 0);
  BOOST_CHECK_EQUAL(d.toString(),"4a.");

}

void Iterator_test()
{
  PropertySet * root = PropertyFactory::readXMLFile("test.xml");
  BOOST_CHECK(root != 0);

  BOOST_CHECK_EQUAL(root->size(), (size_t) 4);
  PropertySet::const_iterator it;
  BOOST_CHECK(it == root->end());
  it = root->begin();

  BOOST_CHECK(it != root->end());
  BOOST_CHECK_EQUAL((*it).first, "Obelix.favorite food.comment");
  BOOST_CHECK_EQUAL((*it).second->getString(), "yummy");
  ++it;
  BOOST_CHECK_EQUAL((*it).first, "Obelix.favorite food.roasted wild pig.ingredient");
  BOOST_CHECK_EQUAL((*it).second->getString(), "wild pigs");

  PropertySet::const_iterator it2(it);
  BOOST_CHECK_EQUAL((*it2).first, "Obelix.favorite food.roasted wild pig.ingredient");
  BOOST_CHECK_EQUAL((*it2).second->getString(), "wild pigs");

  PropertySet::const_iterator it3;
  it3 = it;
  BOOST_CHECK_EQUAL((*it3).first, "Obelix.favorite food.roasted wild pig.ingredient");
  BOOST_CHECK_EQUAL((*it3).second->getString(), "wild pigs");


  ++it;
  BOOST_CHECK_EQUAL((*it).first, "Obelix.favorite food.roasted wild pig.number_pigs");
  BOOST_CHECK_EQUAL((*it).second->getString(), "3");
  ++it2;
  BOOST_CHECK_EQUAL((*it2).first, "Obelix.favorite food.roasted wild pig.number_pigs");
  BOOST_CHECK_EQUAL((*it2).second->getString(), "3");
  ++it3;
  BOOST_CHECK_EQUAL((*it3).first, "Obelix.favorite food.roasted wild pig.number_pigs");
  BOOST_CHECK_EQUAL((*it3).second->getString(), "3");

  BOOST_CHECK(it == it2);
  BOOST_CHECK(it == it3);

  ++it;
  BOOST_CHECK_EQUAL((*it).first, "Obelix.friend");
  BOOST_CHECK_EQUAL((*it).second->getString(), "Asterix");

  ++it;
  ++it2;
  ++it2;
  ++it3;
  ++it3;
  BOOST_CHECK(it == root->end());
  BOOST_CHECK(it2 == root->end());
  BOOST_CHECK(it3 == root->end());
}


test_suite*
init_unit_test_suite( int, char** )
{
  test_suite* test= BOOST_TEST_SUITE( "CHS::UTIL::Property tests" );
  test->add(BOOST_TEST_CASE(&Property_Key_test));
  test->add(BOOST_TEST_CASE(&Property_test));
  test->add(BOOST_TEST_CASE(&PropertyItem_test));
  test->add(BOOST_TEST_CASE(&PropertySet_test));
  test->add(BOOST_TEST_CASE(&FileReader_test));
  test->add(BOOST_TEST_CASE(&Iterator_test));

  return test;
}
