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
 
#include "fawDrawingPrintAction.hh"

#include "fawDrawingShapeNode.hh"
#include "fawDrawingTransformationNode.hh"
#include "fawDrawingStyleNode.hh"

Faw::DrawingPrintAction::DrawingPrintAction(std::ostream &os)
  : _os(os), _indent("")
{
}



void Faw::DrawingPrintAction::apply(const Faw::DrawingNode *node)
{
  _os << "+--Scene Graph-----------" << std::endl;
  _indent = "| ";
  node->accept(this);
  _os << "+------------------------" << std::endl;
}


void Faw::DrawingPrintAction::process_group_begin()
{
  _os << _indent << "+--Group Node----------" << std::endl;
  _indent = _indent + "| ";
}



void Faw::DrawingPrintAction::process_group_end()
{
  _indent = _indent.substr(0, _indent.length()-2);
  _os << _indent << "+----------------------" << std::endl;
}



void Faw::DrawingPrintAction::process_shape(const Faw::DrawingShapeNode *shape)
{
  _os << _indent << "Shape: ";
  if(shape)
  {
    shape->print(_os);
  }
  else
  {
    _os << "NULL";
  }
  _os << std::endl;
}



void Faw::DrawingPrintAction::process_transformation(const Faw::DrawingTransformationNode *xform)
{
  _os << _indent << "Transformation: ";
  if(xform)
  {
    xform->get_transformation().print(_os);
  }
  else
  {
    _os << "NULL";
  }
  _os << std::endl;
}



void Faw::DrawingPrintAction::process_style(const Faw::DrawingStyleNode *style)
{
  _os << _indent << "Style: ";
  if(style)
  {
    style->get_style().print(_os);
  }
  else
  {
    _os << "NULL";
  }
  _os << std::endl;
}

