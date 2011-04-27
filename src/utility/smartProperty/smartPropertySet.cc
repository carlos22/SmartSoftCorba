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

#include "smartPropertySet.hh"
#include "smartPropertyItem.hh"

using namespace Smart;

PropertySet::PropertySet()
{
}


PropertySet::PropertySet(const PropertySet &source)
{
  _copy(source);
}


PropertySet::~PropertySet()
{
  _clear();
}

PropertySet &PropertySet::operator=(const PropertySet &source)
{
  if(&source!=this)
  {
    _clear();
    _copy(source);
  }
  return *this;
}

PropertySet *PropertySet::clone() const
{
  return new PropertySet(*this);
}

unsigned int PropertySet::num_sets() const
{
  unsigned int n = 1;
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = _children.begin();
  while(child_iter!=_children.end())
  {
    n += (*child_iter).second->num_sets();
    ++child_iter;
  }
  return n;
}

unsigned int PropertySet::num_items() const
{
  unsigned int n = 0;
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = _children.begin();
  while(child_iter!=_children.end())
  {
    n += (*child_iter).second->num_items();
    ++child_iter;
  }
  return n;
}

void PropertySet::remove(const Key &key) throw()
{
  const Key *subkey = key.getSubkey();
  if(subkey)
  {
    try
    {
      this->getSet(key.getKey()).remove(*subkey);
    }
    catch(Exception) 
    {
      // ignore exception if item not found.
    }
  }
  else
  {
    std::map<std::string,Property*>::iterator iter = this->_children.find(key.getKey());
    if(iter != this->_children.end())
    {
      _children_inv.erase((*iter).second);
      delete (*iter).second;
      this->_children.erase(iter);
      this->_notify_property_removed(key);
    } 
  }
}

Property &PropertySet::get(const Key &key) throw(Exception)
{
  std::map<std::string,Property*>::iterator child_iter;
  child_iter = _children.find(key.getKey());
  if(child_iter!=_children.end())
  {
    Property *prop = (*child_iter).second;
    const Key *subkey = key.getSubkey();
    if(subkey)
    {
      PropertySet *prop_set = dynamic_cast<PropertySet*>(prop);
      if(prop_set)
      {
        try
        {
          return prop_set->get(*subkey);
        }
        catch(Exception &e)
        {
          e.key = Key(key.getKey(), e.key);
          throw e;
        }
      }
      // expected PropertySet, got PropertyItem
      else 
      {
        throw WrongType(key.getKey());
      }
    }
    else
    {
      return *prop;
    }
  }
  throw NoSuchProperty(key.getKey());
  // dummy return:
  Property *dummy = 0;
  return *dummy;
}

const Property &PropertySet::get(const Key &key) const throw(Exception)
{
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = _children.find(key.getKey());
  if(child_iter!=_children.end())
  {
    const Property *prop = (*child_iter).second;
    const Key *subkey = key.getSubkey();
    if(subkey)
    {
      const PropertySet *prop_set = dynamic_cast<const PropertySet*>(prop);
      if(prop_set)
      {
        try
        {
          return prop_set->get(*subkey);
        }
        catch(Exception &e)
        {
          e.key = Key(key.getKey(), e.key);
          throw e;
        }
      }
      // expected PropertySet, got PropertyItem
      else 
      {
        throw WrongType(key.getKey());
      }
    }
    else
    {
      return *prop;
    }
  }
  throw NoSuchProperty(key.getKey());
  // dummy return:
  Property *dummy = 0;
  return *dummy;
}

PropertyItem &PropertySet::getItem(const Key &key) throw (Exception)
{
  PropertyItem *pi = dynamic_cast<PropertyItem*>(&get(key));
  if(pi==0)  throw WrongType(key);
  return *pi;
}

const PropertyItem &PropertySet::getItem(const Key &key) const throw (Exception)
{
  const PropertyItem *pi = dynamic_cast<const PropertyItem*>(&get(key));
  if(pi==0)  throw WrongType(key);
  return *pi;
}

PropertySet &PropertySet::getSet(const Key &key) throw (Exception)
{
  PropertySet *ps = dynamic_cast<PropertySet*>(&get(key));
  if(ps==0)  throw WrongType(key);
  return *ps;
}

