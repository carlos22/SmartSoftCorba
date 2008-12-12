//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
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

#ifndef SMART_BASE_POSITION_PAINT_METHODS_HH
#define SMART_BASE_POSITION_PAINT_METHODS_HH

#include <sstream>
#include <list>

#include "fawPainter.hh"

#include "commBasePosition.hh"

namespace Faw {

template<>
struct DefaultPaintMethod<Smart::CommBasePosition>
{
  inline DefaultPaintMethod<Smart::CommBasePosition>(const double br = 300, const double tr = 250, const double unit = 0.001)
  : base_circular(true), base_radius(br * unit),
    turret_circular(true), turret_radius(tr * unit)
  {
  }

  template<class In>
  void set_base_shape(In chain_begin, In chain_end, const double unit = 0.001)
  {
    base_circular = false;
    typename std::iterator_traits<In>::value_type::const_iterator point_iter;
    In chain_iter = chain_begin;
    base_shape.clear();
    while(chain_iter!=chain_end)
    {
      base_shape.push_back(std::list<Faw::Painter::Point>());
      point_iter = (*chain_iter).begin();
      while(point_iter!=(*chain_iter).end())
      {
        base_shape.back().push_back(Faw::Painter::Point((*point_iter)[0] * unit,(*point_iter)[1] * unit));
        ++point_iter;
      }
      ++chain_iter;
    }
  }

  inline void set_base_radius(double r, const double unit = 0.001)
  {
    base_circular = true;
    base_radius = r * unit;
  }

  template<class In>
  void set_turret_shape(In chain_begin, In chain_end, const double unit = 0.001)
  {
    turret_circular = false;
    typename std::iterator_traits<In>::value_type::const_iterator point_iter;
    In chain_iter = chain_begin;
    turret_shape.clear();
    while(chain_iter!=chain_end)
    {
      turret_shape.push_back(std::list<Faw::Painter::Point>());
      point_iter = (*chain_iter).begin();
      while(point_iter!=(*chain_iter).end())
      {
        turret_shape.back().push_back(Faw::Painter::Point((*point_iter)[0] * unit,(*point_iter)[1] * unit));
        ++point_iter;
      }
      ++chain_iter;
    }
  }

  inline void set_turret_radius(double r, const double unit = 0.001)
  {
    turret_circular = true;
    turret_radius = r * unit;
  }

  bool base_circular;
  double base_radius;
  std::list<std::list<Faw::Painter::Point> > base_shape;
  
  bool turret_circular;
  double turret_radius;
  std::list<std::list<Faw::Painter::Point> > turret_shape;
  
  inline void operator()(const Smart::CommBasePosition &pos, Painter &painter, bool do_highlight) const
  {
    painter.begin();
    if(do_highlight) painter.set_pen_width(2*painter.get_pen_width());

    const double x = pos.get_x(1.0);
    const double y = pos.get_y(1.0);
    const double s = pos.get_steer_alpha();
    const double b = pos.get_base_alpha();
    const double t = pos.get_turret_alpha();

    painter.set_length_unit(1.0);

    if(base_circular)
    {
      const double cos_s = cos(s);
      const double sin_s = sin(s);
      painter.draw_circle(x, y, base_radius);
      painter.draw_arrow(x + cos_s * base_radius, y + sin_s * base_radius, x + cos_s * 3 * base_radius/2, y + sin_s * 3 * base_radius/2);
    }
    else
    {
      painter.begin();
      painter.set_transformation(x,y,b,1.0);
      std::list<std::list<Faw::Painter::Point> >::const_iterator chain_iter;
      chain_iter = base_shape.begin();
      while(chain_iter!=base_shape.end())
      {
        painter.draw_chain((*chain_iter).begin(), (*chain_iter).end());
        ++chain_iter;
      }
      painter.end();
    }

    if(turret_circular)
    {
      const double cos_t = cos(t);
      const double sin_t = sin(t);
      painter.draw_circle(x, y, turret_radius);
      painter.draw_segment(x, y, x + cos_t * turret_radius/2 , y + sin_t * turret_radius/2);
    }
    else
    {
      painter.begin();
      painter.set_transformation(x,y,t,1.0);
      std::list<std::list<Faw::Painter::Point> >::const_iterator chain_iter;
      chain_iter = turret_shape.begin();
      while(chain_iter!=turret_shape.end())
      {
        painter.draw_chain((*chain_iter).begin(), (*chain_iter).end());
        ++chain_iter;
      }
      painter.end();
    }

    painter.end();
  }
  inline void operator()(const Smart::CommBasePosition &pos, Painter &painter) const
  {
    this->operator()(pos,painter,false);
  }
};

} // namespace Faw

#endif
