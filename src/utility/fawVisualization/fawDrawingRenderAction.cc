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
 
#include "fawDrawingRenderAction.hh"

#include "fawDrawingShapeNode.hh"
#include "fawDrawingTransformationNode.hh"
#include "fawDrawingStyleNode.hh"

Faw::DrawingRenderAction::DrawingRenderAction(Faw::Painter &painter)
  : _painter(painter)
{
}



void Faw::DrawingRenderAction::apply(const Faw::DrawingNode *node)
{
  _painter.begin();
  node->accept(this);
  _painter.end();
}


void Faw::DrawingRenderAction::process_group_begin()
{
  _painter.begin();
}



void Faw::DrawingRenderAction::process_group_end()
{
  _painter.end();
}



void Faw::DrawingRenderAction::process_shape(const Faw::DrawingShapeNode *shape)
{
  shape->render(_painter);
}



void Faw::DrawingRenderAction::process_transformation(const Faw::DrawingTransformationNode *xform)
{
  _painter.set_transformation(xform->get_transformation());
}



void Faw::DrawingRenderAction::process_style(const Faw::DrawingStyleNode *style)
{
  _painter.set_style(style->get_style());
}

