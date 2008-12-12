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
 
#include <math.h>

#include "fawBoundingBoxComputationPainter.hh"

Faw::BoundingBoxComputationPainter::BoundingBoxComputationPainter()
: _bounding_box()
{
}

void Faw::BoundingBoxComputationPainter::_draw_polygon(const std::vector<Faw::Painter::Point> &points, unsigned int num_points)
{
  for(unsigned int i=0; i<num_points; i++)
  {
    _bounding_box.add(points[i][0], points[i][1]);
  }
}

void Faw::BoundingBoxComputationPainter::_draw_cross(double x, double y)
{
  _bounding_box.add(x,y);
}

void Faw::BoundingBoxComputationPainter::_draw_pixel(double x, double y)
{
  _bounding_box.add(x,y);
}

void Faw::BoundingBoxComputationPainter::_draw_node(double x, double y)
{
  _bounding_box.add(x,y);
}

void Faw::BoundingBoxComputationPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  _bounding_box.add(x1,y1);
  _bounding_box.add(x2,y2);
}

void Faw::BoundingBoxComputationPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  const double w = _width_of_ellipse(a,b,alpha);
  const double h = _width_of_ellipse(b,a,alpha);
  _bounding_box.add(x-w,y-h);
  _bounding_box.add(x+w,y+h);
}

void Faw::BoundingBoxComputationPainter::_draw_circle(double x, double y, double r)
{
  _bounding_box.add(x-r,y-r);
  _bounding_box.add(x+r,y+r);
}

void Faw::BoundingBoxComputationPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  _bounding_box.add(x,y);
  if((begin<=2*M_PI)   && (end>=2*M_PI))   _bounding_box.add(x + radius, y);
  if((begin<=M_PI/2)   && (end>=M_PI/2))   _bounding_box.add(x, y + radius);
  if((begin<=M_PI)     && (end>=M_PI))     _bounding_box.add(x - radius, y);
  if((begin<=3*M_PI/2) && (end>=3*M_PI/2)) _bounding_box.add(x, y + radius);
  _bounding_box.add(x + radius * cos(begin), y + radius * sin(begin));
  _bounding_box.add(x + radius * cos(end),   y + radius * sin(end));
}

void Faw::BoundingBoxComputationPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  _bounding_box.add(x1,y1);
  _bounding_box.add(x2,y2);
}

void Faw::BoundingBoxComputationPainter::_draw_text(double x, double y, const char*)
{
  // well, I don't know the size of the text...
  _bounding_box.add(x,y);
}


void Faw::BoundingBoxComputationPainter::_draw_text(double x, double y, const char*, char, char, char)
{
  // I still don't know the size of the text...
  _bounding_box.add(x,y);
}

double Faw::BoundingBoxComputationPainter::_width_of_ellipse(double a, double b, double alpha) const
{
  const double t = atan2(-b*sin(alpha),a*cos(alpha));
  const double s = sin(t);
  const double c = cos(t);
  const double as = a*s;
  const double ac = a*c;
  const double bs = b*s;
  const double bc = b*c;
  const double f = (ac*bc + as*bs) / (as*as + bc*bc);
  const double d1 = bc*f;
  const double d2 = as*f;
  return sqrt(d1*d1 + d2*d2);
}
