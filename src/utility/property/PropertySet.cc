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

#include "PropertySet.hh"

#include <smartParameter.hh>

using namespace UTIL;

PropertySet::~PropertySet()
{
  // destroy all children
  std::map<std::string,Property *>::iterator childIt;
  for(childIt = children.begin(); childIt != children.end(); ++childIt) {
    delete childIt->second;
  }
}

void PropertySet::addChild(Property * prop)
{
  std::map<std::string,Property *>::iterator childIt;
  childIt = children.find(prop->getKey());
  if(childIt != children.end())
  {
    // might cause some trouble if somebody holds a reference
    // to the old properties.
    delete childIt->second;
    childIt->second = prop;
  } else {
    children.insert(std::pair<std::string, Property *>(prop->getKey(), prop));
  }
  prop->setParent(this);
}

void PropertySet::printXML(std::ostream & o, int indent) const
{
  std::string spaces(indent,' ');
  o << spaces << "<propertyset name=\"" << getKey() << "\">" << std::endl;
  std::map<std::string,Property *>::const_iterator childIt;
  for(childIt = children.begin(); childIt != children.end(); ++childIt) {
    childIt->second->printXML(o, indent + 2);
  }
  o << spaces << "</propertyset>" << std::endl;
}


void PropertySet::print(std::ostream & o, int indent) const
{
  std::string spaces(indent,' ');
  o << spaces << getKey() << std::endl;
  std::map<std::string,Property *>::const_iterator childIt;
  for(childIt = children.begin(); childIt != children.end(); ++childIt) {
    childIt->second->print(o, indent + 2);
  }
}

bool PropertySet::const_iterator::findNextItem()
{
  while (!itStack.empty()) {
    while ( *(itStack.back().second) != itStack.back().first->children.end()) {
      const Property * prop = (*itStack.back().second)->second;
      const PropertyItem * item = dynamic_cast<const PropertyItem *>(prop);
      if (item) {
	current = item;
	++(*(itStack.back().second));
	return true;
      } else {
	const PropertySet * set = dynamic_cast<const PropertySet*>(prop);
	if (set) {
	  // visit that child.
	  std::map<std::string, Property *>::const_iterator *it = new std::map<std::string, Property *>::const_iterator(set->children.begin());
	  ++(*(itStack.back().second));
	  itStack.push_back(make_pair(set, it));
	} else {
	  throw std::runtime_error("Internal error - propertytree contains unknown items");
	}
      }
    }
    delete itStack.back().second;
    itStack.pop_back();
  }
  current = 0;
  return false;
}

PropertyItem & PropertySet::createSubPropertyItem(const Key & key)
{
  std::map<std::string,Property *>::iterator childIt;
  childIt = children.find(key.getKey());
  if(childIt != children.end())
  {
    if(key.getSubkey())
    {
      PropertySet *s = dynamic_cast<PropertySet*>(childIt->second);
      if(s)
      {
        return s->createSubPropertyItem(*key.getSubkey());
      }
      // expected PropertySet, got PropertyItem
      else throw WrongType(getCompleteKey() + key.getKey());
    }
	
    try {
      return dynamic_cast<PropertyItem&>(getSubProperty(key));
    }
    catch(std::bad_cast & e) {
      throw WrongType(getCompleteKey() + key.getKey());
    }
  }
  if (key.getSubkey()) {
    PropertySet * set = new PropertySet(key.getKey());
    addChild(set);
    return set->createSubPropertyItem(*key.getSubkey());
  } else {
    try {
      // do not overwrite this item if it already exists
      return getItem(key);
    } catch (NoSuchProperty & e) {
      PropertyItem * item = new PropertyItem(key.getKey());
      addChild(item);
      return *item;
    }
  }
}

