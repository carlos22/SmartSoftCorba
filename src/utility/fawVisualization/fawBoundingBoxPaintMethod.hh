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
 
#ifndef FAW_BOUNDING_BOX_PAINT_METHOD_HH
#define FAW_BOUNDING_BOX_PAINT_METHOD_HH

#include "fawBoundingBox.hh"
#include "fawPainter.hh"

namespace Faw {

template<>
struct DefaultPaintMethod<BoundingBox>
{
  inline void operator()(const BoundingBox &bbox, Painter &painter, bool do_highlight) const
  {
    if(do_highlight)
    {
      painter.begin();
      painter.set_pen_width(2*painter.get_pen_width());
    }
    painter.draw_box(bbox.left(), bbox.bottom(), bbox.right(), bbox.top());
    if(do_highlight)
    {
      painter.end();
    }
  }
  inline void operator()(const BoundingBox &bbox, Painter &painter) const
  {
    this->operator()(bbox,painter,false);
  }
};

} // namespace Faw

#endif
