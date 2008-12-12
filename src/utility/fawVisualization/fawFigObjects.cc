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
 
#include <iomanip>

#include "fawFigObjects.hh"

using namespace Faw;

//
// Polygon
//

void FigPolygon::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  const unsigned int n = _points.size();
  if(n==0) return;
  os << 
    XFIG_O_POLYLINE << " " << 
    T_POLYGON       << " " <<
    _style          << " " << 
    _line_width     << " " <<
    _pen_color      << " " << // pen color
    _fill_color     << " " << // fill color
    _depth          << " " << // depth
    DEFAULT         << " " << // pen (unused)
    _area_fill      << " " << // area fill
    0.0             << " " << // style_val
    JOIN_MITER      << " " << // join style
    CAP_BUTT        << " " << // cap style
    DEFAULT         << " " << // for arc-box only
    0               << " " << 
    0               << " " << // no arrows
    n + 1           << std::endl;  // num points

  os << "\t";
  for (unsigned int i=0; i<n; i++)
  {
    os << _points[i].first << " " <<  _points[i].second << " ";
  }
  os << _points[0].first << " " <<  _points[0].second << std::endl;
}

BoundingBox FigPolygon::bounding_box() const
{
  BoundingBox box;
  for(unsigned int i=0; i<_points.size(); i++) box.add(_points[i].first,_points[i].second);
  return box;
}

void FigPolygon::translate(int dx, int dy)
{
  for(unsigned int i=0; i<_points.size(); i++)
  {
    _points[i].first  = _points[i].first  + dx;
    _points[i].second = _points[i].second + dy;
  }
}

//
// Chain
//

void FigChain::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  const unsigned int n = _points.size();
  if(n==0) return;
  os <<
    XFIG_O_POLYLINE << " " << 
    T_POLYLINE      << " " <<
    _style          << " " << 
    _line_width     << " " <<
    _pen_color      << " " << // pen color
    DEFAULT         << " " << // fill color
    _depth          << " " << // depth
    DEFAULT         << " " << // pen (unused)
    UNFILLED        << " " << // area fill
    0.0             << " " << // style_val
    JOIN_MITER      << " " << // join style
    CAP_BUTT        << " " << // cap style
    DEFAULT         << " " << // for arc-box only
    0               << " " << 
    0               << " " << // no arrows
    n               << std::endl;  // num points

  os << "\t";
  for (unsigned int i=0; i<n; i++)
  {
    if(i>0) os << " ";
    os << _points[i].first << " " <<  _points[i].second;
  }
  os << std::endl;
}

BoundingBox FigChain::bounding_box() const
{
  BoundingBox box;
  for(unsigned int i=0; i<_points.size(); i++) box.add(_points[i].first,_points[i].second);
  return box;
}

void FigChain::translate(int dx, int dy)
{
  for(unsigned int i=0; i<_points.size(); i++)
  {
    _points[i].first  = _points[i].first  + dx;
    _points[i].second = _points[i].second + dy;
  }
}

//
// Segment
//

void FigSegment::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  int x1 = _x1, y1 = _y1, x2 = _x2, y2 = _y2;
  bool do_draw = false;
  if(_boundary_type!=SEGMENT)
  {
    if((_x1!=_x2) || (_y1!=_y2))
    {
      BoundingBox box(left,bottom,right,top);
      double ix1,iy1,ix2,iy2;
      if(box.intersection(_x1,_y1,_x2,_y2, ix1,iy1,ix2,iy2)==2)
      { 
        if(_boundary_type==LINE)
        {
          x1 = (int)rint(ix1);
          y1 = (int)rint(iy1);
          x2 = (int)rint(ix2);
          y2 = (int)rint(iy2);
          do_draw = true;
        }
        else
        {
          const double dx0 = _x2 - _x1;
          const double dy0 = _y2 - _y1;
          const double dx1 = ix1 - _x1;
          const double dy1 = iy1 - _y1;
          const double dx2 = ix2 - _x1;
          const double dy2 = iy2 - _y1;

          const double sp1 = dx0*dx1 + dy0*dy1;
          const double sp2 = dx0*dx2 + dy0*dy2;

          if(box.contains(_x1,_y1))
          {
            if(sp1>sp2)
            {
              x2 = (int)rint(ix1);
              y2 = (int)rint(iy1);
              do_draw = true;
            }
            else
            {
              x2 = (int)rint(ix2);
              y2 = (int)rint(iy2);
              do_draw = true;
            }
          }
          else
          {
            if((sp1>=0) && (sp2>=0))
            {
              x1 = (int)rint(ix1);
              y1 = (int)rint(iy1);
              x2 = (int)rint(ix2);
              y2 = (int)rint(iy2);
              do_draw = true;
            }
          }
        }
      }
    }
  }
  else
  {
    do_draw = true;
  }

  if(do_draw)
  {
    os << 
      XFIG_O_POLYLINE << " " << 
      T_POLYLINE      << " " <<
      _style          << " " << 
      _line_width     << " " <<
      _pen_color      << " " << // pen color
      DEFAULT         << " " << // fill color
      _depth          << " " << // depth
      DEFAULT         << " " << // pen (unused)
      UNFILLED        << " " << // area fill
      0.0             << " " << // style_val
      JOIN_MITER      << " " << // join style
      CAP_BUTT        << " " << // cap style
      DEFAULT         << " " << // for arc-box only
      0               << " " << 
      0               << " " << // no arrows
      2               << std::endl;  // num points

    os << "\t" << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
  }
}

