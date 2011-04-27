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

#include <iostream>

#include "smartPropertySet.hh"
#include "smartPropertyItem.hh"

using namespace Smart;

class Handler: public Property::ChangeHandler
{
public:
  virtual void propertyAdded(const Property *origin, const Property::Key *key)
  {
    std::cout << "Property added: key = " << key->toString() << std::endl;
    origin->print();
  }
  virtual void propertyChanged(const Property *origin, const Property::Key *key)
  {
    std::cout << "Property changed:";
    if(key) std::cout << " key = " << key->toString();
    std::cout << std::endl;
    origin->print();
  }
  virtual void propertyRemoved(const Property *origin, const Property::Key *key)
  {
    std::cout << "Property removed: key = " << key->toString() << std::endl;
    origin->print();
  }
};

int main()
{
  PropertySet ps;
  Handler h;
  ps.addChangeHandler(&h);
  
  ps.set("foo1", PropertyItem("bar1"));
  ps.set("foo2", PropertyItem("bar2"));
  ps.set("rec", ps);
  ps.set("foo3", PropertyItem("42"));
  ps.set("rec.test",42);

  ps.print();

  ps.set("rec.sub", new PropertySet);
  
  ps.print();
  
  ps.set("rec.sub.works", true);

  ps.print();

  ps.print();
  
  std::cout << "foo1 = " << ps.getString("foo1") << std::endl;
  std::cout << "rec.foo2 = " << ps.getString("rec.foo2") << std::endl;
  std::cout << "foo3 = " << ps.getInteger("foo3") << std::endl;

  PropertySet ps2 = ps;
  ps2.print();

  ps2.set("rec.sub.subsub", ps2);
  
  ps2.print();

  std::cout << "num_sets==" << ps2.num_sets() << std::endl;
  std::cout << "num_items==" << ps2.num_items() << std::endl;

  if(ps2.getBool("rec.sub.subsub.rec.sub.works"))
  {
    std::cout << "It works!" << std::endl;
  }
  
  ps.getItem("rec.sub.works").setString("well");

  ps.save_xml(std::cout);

  return 0;
}
