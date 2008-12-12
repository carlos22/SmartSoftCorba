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
 
#ifndef FAW_DRAWING_POLYGON_HH
#define FAW_DRAWING_POLYGON_HH

#include <vector>

#include "fawDrawingPoint.hh"

namespace Faw {

class DrawingPolygon
{
public:
  DrawingPolygon() {}

  template<class Iterator>
  DrawingPolygon(Iterator begin, Iterator End) : _vertices(begin, end) {}
  
  template<class Vector>
  inline void append(const Vector &v) { _vertices.push_back(DrawingPoint(v)); }
  
  inline void append(double x, double y) { _vertices.push_back(DrawingPoint(x,y)); }

  inline void clear() { _vertices.clear(); }

  inline int size() const { return _vertices.size(); }

  inline const DrawingPoint &operator[](int i) const { return _vertices[i%size()]; }
  inline DrawingPoint &operator[](int i) { return _vertices[i%size()]; }

  inline std::vector<DrawingPoint>::const_iterator begin() const { return _vertices.begin(); }
  inline std::vector<DrawingPoint>::const_iterator end()   const { return _vertices.end();   }
  
  inline std::vector<DrawingPoint>::iterator begin() { return _vertices.begin(); }
  inline std::vector<DrawingPoint>::iterator end()   { return _vertices.end();   }

  void print(std::ostream &os) const;
  
private:
  std::vector<DrawingPoint> _vertices;
};

inline std::ostream &operator<<(std::ostream &os, const DrawingPolygon &p)
{
  p.print(os); return os;
}

} // namespace Faw

#endif