BoundingBox FigSegment::bounding_box() const
{
  return BoundingBox(_x1,_y1,_x2,_y2);
}

void FigSegment::translate(int dx, int dy)
{
  _x1 += dx; _y1 += dy; _x2 += dx; _y2 += dy;
}

//
// Ellipse
//

void FigEllipse::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  os << 
    XFIG_O_ELLIPSE   << " " << 
    T_ELLIPSE_BY_RAD << " " <<
    _style           << " " << 
    _line_width      << " " <<
    _pen_color       << " " << // pen color
    _fill_color      << " " << // fill color
    _depth           << " " << // depth
    DEFAULT          << " " << // pen (unused)
    _area_fill       << " " << // area fill
    0.0              << " " << // style_val
    1                << " " << // direction
    _alpha           << " " << // angle of x-axis
    _cx              << " " << // center
    _cy              << " " <<
    _a               << " " << // radius
    _b               << " " <<
    _cx              << " " << // 1st point
    _cy              << " " <<
    _cx + _a         << " " << // 2nd point
    _cy + _b         << std::endl;
}

BoundingBox FigEllipse::bounding_box() const
{
  // compute an approximation of the bounding box:
  // the bounding of the rotated bounding box of the unrotated ellipse
  const double sin_alpha = sin(_alpha);
  const double cos_alpha = cos(_alpha);
  const double h = _a*sin_alpha + _b*cos_alpha;
  const double w = _a*cos_alpha - _b*sin_alpha;
  return BoundingBox(_cx-w,_cy-h, _cx+w,_cy+h);
}

void FigEllipse::translate(int dx, int dy)
{
  _cx += dx; _cy += dy;
}

//
// Arc
//

FigArc::FigArc
  (
    int cx, int cy, int r, double begin, double end,
    int style,
    int line_width,
    int pen_color,
    int area_fill,
    int fill_color,
    int depth
  ) : 
    _cx(cx), _cy(cy), _r(r),
    _begin(begin), _end(end),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _area_fill(area_fill),
    _fill_color(fill_color),
    _depth(depth) 
{
  _begin -= 2*M_PI*floor(_begin/(2*M_PI));
  _end -= 2*M_PI*floor(_end/(2*M_PI));
  if(_end<_begin) _end+=2*M_PI;
}

void FigArc::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  const double middle = (_begin+_end)/2;
  os << 
    XFIG_O_ARC   << " " << 
    T_OPEN_ARC << " " <<
    _style           << " " << 
    _line_width      << " " <<
    _pen_color       << " " << // pen color
    _fill_color      << " " << // fill color
    _depth           << " " << // depth
    DEFAULT          << " " << // pen (unused)
    _area_fill       << " " << // area fill
    0.0              << " " << // style_val
    CAP_BUTT                     << " " << // cap style
    1                            << " " << // direction
    0                            << " " << // forward arrow
    0                            << " " << // backward arrow
    _cx                    << " " << // center
    _cy                    << " " <<
    (int)rint(_cx+_r*cos(-_begin))   << " " <<
    (int)rint(_cy+_r*sin(-_begin))   << " " << 
    (int)rint(_cx+_r*cos(-middle)) << " " <<
    (int)rint(_cy+_r*sin(-middle)) << " " <<
    (int)rint(_cx+_r*cos(-_end)) << " " <<
    (int)rint(_cy+_r*sin(-_end)) << std::endl;
}

