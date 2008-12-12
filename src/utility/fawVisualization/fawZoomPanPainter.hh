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

#ifndef FAW_ZOOM_PAN_PAINTER_HH
#define FAW_ZOOM_PAN_PAINTER_HH

#include <cmath>
#include <vector>

#include "fawPainter.hh"
#include "fawBoundingBox.hh"

namespace Faw {

/**
  A painter which delegates zoomed and panned version of drawing commands
  to another painter.
 */
class ZoomPanPainter: public Painter
{
public:
  ZoomPanPainter(Painter &p, double zoom = 1.0, double dx = 0.0, double dy = 0.0)
  : _target(p)
  {
    set_zoom(zoom);
    set_pan(dx,dy);
  }
  
  ZoomPanPainter(Painter &p, const BoundingBox &source, const BoundingBox &target)
  : _target(p)
  {
    set_to_map(source, target);
  }
  
  /**
    Set zoom factor manually.
   */
  inline void set_zoom(double zoom) { _zoom = zoom; }
  
  /**
    Set panning parameters manually.
   */
  inline void set_pan(double dx, double dy) { _dx = dx; _dy = dy; }

  /**
    Compute and set zoom and panning parameters such that drawing 
    into box \a source will appear within the bounds of box \a target.
   */
  void set_to_map(const BoundingBox &source, const BoundingBox &target);

  /**
    Compute and set zoom and panning parameters such that the left border of
    the box \a source mapped to the given vertical segment.
   */
  void align_left(const BoundingBox &source, double x, double y1, double y2);

  /**
    Compute and set zoom and panning parameters such that the right border of
    the box \a source mapped to the given vertical segment.
   */
  void align_right(const BoundingBox &source, double x, double y1, double y2);

  /**
    Compute and set zoom and panning parameters such that the bottom border of
    the box \a source mapped to the given horizontal segment.
   */
  void align_bottom(const BoundingBox &source, double x1, double x2, double y);

  /**
    Compute and set zoom and panning parameters such that the top border of
    the box \a source mapped to the given horizontal segment.
   */
  void align_top(const BoundingBox &source, double x1, double x2, double y);

  /**
    Get the zoom factor.
   */
  inline double get_zoom() const { return _zoom; }

  /**
    Get the panning in x direction.
   */
  inline double get_pan_x() const { return _dx; }

  /**
    Get the panning in y direction.
   */
  inline double get_pan_y() const { return _dy; }
  
protected:

  virtual void begin();
  virtual void end();
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
  virtual void _style_changed();
  virtual void _layer_changed();

private:
  Painter &_target;
  double _zoom;
  double _dx, _dy;

  std::vector<Painter::Point> _point_buffer;
};

} // namespace Faw

#endif
