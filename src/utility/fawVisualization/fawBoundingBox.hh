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
 
#ifndef FAW_BOUNDING_BOX_H
#define FAW_BOUNDING_BOX_H

#include <iostream>

namespace Faw {

class BoundingBox
{
public:
  /**
    Create an empty box.
   */
  BoundingBox() : _empty(true) {}

  /**
    Create a box containing only one point.
   */
  BoundingBox(double x, double y) { assign(x,y); }
  
  /**
    Create the minimal box containing the given two points.
   */
  BoundingBox(double x1, double y1, double x2, double y2) { assign(x1,y1); add(x2,y2); }

  /**
    Make this box the empty box.
   */
  BoundingBox &clear() { _empty = true; return *this; }

  /**
    Make this box containing only one point.
   */
  BoundingBox &assign(double x, double y) 
  { 
    clear();
    add(x,y);
    return *this;
  }
  
  /**
    Create the minimal box containing the given two points.
   */
  BoundingBox &assign(double x1, double y1, double x2, double y2)
  {
    assign(x1,y1);
    add(x2,y2);
    return *this;
  }

  /**
    Make this box the bounding box of the given point set.
   */
  template<class Iterator>
  BoundingBox &assign_all(Iterator begin, Iterator end)
  {
    clear();
    add(begin, end);
    return *this;
  }

  /**
    Enlarge this box to contain the given point.
   */
  BoundingBox &add(double x, double y) 
  { 
    if(_empty)
    {
      _empty = false;
      _l = _r = x;
      _t = _b = y;
    }
    else
    {
      _l = _min(_l, x);
      _b = _min(_b, y);
      _r = _max(_r, x);
      _t = _max(_t, y);
    }
    return *this;
  }
  
  /**
    Enlarge this box to contains the box \a bbox.
   */
  BoundingBox &add(const BoundingBox &bbox)
  {
    if(!bbox.empty()) 
    {
      add(bbox._l,bbox._b);
      add(bbox._r,bbox._t);
    }
    return *this;
  }
  
  /**
    Enlarge this box to contain the given point set.
   */
  template<class Iterator>
  BoundingBox &add_all(Iterator begin, Iterator end)
  {
    while(begin!=end)
    {
      add((*begin)[0], (*begin)[1]);
      ++begin;
    }
    return *this;
  }
  
  /**
    Reduce this box to the intersection with the given box \a b.
   */
  BoundingBox &intersection(const BoundingBox &b)
  {
    if(_empty || b._empty)
    {
      _empty = true;
    }
    else
    {
      _l = _max(_l, b._l);
      _r = _min(_r, b._r);
      _t = _min(_t, b._t);
      _b = _max(_b, b._b);
      _check_empty();
    }
    return *this;
  }

  /**
    Intersect the line passing through (x1,y1) and (x2,y2) with the bounding box.
    Proper intersection points (there are no more than two) are reported via the
    reference parameters \p ix1, \p iy1, \p ix2, \p iy2, the number of proper intersections
    (zero, one or two) is returned by the function.
   */
  inline int intersection(double x1, double y1, double x2, double y2, double &ix1, double &iy1, double &ix2, double &iy2) const;

  /**
    Enlarge this box in each direction by \c b, unless it is empty.
   */
  BoundingBox &grow(double b)
  {
    if(!_empty)
    {
      _l -= b;
      _r += b;
      _b -= b;
      _t += b;
      _check_empty(); // in case b<0
    }
    return *this;
  }

  /**
    Test if empty.
   */
  bool empty() const { return _empty; }

  /**
    Test for intersection with another box.
   */
  bool intersects(const BoundingBox &b) const
  {
    return (!_empty) && (!b._empty) && (_max(_l, b._l)<=_min(_r, b._r)) && (_max(_b, b._b)<=_min(_t, b._t));
  }

  double top() const { return _t; }
  double bottom() const { return _b; }
  double left() const { return _l; }
  double right() const { return _r; }

  double width() const { return _r - _l; }
  double height() const { return _t - _b; }

  /**
    Test if the given point lies inside the box.
   */
  bool contains(double x, double y) const 
  { 
    return (!_empty) && (_l<=x) && (x<=_r) && (_b<=y) && (y<=_t);
  }

  /**
    Test if the given box \c b lies completely inside this box.
   */
  bool contains(const BoundingBox &b) const
  {
    return contains(b._l,b._b) && contains(b._r, b._t);
  }

