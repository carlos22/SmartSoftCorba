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
 
#include "fawDrawingGenericProxyShapeNode.hh"



Faw::DrawingGenericProxyShapeNode::DrawingGenericProxyShapeNode()
{
}



Faw::DrawingGenericProxyShapeNode::~DrawingGenericProxyShapeNode()
{
  std::map<void*, _PaintableInterface*>::iterator iter = _remote_objects.begin();
  while(iter!=_remote_objects.end())
  {
    delete (*iter).second;
    ++iter;
  }
  _remote_objects.clear();
}



Faw::DrawingGenericProxyShapeNode *Faw::DrawingGenericProxyShapeNode::clone() const
{
  Faw::DrawingGenericProxyShapeNode *gpsn = new Faw::DrawingGenericProxyShapeNode;
  std::map<void*, _PaintableInterface*>::const_iterator iter = _remote_objects.begin();
  while(iter!=_remote_objects.end())
  {
    gpsn->_remote_objects.insert(*iter);
    ++iter;
  }
  return gpsn;
}



void Faw::DrawingGenericProxyShapeNode::render(Faw::Painter &painter) const
{
  std::map<void*, _PaintableInterface*>::const_iterator iter = _remote_objects.begin();
  while(iter!=_remote_objects.end())
  {
    _PaintableInterface *paintable = const_cast<_PaintableInterface*>((*iter).second);
    paintable->paint(painter);
    ++iter;
  }
}



void Faw::DrawingGenericProxyShapeNode::print(std::ostream &os) const
{
  os << "GenericProxyShapeNode: (";
  std::map<void*, _PaintableInterface*>::const_iterator iter = _remote_objects.begin();
  while(iter!=_remote_objects.end())
  {
    (*iter).second->print(os);
    ++iter;
    if(iter!=_remote_objects.end()) os << " ";
  }
  os << ")";
}



