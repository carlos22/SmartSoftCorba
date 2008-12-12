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
 
#ifndef FAW_DRAWING_TRANSFORMATION_NODE_HH
#define FAW_DRAWING_TRANSFORMATION_NODE_HH

#include "fawDrawingNode.hh"
#include "fawDrawingTransformation.hh"

namespace Faw {

class DrawingTransformationNode: public DrawingNode
{
public:
  DrawingTransformationNode() : _xform() {}
  DrawingTransformationNode(const DrawingTransformation &xform) : _xform(xform) {}

  virtual DrawingTransformationNode *clone() const;
  virtual void accept(DrawingAction *action) const;

  const DrawingTransformation &get_transformation() const { return _xform; }

  void set_transformation(const DrawingTransformation &xform) { _xform = xform; }

private:
  DrawingTransformation _xform;
};

} // namespace Faw

#endif