  /**
    Test if all the given points are inside this box.
   */
  template<class Iterator>
  bool contains_all(Iterator begin, Iterator end) const
  {
    bool ok = true;
    while(ok && (begin!=end))
    {
      ok = contains((*begin)[0], (*begin)[1]);
      ++begin;
    }
    return ok;
  }

  void print(std::ostream &os) const
  {
    os << "BoundingBox(";
    if(!_empty)
    {
      os << "left=" << _l << " right=" << _r << " top=" << _t << " bottom=" << _b;
    }
    else
    {
      os << "empty";
    }
    os << ")";
  }

private:
  bool _empty;

  /**
    Left, right, top and bottom.
   */
  double _l, _r, _t, _b;

  inline void _check_empty() { _empty = _empty || (_l>_r) || (_b>_t); }
  inline static double _min(double a, double b) { return(a<b)?a:b; }
  inline static double _max(double a, double b) { return(a<b)?b:a; }

  /**
    Computes the orientation of points (x1,y1), (x2,y2), and (x3,y3).
    Right turns return -1, collinear points return 0, and left turns return 1.
   */
  static inline int _orientation(double x1, double y1, double x2, double y2, double x3, double y3);

  /**
    Computes the intersection of a line given by (x1,y1) and (x2,y2) with the x axis.
   */
  static inline double _null_position_x(double x1, double y1, double x2, double y2);

  /**
    Computes the intersection of a line given by (x1,y1) and (x2,y2) with the y axis.
   */
  static inline double _null_position_y(double x1, double y1, double x2, double y2);
};

inline std::ostream &operator<<(std::ostream &os, const BoundingBox &box)
{
  box.print(os); return os;
}

/**
  Combine two bounding boxes.
 */
inline BoundingBox operator+(BoundingBox bb1, const BoundingBox &bb2)
{
  return (bb1.add(bb2));
}

/**
  Intersect two bounding boxes.
 */
inline BoundingBox operator*(BoundingBox bb1, const BoundingBox &bb2)
{
  return (bb1.intersection(bb2));
}

inline int BoundingBox::intersection
    (
        double x1, double y1,
        double x2, double y2,
        double &ix1, double &iy1,
        double &ix2, double &iy2
    ) const
{
  double ix[2];
  double iy[2];
  int count = 0;

  // top border
  if(_orientation(x1,y1,x2,y2, _l, _t) * _orientation(x1,y1,x2,y2, _r, _t) < 0)
  {
    const double y = _t;
    iy[count] = y;
    ix[count] = _null_position_x(x1,y1-y,x2,y2-y);
    ++count;
  }
  // bottom border
  if(_orientation(x1,y1,x2,y2, _l, _b) * _orientation(x1,y1,x2,y2, _r, _b) < 0)
  {
    const double y = _b;
    iy[count] = y;
    ix[count] = _null_position_x(x1,y1-y,x2,y2-y);
    ++count;
  }
  if(count<2)
  {
    // left border
    if(_orientation(x1,y1,x2,y2, _l, _b) * _orientation(x1,y1,x2,y2, _l, _t) < 0)
    {
      const double x = _l;
      ix[count] = x;
      iy[count] = _null_position_y(x1-x,y1,x2-x,y2);
      ++count;
    }
  }
  if(count<2)
  {
    // right border
    if(_orientation(x1,y1,x2,y2, _r, _b) * _orientation(x1,y1,x2,y2, _r, _t) < 0)
    {
      const double x = _r;
      ix[count] = x;
      iy[count] = _null_position_y(x1-x,y1,x2-x,y2);
      ++count;
    }
  }
  if(count>0)
  {
    ix1 = ix[0];
    iy1 = iy[0];
    if(count>1)
    {
      ix2 = ix[1];
      iy2 = iy[1];
    }
  }
  return count;
}



inline int BoundingBox::_orientation(double x1, double y1, double x2, double y2, double x3, double y3)
{
  const double d1 = (x1 - x2) * (y1 - y3);
  const double d2 = (y1 - y2) * (x1 - x3);
  if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
}

double BoundingBox::_null_position_x(double x1, double y1, double x2, double y2)
{
  if(y1!=y2)
  {
    return x1 - y1*(x2-x1)/(y2-y1);
  }
  else return 0;
}


double BoundingBox::_null_position_y(double x1, double y1, double x2, double y2)
{
  return _null_position_x(y1,x1,y2,x2);
}

} // namespace Faw

#endif
