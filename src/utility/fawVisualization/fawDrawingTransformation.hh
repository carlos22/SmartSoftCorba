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
 
#ifndef FAW_DRAWING_TRANSFORMATION_HH
#define FAW_DRAWING_TRANSFORMATION_HH

#include <math.h>
#include <iostream>

namespace Faw {

class DrawingTransformation
{
public:
  DrawingTransformation(double dx = 0, double dy = 0, double alpha = 0, const double unit = 1.0)
    : _dx(dx*unit), _dy(dy*unit) { set_alpha(alpha); }

  inline double get_dx(const double unit = 1.0) const { return _dx/unit; }
  inline double get_dy(const double unit = 1.0) const { return _dy/unit; }
  inline double get_alpha() const { return _alpha; }

  inline void set(double dx, double dy, double alpha, const double unit = 1.0) { _dx = dx*unit; _dy = dy*unit; set_alpha(alpha); }
  inline void set_dx_dy(double dx, double dy, const double unit = 1.0) { _dx = dx*unit; _dy = dy*unit; }
  inline void set_dx(double dx, const double unit = 1.0) { _dx = dx*unit; }
  inline void set_dy(double dy, const double unit = 1.0) { _dy = dy*unit; }
  inline void set_alpha(double alpha) { _alpha = alpha; _cos_alpha = cos(alpha); _sin_alpha = sin(alpha); }

  inline double xform_x(double x, double y) { return x*_cos_alpha - y*_sin_alpha + _dx; }
  inline double xform_y(double x, double y) { return x*_sin_alpha + y*_cos_alpha + _dy; }
  
  inline DrawingTransformation &operator*=(const DrawingTransformation &other)
  {
    const double tmp_dx = xform_x(other._dx, other._dy);
                    _dy = xform_y(other._dx, other._dy);
                    _dx = tmp_dx;
    set_alpha(_alpha + other._alpha);
    return *this;
  }

  inline void print(std::ostream &os = std::cout) const
  {
    os << "(dx=" << _dx << ", dy=" << _dy << ", alpha=" << _alpha << ")";
  }

private:
  double _dx;
  double _dy;
  double _alpha;
  double _cos_alpha;
  double _sin_alpha;
};

inline DrawingTransformation operator*(DrawingTransformation t1, const DrawingTransformation &t2)
{
  return t1*=t2;
}

inline std::ostream &operator<<(std::ostream &os, const DrawingTransformation &dt)
{
  dt.print(os); return os;
}

} // namespace Faw

#endif
