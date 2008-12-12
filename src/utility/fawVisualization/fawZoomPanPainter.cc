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

#include "fawZoomPanPainter.hh"

using namespace Faw;

void ZoomPanPainter::set_to_map(const BoundingBox &source, const BoundingBox &target)
{
  const double h_zoom = target.width()  / source.width();
  const double v_zoom = target.height() / source.height();
  _zoom = (h_zoom<v_zoom)?h_zoom:v_zoom;

  const double cx1 = _zoom * (source.left()   + source.right())/2;
  const double cy1 = _zoom * (source.bottom() + source.top()  )/2;
  const double cx2 =         (target.left()   + target.right())/2;
  const double cy2 =         (target.bottom() + target.top()  )/2;
  _dx = cx2 - cx1;
  _dy = cy2 - cy1;
}

void ZoomPanPainter::align_left(const BoundingBox &source, double x, double y1, double y2)
{
  if(y2<y1)
  {
    const double y = y2; y2 = y1; y1 = y;
  }
  _zoom = (y2 - y1) / source.height();
  _dx = x  - _zoom * source.left();
  _dy = y1 - _zoom * source.bottom();
}

void ZoomPanPainter::align_right(const BoundingBox &source, double x, double y1, double y2)
{
  if(y2<y1)
  {
    const double y = y2; y2 = y1; y1 = y;
  }
  _zoom = (y2 - y1) / source.height();
  _dx = x  - _zoom * source.right();
  _dy = y1 - _zoom * source.bottom();
}

void ZoomPanPainter::align_bottom(const BoundingBox &source, double x1, double x2, double y)
{
  if(x2<x1)
  {
    const double x = x2; x2 = x1; x1 = x;
  }
  _zoom = (x2 - x1) / source.width();
  _dx = x1 - _zoom * source.left();
  _dy = y  - _zoom * source.bottom();
}

void ZoomPanPainter::align_top(const BoundingBox &source, double x1, double x2, double y)
{
  if(x2<x1)
  {
    const double x = x2; x2 = x1; x1 = x;
  }
  _zoom = (x2 - x1) / source.width();
  _dx = x1 - _zoom * source.left();
  _dy = y  - _zoom * source.top();
}

void ZoomPanPainter::begin()
{
  _target.begin();
}

void ZoomPanPainter::end()
{
  _target.end();
}

void ZoomPanPainter::_draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(_point_buffer.size() < num_points)
  {
    _point_buffer.resize(num_points);
  }
  for(unsigned int i=0; i<num_points; ++i)
  {
    _point_buffer[i][0] = _zoom * points[i][0] + _dx;
    _point_buffer[i][1] = _zoom * points[i][1] + _dy;
  }
  _target.draw_polygon(_point_buffer.begin(), _point_buffer.begin() + num_points);
}


void ZoomPanPainter::_draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(_point_buffer.size() < num_points)
  {
    _point_buffer.resize(num_points);
  }
  for(unsigned int i=0; i<num_points; ++i)
  {
    _point_buffer[i][0] = _zoom * points[i][0] + _dx;
    _point_buffer[i][1] = _zoom * points[i][1] + _dy;
  }
  _target.draw_chain(_point_buffer.begin(), _point_buffer.begin() + num_points);
}

void ZoomPanPainter::_draw_cross(double x, double y)
{
  _target.draw_cross(_zoom * x + _dx, _zoom * y + _dy);
}

void ZoomPanPainter::_draw_pixel(double x, double y)
{
  _target.draw_pixel(_zoom * x + _dx, _zoom * y + _dy);
}

void ZoomPanPainter::_draw_node(double x, double y)
{
  _target.draw_node(_zoom * x + _dx, _zoom * y + _dy);
}

void ZoomPanPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  _target.draw_segment(_zoom * x1 + _dx, _zoom * y1 + _dy, _zoom * x2 + _dx, _zoom * y2 + _dy);
}

void ZoomPanPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  _target.draw_line(_zoom * x1 + _dx, _zoom * y1 + _dy, _zoom * x2 + _dx, _zoom * y2 + _dy);
}

void ZoomPanPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  _target.draw_ray(_zoom * x1 + _dx, _zoom * y1 + _dy, _zoom * x2 + _dx, _zoom * y2 + _dy);
}

void ZoomPanPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  _target.draw_ellipse(_zoom * x + _dx, _zoom * y + _dy, _zoom * a, _zoom * b, alpha);
}

void ZoomPanPainter::_draw_circle(double x, double y, double r)
{
  _target.draw_circle(_zoom * x + _dx, _zoom * y + _dy, _zoom * r);
}

void ZoomPanPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  _target.draw_arc(_zoom * x + _dx, _zoom * y + _dy, _zoom * radius, begin, end);
}

void ZoomPanPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  _target.draw_arrow(_zoom * x1 + _dx, _zoom * y1 + _dy, _zoom * x2 + _dx, _zoom * y2 + _dy);
}

void ZoomPanPainter::_draw_text(double x, double y, const char *txt)
{
  _target.draw_text(_zoom * x + _dx, _zoom * y + _dy, txt);
}

void ZoomPanPainter::_draw_text(double x, double y, const char *txt, char orientation, char h_align, char v_align)
{
  _target.draw_text(_zoom * x + _dx, _zoom * y + _dy, txt, orientation, h_align, v_align);
}

void ZoomPanPainter::_style_changed()
{
  _target.set_style(this->get_style());
}

void ZoomPanPainter::_layer_changed()
{
  _target.set_layer(this->get_layer());
}

