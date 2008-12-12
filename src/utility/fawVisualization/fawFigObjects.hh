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
 
#ifndef FAW_FIG_OBJECTS_HH
#define FAW_FIG_OBJECTS_HH

#include <iostream>
#include <vector>
#include <string>

#include "fawBoundingBox.hh"
#include "fawPainter.hh"

namespace Faw {

class FigObject
{
public:

  virtual ~FigObject() {}
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const = 0;
  virtual BoundingBox bounding_box() const = 0;
  virtual void translate(int dx, int dy) = 0;

  //
  // Constants
  //
  
  static const int DEFAULT = -1;
  static const int UNFILLED = -1;
  static const int FULL_COLOR_SATURATION = 20;
  
  static const int DEF_ARROW_WID = 4;
  static const int DEF_ARROW_HT = 8;

  static const int CAP_BUTT = 0;
  static const int CAP_ROUND = 1;
  static const int CAP_PROJECT = 2;

  enum EllipseConstants
  {
    T_ELLIPSE_BY_RAD = 1,
    T_ELLIPSE_BY_DIA = 2,
    T_CIRCLE_BY_RAD = 3,
    T_CIRCLE_BY_DIA = 4
  };

  enum ArcConstants
  {
    T_OPEN_ARC = 0,
    T_PIE_WEDGE_ARC = 1
  };

  static const int CLOSED_PATH = 0;
  static const int OPEN_PATH = 1;
  static const int DEF_BOXRADIUS = 7;
  static const int DEF_DASHLENGTH = 4;
  static const int DEF_DOTGAP = 3;

  enum LineConstants
  {
    T_POLYLINE = 1,
    T_BOX = 2,
    T_POLYGON = 3,
    T_ARC_BOX = 4,
    T_PICTURE = 5
  };
  
  static const int JOIN_MITER = 0;
  static const int JOIN_ROUND = 1;
  static const int JOIN_BEVEL = 2;

  
  enum ArrowType
  {
    ARROW_STICK = 0,
    ARROW_CLOSED_TRIANGLE = 1,
    ARROW_CLOSED_TRIANGLE_INDENTED_BUTT = 2,
    ARROW_CLOSED_TRIANGLE_POINTED_BUTT = 3
  };
  enum ArrowStyle
  {
    ARROW_HOLLOW = 0,
    ARROW_FILLED = 1
  };

  enum TextConstants
  {
    T_LEFT_JUSTIFIED = 0,
    T_CENTER_JUSTIFIED = 1,
    T_RIGHT_JUSTIFIED = 2
  };

  static const int RIGID_TEXT = 1;
  static const int SPECIAL_TEXT = 2;
  static const int PSFONT_TEXT = 4;
  static const int HIDDEN_TEXT = 8;

  enum TextPostscriptFonts
  {
    PSFONT_TIMES_ROMAN = 0,
    PSFONT_COURIER = 12,
    PSFONT_HELVETICA = 16
  };

  enum ObjectCodes
  {
    XFIG_O_COLOR_DEF        = 0,
    XFIG_O_ELLIPSE          = 1,
    XFIG_O_POLYLINE         = 2,
    XFIG_O_SPLINE           = 3,
    XFIG_O_TEXT             = 4,
    XFIG_O_ARC              = 5,
    XFIG_O_COMPOUND         = 6,
    XFIG_O_END_COMPOUND     = -XFIG_O_COMPOUND,
    XFIG_O_ALL_OBJECT       = 99
  };
};

//
// Polygon
//

class FigPolygon: public FigObject
{
public:
  FigPolygon
  (
    int num_points,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int area_fill = UNFILLED,
    int fill_color = DEFAULT,
    int depth = 0
  ) : 
    _points(num_points),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _area_fill(area_fill),
    _fill_color(fill_color),
    _depth(depth)
  {}

  void set_point(int i, int x, int y) { _points[i].first = x; _points[i].second = y; }
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  std::vector<std::pair<int,int> > _points;
  int _style;
  int _line_width;
  int _pen_color;
  int _area_fill;
  int _fill_color;
  int _depth;
};

//
// Chain
//

class FigChain: public FigObject
{
public:
  FigChain
  (
    int num_points,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int depth = 0
  ) : 
    _points(num_points),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _depth(depth)
  {}

