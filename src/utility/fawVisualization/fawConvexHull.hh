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
 
#ifndef FAW_CONVEX_HULL_H
#define FAW_CONVEX_HULL_H

#include <list>
#include <util>

namespace Faw {

class ConvexHull
{
public:


  /**
    Create an empty hull.
   */
  ConvexHull() : _slope_less_than(), _vertices(_slope_less_than) {}

  /**
    Create a hull containing only one point.
   */
  ConvexHull(double x, double y) { assign(x,y); }
  
  /**
    Create the minimal hull containing the given two points.
   */
  ConvexHull(double x1, double y1, double x2, double y2) { assign(x1,y1,x2,y2); }

  /**
    Make this hull the empty hull.
   */
  ConvexHull &clear() { _vertices.clear(); return *this; }

  /**
    Make this hull containing only one point.
   */
  ConvexHull &assign(double x, double y) 
  { 
    clear();
    add(x,y);
    return *this;
  }
  
  /**
    Create the minimal hull containing the given two points.
   */
  ConvexHull &assign(double x1, double y1, double x2, double y2)
  {
    assign(x1,y1);
    add(x2,y2);
    return *this;
  }

  /**
    Make this hull the convex hull of the given point set.
   */
  template<class Iterator>
  ConvexHull &assign_all(Iterator begin, Iterator end)
  {
    clear();
    add(begin, end);
    return *this;
  }

  /**
    Enlarge this hull to contain the given point.
   */
  ConvexHull &add(double x, double y) 
  { 
    if(_vertices.empty())
    {
      _slope_less_than.set_ref_point(x,y);
      _vertices.insert(make_pair(x,y));
    }
    else if(_vertices.size()==1)
    {
      skljfsldkfj
    }
    else
    {
      const double ref_x = _slope_less_than.get_ref_point_x();
      const double ref_y = _slope_less_than.get_ref_point_y();
      set<pair<double,double>,SlopeLessThan>::iterator iter;
      if(ref_x<x)
      {
        iter = _vertices.find(make_pair(x,y));
        if(iter==_vertices.end())
        {
          iter = _vertices.insert(make_pair(x,y));
        }
        else
        {
          if(_slope_less_than.less_distance(*iter, make_pair(x,y)))
          {
            // "push" *iter outside, away from ref_point
            *iter = make_pair(x,y);
          }
          else
          {
            // (x,y) already lies inside the hull
            return *this;
          }
        }
        _graham_scan_backward(iter);
        _graham_scan_forward(iter);
      }
      else if(ref_x==x)
      {
        if(y<ref_y)
        {
          iter = _vertices.begin(); // the ref point
          (*iter).second = y;
          _slope_less_than.set_ref_point(x,y);
          _graham_scan_forward(iter);
        }
        else
        {
          iter = _vertices.end(); --iter;
          if((*iter).first==x)
          {
            if((*iter).second<y)
            {
              (*iter).second = y;
              _graham_scan_backward(iter);
            }
          }
          else
          {
            iter = _vertices.insert(make_pair(x,y));
            _graham_scan_back(iter);
          }
        }
      }
      else // x < ref_x
      {
        if(y<ref_y)
        {
          iter = _vertices.begin();

          iter = _vertices.begin();
          *iter = make_pair(x,y);
          _graham_scan_forward(iter);
          while(true)
          {
            
            break;
          }
          _slope_less_than.set_ref_point(x,y);
          iter2 = _vertices.insert(make_pair(ref_x,ref_y));
          _graham_scan_backward(iter2);
          _graham_scan_forward(iter2);
        }
        else
        {
          
        }
      }
    }
    return *this;
  }
  
  /**
    Enlarge this hull to contains the hull \a chull.
   */
  ConvexHull &add(const ConvexHull &chull)
  {
    if(!chull.empty()) 
    {
      // combine the two hulls of sizes n and m in time O(m+n)
      // ...
    }
    return *this;
  }
  
  /**
    Enlarge this hull to contain the given point set.
   */
  template<class Iterator>
  ConvexHull &add_all(Iterator begin, Iterator end)
  {
    while(begin!=end)
    {
      add((*begin)[0], (*begin)[1]);
      ++begin;
    }
    return *this;
  }
  
