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
 
#ifndef FAW_DRAWING_WIDGET_PAINTER_H
#define FAW_DRAWING_WIDGET_PAINTER_H

#include <math.h>
#include <list>
#include <iterator>
#include <set>

#include <qpainter.h>

#include <fawDrawingWidgetTransformation.hh>

#include "fawPainter.hh"
#include "fawBoundingBox.hh"

namespace Faw {

/**
  A Painter similar to Qt's QPainters but customized to our needs (floating point coordinates,
  y coordinates increase upwards, and so on).
  Drawing should be wrapped by calls to \c begin() and \c end() methods.
  \author Boris Kluge
  \date 15.05.2001
 */
class DrawingWidgetPainter: public Painter
{
public:
  DrawingWidgetPainter(QPainter &painter, DrawingWidgetTransformation dwt);

  void hide_layer(unsigned int layer);
  void show_layer(unsigned int layer);

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

  /**
    Used to compute a zoom independent length for painting arrows and nodes.
   */
  static const int SMALL_LENGTH_PIXELS = 4;

  QPainter &_painter;
  DrawingWidgetTransformation _dwt;

  /**
    The set of hidden layers.
   */
  std::set<unsigned int> _hidden_layers;

  /**
    A flag that is \c true if the current layer is not hidden.
   */
  bool _current_layer_visible;

  /**
    Static (shared and reused) buffer for point data.
    Used to draw polygons and polylines (chains).
   */
  static QPointArray _point_buffer;

  /**
    The current view port as a bounding box object.
   */
  BoundingBox _viewport;

  /**
    A zoom independent length for painting arrows and nodes.
    Computed from \sa SMALL_LENGTH_PIXELS.
   */
  double _small_length;
};

} // namespace Faw

#endif
