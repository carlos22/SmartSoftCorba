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
 
#include "fawClippingPainter.hh"

using namespace Faw;

ClippingPainter::ClippingPainter(Painter &painter, double min_x, double max_x, double min_y, double max_y)
: _painter(painter),
  _clipping_rect(min_x, max_y, max_x, min_y)
{
}

ClippingPainter::ClippingPainter(Painter &painter, const BoundingBox &bbox)
: _painter(painter),
  _clipping_rect(bbox)
{
}

void ClippingPainter::_draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(num_points>1)
  {
    this->_draw_chain(points,num_points);
    if(num_points>2)
    {
      const unsigned int last = num_points - 1;
      this->_draw_segment(points[last][0],points[last][1],points[0][0],points[0][1]);
    }
  }
}

void ClippingPainter::_draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(num_points>1)
  {
    if(_point_buffer.size()<num_points) _point_buffer.resize(num_points);
    unsigned int idx = 0;
    bool prev_inside = _clipping_rect.contains(points[0][0],points[0][1]);
    if(prev_inside) _point_buffer[idx++] = points[0];
    for(unsigned int i=1; i<num_points; ++i)
    {
      const bool inside = _clipping_rect.contains(points[i][0],points[i][1]);
      if(prev_inside)
      {
        if(inside)
        {
          _point_buffer[idx++] = points[i];
        }
        else
        {
          _point_buffer[idx++] = _exit_point(points[i-1],points[i]);
          _painter.draw_chain(_point_buffer.begin(), _point_buffer.begin() + idx);
          idx = 0;
        }
      }
      else
      {
        if(inside)
        {
          _point_buffer[idx++] = _enter_point(points[i-1],points[i]);
          _point_buffer[idx++] = points[i];
        }
        else
        {
          Painter::Point p1,p2;
          if(_check_transit(points[i-1],points[i],p1,p2))
          {
            _painter.draw_segment(p1,p2);
          }
        }
      }
      prev_inside = inside;
    }
    if(idx>0)
    {
      _painter.draw_chain(_point_buffer.begin(), _point_buffer.begin() + idx);
    }
  }
  else if(num_points==1)
  {
    this->_draw_pixel(points[0][0],points[0][1]);
  }
}

void ClippingPainter::_draw_cross(double x, double y)
{
  if(_clipping_rect.contains(x,y))
  {
    _painter.draw_cross(x,y);
  }
}

void ClippingPainter::_draw_pixel(double x, double y)
{
  if(_clipping_rect.contains(x,y))
  {
    _painter.draw_pixel(x,y);
  }
}

void ClippingPainter::_draw_node(double x, double y)
{
  if(_clipping_rect.contains(x,y))
  {
    _painter.draw_node(x,y);
  }
}

void ClippingPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  const Painter::Point p1(x1,y1);
  const Painter::Point p2(x2,y2);
  if(_clipping_rect.contains(x1,y1))
  {
    if(_clipping_rect.contains(x2,y2))
    {
      _painter.draw_segment(p1,p2);
    }
    else
    {
      _painter.draw_segment(p1, _exit_point(p1,p2));
    }
  }
  else
  {
    if(_clipping_rect.contains(x2,y2))
    {
      _painter.draw_segment(_enter_point(p1,p2),p2);
    }
    else
    {
      Painter::Point q1,q2;
      if(_check_transit(p1,p2,q1,q2))
      {
        _painter.draw_segment(q1,q2);
      }
    }
  }
}

void ClippingPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  if((x1!=x2) || (y1!=y2))
  {
    double ix1,iy1,ix2,iy2;
    if(_clipping_rect.intersection(x1,y1,x2,y2, ix1,iy1,ix2,iy2)==2)
    {
      _draw_segment(ix1,iy1,ix2,iy2);
    }
  }
}

void ClippingPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  if((x1!=x2) || (y1!=y2))
  {
    double ix1,iy1,ix2,iy2;
    if(_clipping_rect.intersection(x1,y1,x2,y2, ix1,iy1,ix2,iy2)==2)
    {
      const double dx0 =  x2 - x1;
      const double dy0 =  y2 - y1;
      const double dx1 = ix1 - x1;
      const double dy1 = iy1 - y1;
      const double dx2 = ix2 - x1;
      const double dy2 = iy2 - y1;

      const double sp1 = dx0*dx1 + dy0*dy1;
      const double sp2 = dx0*dx2 + dy0*dy2;

      if(_clipping_rect.contains(x1,y1))
      {
        if(sp1>sp2)
        {
          _painter.draw_arrow(x1,y1,ix1,iy1);
        }
        else
        {
          _painter.draw_arrow(x1,y1,ix2,iy2);
        }
        _painter.draw_node(x1,y1);
      }
      else
      {
        if((sp1>=0) && (sp2>=0))
        {
          if((dx1*dx1+dy1*dy1)<(dx2*dx2+dy2*dy2))
          {
            _painter.draw_arrow(ix1,iy1,ix2,iy2);
          }
          else
          {
            _painter.draw_arrow(ix2,iy2,ix1,iy1);
          }
        }
      }
    }
  }
}

void ClippingPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  std::cerr << "Faw::ClippingPainter: ellipse clipping not implemented" << std::endl;
  _painter.draw_ellipse(x,y,a,b,alpha);
}

void ClippingPainter::_draw_circle(double x, double y, double r)
{
  std::cerr << "Faw::ClippingPainter: circle clipping not implemented" << std::endl;
  _painter.draw_circle(x,y,r);
}

void ClippingPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  std::cerr << "Faw::ClippingPainter: arc clipping not implemented" << std::endl;
  _painter.draw_arc(x,y,radius,begin,end);
}

void ClippingPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  const Painter::Point p1(x1,y1);
  const Painter::Point p2(x2,y2);
  if(_clipping_rect.contains(x1,y1))
  {
    if(_clipping_rect.contains(x2,y2))
    {
      _painter.draw_arrow(p1,p2);
    }
    else
    {
      _painter.draw_segment(p1,_exit_point(p1,p2));
    }
  }
  else
  {
    if(_clipping_rect.contains(x2,y2))
    {
      _painter.draw_arrow(_enter_point(p1,p2),p2);
    }
    else
    {
      Painter::Point q1,q2;
      if(_check_transit(p1,p2,q1,q2))
      {
        _painter.draw_segment(q1,q2);
      }
    }
  }
}

void ClippingPainter::_draw_text(double x, double y, const char *msg)
{
  std::cerr << "Faw::ClippingPainter: text clipping not implemented" << std::endl;
//  _painter.draw_text(x,y,msg);
}

void ClippingPainter::_draw_text(double x, double y, const char *msg, char orientation, char h_align, char v_align)
{
  std::cerr << "Faw::ClippingPainter: text clipping not implemented" << std::endl;
//  _painter.draw_text(x,y,msg,orientation,h_align,v_align);
}

void ClippingPainter::_style_changed()
{
  _painter.set_style(this->get_style());
}

void ClippingPainter::_layer_changed()
{
  _painter.set_layer(this->get_layer());
}

void ClippingPainter::_begin()
{
  _painter.begin();
}

void ClippingPainter::_end()
{
  _painter.end();
}

Painter::Point ClippingPainter::_enter_point(const Painter::Point &p1, const Painter::Point &p2) const
{
  double ix1=0,iy1=0,ix2=0,iy2=0;
  _clipping_rect.intersection(p1[0],p1[1],p2[0],p2[1], ix1,iy1,ix2,iy2); // result is two.
  if(p2[0]<p1[0])
  {
    if(p2[0]<ix1)
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else if(p1[0]<p2[0])
  {
    if(ix1<p2[0])
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else if(p2[1]<p1[1])
  {
    if(p2[1]<iy1)
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else // if(p1[1]<p2[1])
  {
    if(iy1<p2[1])
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
}

Painter::Point ClippingPainter::_exit_point(const Painter::Point &p1, const Painter::Point &p2) const
{
  double ix1=0,iy1=0,ix2=0,iy2=0;
  _clipping_rect.intersection(p1[0],p1[1],p2[0],p2[1], ix1,iy1,ix2,iy2); // result is two.
  if(p1[0]<p2[0])
  {
    if(p1[0]<ix1)
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else if(p2[0]<p1[0])
  {
    if(ix1<p1[0])
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else if(p1[1]<p2[1])
  {
    if(p1[1]<iy1)
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
  else // if(p2[1]<p1[1])
  {
    if(iy1<p1[1])
    {
      return Painter::Point(ix1,iy1);
    }
    else
    {
      return Painter::Point(ix2,iy2);
    }
  }
}

bool ClippingPainter::_check_transit(const Painter::Point &p1, const Painter::Point &p2, Painter::Point &ip1, Painter::Point &ip2) const
{
  if((p1[0]!=p2[0]) || (p1[1]!=p2[1]))
  {
    if(_clipping_rect.intersection(p1[0],p1[1],p2[0],p2[1], ip1[0],ip1[1],ip2[0],ip2[1])==2)
    {
      const double dx1 = p1[0] - ip1[0];
      const double dy1 = p1[1] - ip1[1];
      const double dx2 = p2[0] - ip1[0];
      const double dy2 = p2[1] - ip1[1];
      return (dx1*dx2 + dy1*dy2 < 0);
    }
  }
  return false;
}