const PropertySet &PropertySet::getSet(const Key &key) const throw (Exception)
{
  const PropertySet *ps = dynamic_cast<const PropertySet*>(&get(key));
  if(ps==0)  throw WrongType(key);
  return *ps;
}

std::string PropertySet::getString(const Key &key) const throw(Exception)
{
  return this->getItem(key).getString();
}

bool PropertySet::tryGetString(const Key &key, std::string &s) const throw()
{
  try {
    s = this->getItem(key).getString();
    return true;
  } catch(Exception) {}
  return false;
}

int PropertySet::getInteger(const Key &key) const throw(Exception)
{
  return this->getItem(key).getInteger();
}

double PropertySet::getDouble(const Key &key) const throw(Exception)
{
  return this->getItem(key).getDouble();
}

bool PropertySet::getBool(const Key &key) const throw(Exception)
{
  return this->getItem(key).getBool();
}

bool PropertySet::tryGetBool(const Key &key, bool &b) const throw()
{
  try {
    b = this->getItem(key).getBool();
    return true;
  } catch(Exception) {}
  return false;
}

void PropertySet::save_xml(std::ostream &os, const std::string &indent) const
{
  const std::string more_indent = indent + "    ";
  os << indent << "<property-set>" << std::endl;
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = _children.begin();
  while(child_iter!=_children.end())
  {
    os << indent << "  <entry key=\"" << (*child_iter).first << "\">" << std::endl;
    (*child_iter).second->save_xml(os, more_indent);
    os << indent << "  </entry>" << std::endl;
    ++child_iter;
  }
  os << indent << "</property-set>" << std::endl;
}


void PropertySet::print(std::ostream &os, const std::string &indent) const
{
  const std::string more_indent = indent + "    ";
  os << indent << "PropertySet {" << std::endl;
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = _children.begin();
  while(child_iter!=_children.end())
  {
    os << indent << "  Entry \"" << (*child_iter).first << "\" {" << std::endl;
    (*child_iter).second->print(os, more_indent);
    os << indent << "  }" << std::endl;
    ++child_iter;
  }
  os << indent << "}" << std::endl;
}

void PropertySet::_set_impl(const Key &key, Property *prop) throw(Exception)
{
  const Key *subkey = key.getSubkey();
  if(subkey)
  {
    try
    {
      this->getSet(key.getKey())._set_impl(*subkey, prop);
    }
    catch(Exception &e)
    {
      e.key = Key(key.getKey(), e.key);
      throw e;
    }
  }
  else
  {
    std::map<std::string,Property*>::iterator iter = this->_children.find(key.getKey());
    if(iter != this->_children.end())
    {
      _children_inv.erase((*iter).second);
      delete (*iter).second;
      this->_children.erase(iter);
      this->_notify_property_removed(key);
    } 
    this->_children.insert(std::pair<std::string,Property*>(key.getKey(), prop));
    this->_children_inv.insert(std::pair<const Property*,std::string>(prop,key.getKey()));
    prop->addChangeHandler(this);
    this->_notify_property_added(key);
  }
}

void PropertySet::_clear()
{
  // destroy all children
  std::map<std::string,Property*>::iterator child_iter;
  child_iter = _children.begin();
  while(child_iter!=_children.end())
  {
    const std::string key = (*child_iter).first;
    _children_inv.erase((*child_iter).second);
    delete (*child_iter).second;
    _children.erase(child_iter);
    this->_notify_property_removed(key);
    child_iter = _children.begin();
  }
}

void PropertySet::_copy(const PropertySet &source)
{
  std::map<std::string,Property*>::const_iterator child_iter;
  child_iter = source._children.begin();
  while(child_iter!=source._children.end())
  {
    this->set((*child_iter).first, (*child_iter).second->clone());
    ++child_iter;
  }
}

void PropertySet::propertyAdded(const Property *origin, const Key *subkey)
{
  const Key key(this->_children_inv[origin], subkey);
  this->_notify_property_added(key);
}

void PropertySet::propertyChanged(const Property *origin, const Key *subkey)
{
  const Key key(this->_children_inv[origin], subkey);
  this->_notify_property_changed(&key);
}

void PropertySet::propertyRemoved(const Property *origin, const Key *subkey)
{
  const Key key(this->_children_inv[origin], subkey);
  this->_notify_property_removed(key);
}