BoundingBox FigArc::bounding_box() const
{
  BoundingBox box(_cx,_cy);
  if((_begin<=2*M_PI)   && (_end>=2*M_PI))   box.add(_cx + _r, _cy);
  if((_begin<=M_PI/2)   && (_end>=M_PI/2))   box.add(_cx, _cy + _r);
  if((_begin<=M_PI)     && (_end>=M_PI))     box.add(_cx - _r, _cy);
  if((_begin<=3*M_PI/2) && (_end>=3*M_PI/2)) box.add(_cx, _cy + _r);
  box.add(_cx + _r * cos(_begin), _cy + _r * sin(_begin));
  box.add(_cx + _r * cos(_end),   _cy + _r * sin(_end));
  return box;
}

void FigArc::translate(int dx, int dy)
{
  _cx += dx; _cy += dy;
}

//
// Arrow
//

void FigArrow::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  os << 
    XFIG_O_POLYLINE << " " << 
    T_POLYLINE      << " " <<
    _style          << " " << 
    _line_width     << " " <<
    _pen_color      << " " << // pen color
    DEFAULT         << " " << // fill color
    _depth          << " " << // depth
    DEFAULT         << " " << // pen (unused)
    UNFILLED        << " " << // area fill
    0.0             << " " << // style_val
    JOIN_MITER      << " " << // join style
    CAP_BUTT        << " " << // cap style
    DEFAULT         << " " << // for arc-box only
    1               << " " << // forward arrow
    0               << " " << // no backward arrows
    2               << std::endl;  // num points

  os << "\t" << 
    ARROW_CLOSED_TRIANGLE_INDENTED_BUTT << " " << // arrow type
    ARROW_FILLED                        << " " << // arrow style
    _line_width                         << " " << // thickness
    40.0*_line_width                    << " " << // width
    80.0*_line_width                   << std::endl;  // height

  os << "\t" << _x1 << " " << _y1 << " " << _x2 << " " << _y2 << std::endl;
}

BoundingBox FigArrow::bounding_box() const
{
  return BoundingBox(_x1,_y1,_x2,_y2);
}

void FigArrow::translate(int dx, int dy)
{
  _x1 += dx; _y1 += dy; _x2 += dx; _y2 += dy;
}

//
// Text
//

void FigText::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  const int CHAR_HEIGHT = 135;
  const int CHAR_WIDTH = 86;
  os << XFIG_O_TEXT << " ";
  switch(_h_align)
  {
    case 'l': os << T_LEFT_JUSTIFIED; break;
    case 'c': os << T_CENTER_JUSTIFIED; break;
    case 'r': os << T_RIGHT_JUSTIFIED; break;
    default:  os << T_LEFT_JUSTIFIED;
  }
  os << " " <<
    _pen_color         << " " <<
    _depth             << " " << // depth
    DEFAULT            << " " << // pen (unused)
    PSFONT_TIMES_ROMAN << " " << // font
    12.0               << " ";   // font size
  if(_orientation=='v')
  {
    os << 1.5708;
  }
  else
  {
    os << 0.0; // text angle
  }
  os << " " <<
    PSFONT_TEXT               << " " << // font flags
    CHAR_HEIGHT               << " " <<
    _msg.length()*CHAR_WIDTH  << " ";
    if(_orientation=='v')
    {
      switch(_v_align)
      {
        case 't': os << _x + CHAR_HEIGHT; break;
        case 'c': os << _x + CHAR_HEIGHT/2; break;
        case 'b': os << _x; break;
        default:  os << _x; break;
      }      
      os << " " << _y;
    }
    else
    {
      os << _x << " ";
      switch(_v_align)
      {
        case 't': os << _y + CHAR_HEIGHT; break;
        case 'c': os << _y + CHAR_HEIGHT/2; break;
        case 'b': os << _y; break;
        default:  os << _y; break;
      }
    }
    os << " " <<
    _msg << "\\001"            << std::endl;
}

BoundingBox FigText::bounding_box() const
{
  return BoundingBox(/* _x,_y, _x + _msg.length()*CHAR_WIDTH, _y - CHAR_HEIGHT */);
}

void FigText::translate(int dx, int dy)
{
  _x += dx; _y += dy;
}

//
// Color
//

void FigColor::save(std::ostream &os, int left, int right, int bottom, int top) const
{
  os << 
    XFIG_O_COLOR_DEF << " " << // user color definition
    _idx             << " " << // new color index
    "#" << std::hex << std::setw(6) << std::setfill('0') << (_color & 0xffffff) << std::setw(0) << std::dec << std::endl;
}