  /**
    Reduce this hull to the intersection with the given hull \a b.
   */
  ConvexHull &intersection(const ConvexHull &b)
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
    Intersect the line passing through (x1,y1) and (x2,y2) with the convex hull.
    Proper intersection points (there are no more than two) are reported via the
    reference parameters \p ix1, \p iy1, \p ix2, \p iy2, the number of proper intersections
    (zero, one or two) is returned by the function.
   */
  inline int intersection(double x1, double y1, double x2, double y2, double &ix1, double &iy1, double &ix2, double &iy2) const;

  /**
    Test if empty.
   */
  bool empty() const { return _empty; }

  /**
    Test for intersection with another hull.
   */
  bool intersects(const ConvexHull &b) const
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
    Test if the given point lies inside the hull.
   */
  bool contains(double x, double y) const 
  { 
    if(_vertices.size()<3)
    {
      if(_vertices.size()==2)
      {
        return _orientation(_vertices.front(), make_pair(x,y), _vertices.back())==0;
      }
      else if(_vertices.size()==1)
      {
        return _vertices.front()==make_pair(x,y);
      }
      else
      {
        return false;
      }
    }
    else if(x<_slope_less_than.get_ref_point_x())
    {
      return false;
    }
    else if(x>_slope_less_than.get_ref_point_x())
    {
      set<pair<double,double>,SlopeLessThan>::const_iterator lower_bound = _vertices.lower_bound(make_pair(x,y));
      set<pair<double,double>,SlopeLessThan>::const_iterator upper_bound = _vertices.upper_bound(make_pair(x,y));
      // lower_bound != _vertices.begin() since x > _slope_less_than.get_ref_point_x()
      if(lower_bound==upper_bound)
      {
        --lower_bound;
        if(upper_bound!=_vertices.end())
        {
          return _orientation(*lower_bound, make_pair(x,y), *upper_bound)<=0;
        }
        else
        {
          return _orientation(*lower_bound, make_pair(x,y), _vertices.front())<=0;
        }
      }
      else
      {
        return !_slope_less_than.less_distance(*lower_bound, make_pair(x,y));
      }
    }
    else // x == _slope_less_than.get_ref_point_x()
    {
      if(y<_slope_less_than.get_ref_point_y())
      {
        return false;
      }
      else
      {
        return (_vertices.back().first==_slope_less_than.get_ref_point_x()) && (y<=_vertices.back().second);
      }
    }
  }

  /**
    Test if the given hull \c b lies completely inside this hull.
   */
  bool contains(const ConvexHull &b) const
  {
    return contains(b._l,b._b) && contains(b._r, b._t);
  }

  /**
    Test if all the given points are inside this hull.
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

  class point
  {
  public:
    point(double xx, double yy) : x(xx), y(yy) {}
    inline int operator==(const point &p) const { return (p.x==x) && (p.y==y); }
    const double &operator[](int i) const { return ((double*)this)[i%2]; }
    double &operator[](int i) { return ((double*)this)[i%2]; }
    double x,y;
  };

private:

  class SlopeLessThan
  {
  public:
    inline void set_ref_point(double x, double y) { _ref_x = x; _ref_y = y; }
    inline const double &get_ref_point_x() const { return _ref_x; }
    inline const double &get_ref_point_y() const { return _ref_y; }
    inline bool operator()(const pair<double,double> &p1, const pair<double,double> &p2) const
    {
      const double dx1 = p1.first  - _ref_x;
      const double dy1 = p1.second - _ref_y;
      const double dx2 = p2.first  - _ref_x;
      const double dy2 = p2.second - _ref_y;
      if(dx1>0)
      {
        if(dx2>0)
        {
          return (dy1*dx2)<(dy2*dx1);
        }
        else // dx2==0
        {
          return dy2>0;
        }
      }
      else // dx1==0
      {
        if(dx2>0)
        {
          return dy1<=0;
        }
        else // dx2==0
        {
          return ((dy2!=0) && (dy1==0)) || ((dy1<0) && (dy2>0)));
        }
      }
    }
    inline bool less_distance(const pair<double,double> &p1, const pair<double,double> &p2) const
    {
      const double dx1 = p1.first  - _ref_x;
      const double dy1 = p1.second - _rex_y;
      const double dx2 = p2.first  - _ref_x;
      const double dy2 = p2.second - _rex_y;
      return (dx1*dx1 + dy1*dy1) < (dx2*dx2 + dy2*dy2);
    }
  private:
    double _ref_x, _ref_y;
  }

  /**
    Comparator for the angularly ordered vertex set.
   */
  SlopeLessThan _slope_less_than;

