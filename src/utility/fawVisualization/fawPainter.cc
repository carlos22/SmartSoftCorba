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
 
#include <fawPainter.hh>

using namespace Faw;

Painter::Painter()
  : _unit(1.0),
    _layer(0),
    _style_propagated(false)
{
}



void Painter::begin()
{
  _painter_state_stack.push_back(PainterState(_unit,_layer,_style,_base_xform));
  _base_xform = _accu_xform;
  _begin();
}



void Painter::end()
{
  if(!_painter_state_stack.empty())
  {
    _end();
    PainterState painter_state = _painter_state_stack.back();
    _painter_state_stack.pop_back();
    _unit = painter_state.unit;
    _layer = painter_state.layer;
    _style = painter_state.style;
    _accu_xform = _base_xform;
    _base_xform = painter_state.base_xform;
    _style_propagated = false;
  }
  else
  {
    std::cerr << "Faw::Painter::end() : trying to pop element from empty stack." << std::endl;
  }
}



void Painter::_draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(num_points>1)
  {
    for(unsigned int i=0; i<num_points; i++)
    {
      if(i>0)
      {
        const unsigned int j = i - 1;
        this->_draw_segment(points[j][0], points[j][1], points[i][0], points[i][1]);
      }
      else
      {
        const unsigned int j = num_points - 1;
        this->_draw_segment(points[j][0], points[j][1], points[i][0], points[i][1]);
      }
    }
  }
}

void Painter::_draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  if(num_points>1)
  {
    const unsigned int num_segments = num_points - 1;
    for(unsigned int i=0; i<num_segments; i++)
    {
      const unsigned int j = i + 1;
      this->_draw_segment(points[i][0], points[i][1], points[j][0], points[j][1]);
    }
  }
}

void Painter::_draw_line(double x1, double y1, double x2, double y2)
{ 
  this->_draw_segment(x1,y1,x2,y2); 
}

void Painter::_draw_ray(double x1, double y1, double x2, double y2)
{ 
  this->_draw_arrow(x1,y1,x2,y2); 
}

void Painter::_draw_circle(double x, double y, double r)
{ 
  this->_draw_ellipse(x,y,r,r,0); 
}

std::vector<Painter::Point> Painter::_point_array(1000);

