//--------------------------------------------------------------------------
//
//  Copyright (C) 2002 Boris Kluge
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
 
#include "fawDrawingGroupNode.hh"

Faw::DrawingGroupNode::DrawingGroupNode()
{
}



Faw::DrawingGroupNode::~DrawingGroupNode()
{
  clear();
}



Faw::DrawingGroupNode *Faw::DrawingGroupNode::clone() const
{
  Faw::DrawingGroupNode *group = new Faw::DrawingGroupNode;
  children_container::const_iterator iter = _children.begin();
  while(iter!=_children.end())
  {
    group->add((*iter).node->clone());
    ++iter;
  }
  return group;
}

void Faw::DrawingGroupNode::accept(Faw::DrawingAction *action) const
{
  action->process_group_begin();
  children_container::const_iterator iter = _children.begin();
  while(iter!=_children.end())
  {
    (*iter).node->accept(action);
    ++iter;
  }
  action->process_group_end();
}

void Faw::DrawingGroupNode::add(Faw::DrawingNode *child)
{
  if(child)
  {
    child_struct cs;
    cs.node = child;
    _children.push_back(cs);
    child->set_parent(this);
  }
}

void Faw::DrawingGroupNode::remove(const Faw::DrawingNode *child)
{
  children_container::iterator iter = _children.begin();
  while(iter!=_children.end())
  {
    if((*iter).node==child)
    {
      (*iter).node->unset_parent();
      delete (*iter).node;
      _children.erase(iter);
      return;
    }
    ++iter;
  }
}

void Faw::DrawingGroupNode::clear()
{
  while(!_children.empty())
  {
    _children.front().node->unset_parent();
    delete _children.front().node;
    _children.pop_front();
  }
}

bool Faw::DrawingGroupNode::is_child(const Faw::DrawingNode *child) const
{
  children_container::const_iterator iter = _children.begin();
  while(iter!=_children.end())
  {
    if((*iter).node==child) return true;
    ++iter;
  }
  return false;
}


void Faw::DrawingGroupNode::child_changed(const Faw::DrawingNode *child)
{
  if(_get_parent()) _get_parent()->child_changed(child);
}

void Faw::DrawingGroupNode::child_destroyed(const Faw::DrawingNode *child)
{
  children_container::iterator iter = _children.begin();
  while(iter!=_children.end())
  {
    if((*iter).node==child)
    {
      _children.erase(iter);
      return;
    }
    ++iter;
  }
}