  /**
    Angularly ordered set of the vertices of this convex hull.
   */
  set<pair<double,double>,SlopeLessThan> _vertices;
  
  double _sum_x;
  double _sum_y;

  inline static double _min(double a, double b) { return(a<b)?a:b; }
  inline static double _max(double a, double b) { return(a<b)?b:a; }

  /**
    Computes the orientation of points (x1,y1), (x2,y2), and (x3,y3).
    Right turns return -1, collinear points return 0, and left turns return 1.
   */
  static inline int _orientation(double x1, double y1, double x2, double y2, double x3, double y3);

  static inline int _orientation(const pair<double,double> &p1, const pair<double,double> &p2, const pair<double,double> &p3)
  {
    return _orientation(p1.first,p1.second, p2.first,p2.second, p3.first,p3.second);
  }

  inline void _graham_scan_forward(set<pair<double,double>,SlopeLessThan>::iterator iter);
  inline void _graham_scan_backward(set<pair<double,double>,SlopeLessThan>::iterator iter);

};

/**
  Combine two convex hulls.
 */
inline ConvexHull operator+(ConvexHull bb1, const ConvexHull &bb2)
{
  return (bb1.add(bb2));
}

/**
  Intersect two convex hulls.
 */
inline ConvexHull operator*(ConvexHull bb1, const ConvexHull &bb2)
{
  return (bb1.intersection(bb2));
}

inline int ConvexHull::intersection
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



inline int ConvexHull::_orientation(double x1, double y1, double x2, double y2, double x3, double y3)
{
  const double d1 = (x1 - x2) * (y1 - y3);
  const double d2 = (y1 - y2) * (x1 - x3);
  if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
}

double ConvexHull::_null_position_x(double x1, double y1, double x2, double y2)
{
  if(y1!=y2)
  {
    return x1 - y1*(x2-x1)/(y2-y1);
  }
  else return 0;
}


double ConvexHull::_null_position_y(double x1, double y1, double x2, double y2)
{
  return _null_position_x(y1,x1,y2,x2);
}

inline void ConvexHull::_graham_scan_forward(set<pair<double,double>,SlopeLessThan>::iterator iter1)
{
  set<pair<double,double>,SlopeLessThan>::iterator iter2;
  set<pair<double,double>,SlopeLessThan>::iterator iter3;

  // "graham scan" forth over the vertex list
  iter2 = iter1; ++iter2; iter3 = iter2;
  if(iter3!=_vertices.end()) ++iter3;
  while(iter3!=_vertices.end())
  {
    if(_orientation(*iter1,*iter2,*iter3)>0) // left turn
    {
      break; // done
    }
    else
    {
      _vertices.erase(iter2);
      iter2 = iter3;
      ++iter3;
    }
  }
}

inline void ConvexHull::_graham_scan_backward(set<pair<double,double>,SlopeLessThan>::iterator iter1);
{
  set<pair<double,double>,SlopeLessThan>::iterator iter2;
  set<pair<double,double>,SlopeLessThan>::iterator iter3;

  // "graham scan" back over the vertex list
  iter2 = iter1; if(iter1!=_vertices.begin()) --iter2;
  while(iter2!=_vertices.begin())
  {
    iter3 = iter2; --iter3;
    if(_orientation(*iter1,*iter2,*iter3)<0) // right turn
    {
      break; // done.
    }
    else
    {
      _vertices.erase(iter2); // remove middle point
      iter2 = iter3;
    }
  }
}

} // namespace Faw

#endif
