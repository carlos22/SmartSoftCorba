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
 
#ifndef FAW_BOUNDING_BOX_COMPUTATION_PAINTER_HH
#define FAW_BOUNDING_BOX_COMPUTATION_PAINTER_HH

#include <cmath>
#include <iostream>
#include <string>
#include <list>

#include "fawPainter.hh"
#include "fawBoundingBox.hh"

namespace Faw {

class BoundingBoxComputationPainter: public Painter
{
public:

  BoundingBoxComputationPainter();

  inline void clear() { _bounding_box.clear(); }
  const BoundingBox &get_bounding_box() const { return _bounding_box; }

protected:

  virtual void _draw_polygon(const std::vector<Faw::Painter::Point> &points, unsigned int num_points);
  virtual void _draw_cross(double x, double y);
  virtual void _draw_pixel(double x, double y);
  virtual void _draw_node(double x, double y);
  virtual void _draw_segment(double x1, double y1, double x2, double y2);
  virtual void _draw_ellipse(double x, double y, double a, double b, double alpha);
  virtual void _draw_circle(double x, double y, double r);
  virtual void _draw_arc(double x, double y, double radius, double begin, double end);
  virtual void _draw_arrow(double x1, double y1, double x2, double y2);
  virtual void _draw_text(double x, double y, const char*);
  virtual void _draw_text(double x, double y, const char*, char orientation, char h_align, char v_align);

private:
  BoundingBox _bounding_box;

  double _width_of_ellipse(double a, double b, double alpha) const;
};

} // namespace Faw

#endif
