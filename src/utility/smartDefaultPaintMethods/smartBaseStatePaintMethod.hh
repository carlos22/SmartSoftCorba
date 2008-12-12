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

#ifndef SMART_BASE_STATE_PAINT_METHODS_HH
#define SMART_BASE_STATE_PAINT_METHODS_HH

#include <sstream>

#include "fawPainter.hh"
#include "commBaseState.hh"
#include "smartBasePositionPaintMethod.hh"

namespace Faw {

template<>
struct DefaultPaintMethod<Smart::CommBaseState>
{
  DefaultPaintMethod<Smart::CommBasePosition> position_paint_method;

  inline DefaultPaintMethod<Smart::CommBaseState>(const double br = 300, const double tr = 250, const double unit = 0.001)
  : position_paint_method(br,tr,unit)
  {
  }

  template<class In>
  void set_base_shape(In chain_begin, In chain_end, const double unit = 0.001)
  {
    position_paint_method.set_base_shape(chain_begin,chain_end,unit);
  }

  inline void set_base_radius(double r, const double unit = 0.001)
  {
    position_paint_method.set_base_radius(r,unit);
  }

  template<class In>
  void set_turret_shape(In chain_begin, In chain_end, const double unit = 0.001)
  {
    position_paint_method.set_turret_shape(chain_begin,chain_end,unit);
  }

  inline void set_turret_radius(double r, const double unit = 0.001)
  {
    position_paint_method.set_turret_radius(r,unit);
  }

  inline void operator()(const Smart::CommBaseState &s, Painter &painter, bool do_highlight) const
  {
    position_paint_method(s.get_base_position(),painter,do_highlight);
  }

  inline void operator()(const Smart::CommBaseState &s, Painter &painter) const
  {
    this->operator()(s,painter,false);
  }
};

} // namespace Faw

#endif
