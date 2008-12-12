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
 
#ifndef FAW_CLIPPING_PAINTER_H
#define FAW_CLIPPING_PAINTER_H

#include "fawPainter.hh"
#include "fawBoundingBox.hh"

namespace Faw {

/**
  A Painter which acts as a filter and forwards clipped drawing commands to another painter.
  This is an instance of the Decorator design pattern.
 */
class ClippingPainter: public Painter
{
public:
  ClippingPainter(Painter &painter, double min_x, double max_x, double min_y, double max_y);
  ClippingPainter(Painter &painter, const BoundingBox &bbox);

  /**
    Draws the clipping rectangle.
    Adds a nice border to the drawing.
   */
  void draw_clipping_rectangle();

protected:

  virtual void _begin();
  virtual void _end();  

  //
  // Drawing
  //

  virtual void _draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points);
  virtual void _draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points);
  virtual void _draw_cross(double x, double y);
  virtual void _draw_pixel(double x, double y);
  virtual void _draw_node(double x, double y);
  virtual void _draw_segment(double x1, double y1, double x2, double y2);
  virtual void _draw_line(double x1, double y1, double x2, double y2);
  virtual void _draw_ray(double x1, double y1, double x2, double y2);
  virtual void _draw_ellipse(double x, double y, double a, double b, double alpha);
  virtual void _draw_circle(double x, double y, double r);
  virtual void _draw_arc(double x, double y, double radius, double begin, double end);
  virtual void _draw_arrow(double x1, double y1, double x2, double y2);
  virtual void _draw_text(double x, double y, const char*);
  virtual void _draw_text(double x, double y, const char*, char orientation, char h_align, char v_align);

  //
  // style
  //

  virtual void _style_changed();
  virtual void _layer_changed();

private:

  Painter &_painter;

  /**
    The clipping rectangle as a bounding box object.
   */
  BoundingBox _clipping_rect;

  /**
    Point buffer for clipped chains
   */
  std::vector<Painter::Point> _point_buffer;

  Painter::Point _enter_point(const Painter::Point &p1, const Painter::Point &p2) const;
  Painter::Point _exit_point(const Painter::Point &p1, const Painter::Point &p2) const;
  bool _check_transit(const Painter::Point &p1, const Painter::Point &p2, Painter::Point &ip1, Painter::Point &ip2) const;
};

} // namespace Faw

#endif
