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
 
#ifndef FAW_DRAWING_POINT_HH
#define FAW_DRAWING_POINT_HH

#include <iostream>

namespace Faw {

class DrawingPoint
{
public:
  DrawingPoint() : _x(0), _y(0) {}

  DrawingPoint(double x, double y) : _x(x), _y(y) {}

  template<class Vector>
  DrawingPoint(const Vector &v) : _x(v[0]), _y(v[1]) {}

  inline const double &operator[](int i) const { return ((double*)this)[i%2]; }
  inline double &operator[](int i) { return ((double*)this)[i%2]; }

  template<class Vector>
  inline DrawingPoint &operator=(const Vector &v) { _x=v[0]; _y=v[1]; return *this; }
  
  inline bool operator==(const DrawingPoint &o) const { return (_x==o._x) && (_y==o._y); }

  inline double sqr_dist(const DrawingPoint &o) const
  {
    const double dx = _x - o._x;
    const double dy = _y - o._y; 
    return dx*dx + dy*dy;
  }

  void print(std::ostream &os) const { os << "(" << _x << "," << _y << ")"; }

private:
  double _x, _y;
};

inline std::ostream &operator<<(std::ostream &os, const DrawingPoint &p)
{
  p.print(os); return os;
}

} // namespace Faw

#endif

