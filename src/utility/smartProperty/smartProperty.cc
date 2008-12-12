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

#include "smartProperty.hh"

using namespace Smart;

Property::Property()
{
}

Property::~Property() 
{
  std::set<Property::ChangeHandler*>::iterator iter;
  iter = _listeners.begin();
  while(iter!=_listeners.end())
  {
    (*iter)->propertyDestroyed(this);
    ++iter;
  }
}

void Property::_notify_property_added(const Key &key)
{
  std::set<Property::ChangeHandler*>::iterator iter;
  iter = _listeners.begin();
  while(iter!=_listeners.end())
  {
    (*iter)->propertyAdded(this,&key);
    ++iter;
  }
}

void Property::_notify_property_changed(const Key *key)
{
  std::set<Property::ChangeHandler*>::iterator iter;
  iter = _listeners.begin();
  while(iter!=_listeners.end())
  {
    (*iter)->propertyChanged(this,key);
    ++iter;
  }
}

void Property::_notify_property_removed(const Key &key)
{
  std::set<Property::ChangeHandler*>::iterator iter;
  iter = _listeners.begin();
  while(iter!=_listeners.end())
  {
    (*iter)->propertyRemoved(this,&key);
    ++iter;
  }
}