  void set_point(int i, int x, int y) { _points[i].first = x; _points[i].second = y; }
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  std::vector<std::pair<int,int> > _points;
  int _style;
  int _line_width;
  int _pen_color;
  int _depth;
};

//
// Segment
//

class FigSegment: public FigObject
{
public:
  enum BoundaryType
  {
    SEGMENT,
    RAY,
    LINE
  };
  
  FigSegment
  (
    int x1, int y1, int x2, int y2,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int depth = 0,
    BoundaryType boundary_type = SEGMENT
  ) : 
    _x1(x1), _y1(y1), _x2(x2), _y2(y2),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _depth(depth),
    _boundary_type(boundary_type)
  {}

  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  int _x1,_y1,_x2,_y2;
  int _style;
  int _line_width;
  int _pen_color;
  int _depth;
  BoundaryType _boundary_type;
};

//
// Ellipse
//

class FigEllipse: public FigObject
{
public:
  FigEllipse
  (
    int cx, int cy, int a, int b,
    double alpha = 0,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int area_fill = UNFILLED,
    int fill_color = DEFAULT,
    int depth = 0
  ) : 
    _cx(cx), _cy(cy), _a(a), _b(b),
    _alpha(alpha),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _area_fill(area_fill),
    _fill_color(fill_color),
    _depth(depth) 
  {}
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  int _cx,_cy,_a,_b;
  double _alpha;
  int _style;
  int _line_width;
  int _pen_color;
  int _area_fill;
  int _fill_color;
  int _depth;
};

//
// Arc
//

class FigArc: public FigObject
{
public:
  FigArc
  (
    int cx, int cy, int r, double begin, double end,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int area_fill = UNFILLED,
    int fill_color = DEFAULT,
    int depth = 0
  );
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  int _cx,_cy,_r;
  double _begin,_end;
  int _style;
  int _line_width;
  int _pen_color;
  int _area_fill;
  int _fill_color;
  int _depth;
};

//
// Arrow
//

class FigArrow: public FigObject
{
public:
  FigArrow
  (
    int x1, int y1, int x2, int y2,
    int style = DEFAULT,
    int line_width = 1,
    int pen_color = DEFAULT,
    int depth = 0
  ) : 
    _x1(x1), _y1(y1), _x2(x2), _y2(y2),
    _style(style),
    _line_width(line_width),
    _pen_color(pen_color),
    _depth(depth) 
  {}
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

private:
  int _x1,_y1,_x2,_y2;
  int _style;
  int _line_width;
  int _pen_color;
  int _depth;
};

//
// Text
//

class FigText: public FigObject
{
public:
  FigText
  (
    int x, int y,
    const char *msg,
    int pen_color = DEFAULT,
    int depth = 0
  ) : 
    _x(x), _y(y),
    _msg(msg),
    _pen_color(pen_color),
    _depth(depth),
    _orientation('h'),
    _h_align('l'),
    _v_align('b')
  {}
  
  void set_align(char o, char h, char v)
  {
    _orientation = o;
    _h_align = h;
    _v_align = v;
  }

  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const;
  virtual void translate(int dx, int dy);

  static const int CHAR_HEIGHT = 135;
  static const int CHAR_WIDTH = 86;

private:
  int _x,_y;
  std::string _msg;
  int _pen_color;
  int _depth;
  char _orientation;
  char _h_align;
  char _v_align;
};

//
// Color
//

class FigColor: public FigObject
{
public:
  FigColor(unsigned int color, int idx) : _color(color), _idx(idx) {}
  
  virtual void save(std::ostream &os, int left, int right, int bottom, int top) const;
  virtual BoundingBox bounding_box() const { return BoundingBox(); }
  virtual void translate(int dx, int dy) { return; }

private:
  unsigned int _color;
  int _idx;
};

} // namespace Faw

#endif
