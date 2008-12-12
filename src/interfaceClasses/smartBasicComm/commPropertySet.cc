// --------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo, Boris Kluge 
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
//
// --------------------------------------------------------------------------

#include <map>
#include <list>

#include "commPropertySet.hh"

using namespace Smart;

void CommPropertySet::get(CORBA::Any &a) const
{
  SmartIDL::PropertySet ps;

  const unsigned int num_sets = this->num_sets();
  ps.sets.length(num_sets);
  const unsigned int num_items = this->num_items();
  ps.items.length(num_items);

  std::map<std::string,Property*>::const_iterator iter;
  unsigned int set_index = 0;
  unsigned int item_index = 0;
  std::list<std::pair<const PropertySet*,unsigned int> > stack;
  
  ps.sets[set_index].key = "root";
  ps.sets[set_index].parent = 0;
  stack.push_front(std::make_pair(this, set_index));
  ++set_index;
  
  while(!stack.empty())
  {
    const PropertySet *prop_set = stack.front().first;
    const unsigned int parent = stack.front().second;
    stack.pop_front();

    iter = this->_get_children(prop_set)->begin();
    while(iter != this->_get_children(prop_set)->end())
    {
      const PropertyItem *leaf = dynamic_cast<const PropertyItem*>((*iter).second);
      if(leaf)
      {
        ps.items[item_index].key = (*iter).first.c_str();
        ps.items[item_index].value = leaf->getString().c_str();
        ps.items[item_index].parent = parent;
        ++item_index;
      }
      else
      {
        const PropertySet *subset = dynamic_cast<const PropertySet*>((*iter).second);
        ps.sets[set_index].key = (*iter).first.c_str();
        ps.sets[set_index].parent = parent;
        stack.push_front(std::make_pair(subset,set_index));
        ++set_index;
      }
      ++iter;
    }
  }
  a <<= ps;
}

void CommPropertySet::set(const CORBA::Any &a)
{
  SmartIDL::PropertySet *ps;
  if(a >>= ps)
  {
    _clear();
    std::map<unsigned int,PropertySet*> sets;
    sets[0] = this;
    for(unsigned int i=1; i < ps->sets.length(); ++i)
    {
      sets[i] = new PropertySet;
      sets[ps->sets[i].parent]->set(std::string(ps->sets[i].key), sets[i]);
    }
    for(unsigned int i=0; i < ps->items.length(); ++i)
    {
      sets[ps->items[i].parent]->set(std::string(ps->items[i].key), ps->items[i].value);
    }
  }
}

