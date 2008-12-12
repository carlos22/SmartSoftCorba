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
 
#ifndef FAW_DRAWING_STYLES_HH
#define FAW_DRAWING_STYLES_HH

#include <iostream>

#if (__GNUC__ >= 3)
#include <ext/hash_map>
#else
#include <hash_map>
#endif

namespace Faw {

class DrawingStyle
{
public:

  enum PointStyle 
  { 
    POINT, CROSS, NODE 
  };

  enum LineStyle
  { 
    SOLID = 0,
    DASH = 1,
    DOTTED = 2,
    DASH_DOT = 3,
    DASH_DOT_DOT = 4
  };
  
  typedef unsigned int Color;

  inline static Color color(int red, int green, int blue) 
  { 
    return Color(((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff));
  }

  inline static unsigned char red(Color c) { return ((c & 0xff0000) >> 16); }
  inline static unsigned char green(Color c) { return ((c & 0xff00) >> 8); }
  inline static unsigned char blue(Color c) { return (c & 0xff); }

  struct ColorHashFunc
  {
    inline size_t operator()(const Color &c) const
    {
      return ((c & 0xff0000) >> 10) ^ ((c & 0xff00) >> 5) ^ (c & 0xff);
    }
  };

  enum
  {
    RGB_BLACK   = 0x000000,
    RGB_RED     = 0xff0000,
    RGB_GREEN   = 0x00ff00,
    RGB_BLUE    = 0x0000ff,
    RGB_CYAN    = 0x00ffff,
    RGB_MAGENTA = 0xff00ff,
    RGB_YELLOW  = 0xffff00,
    RGB_WHITE   = 0xffffff,

    RGB_DARK_RED     = 0x800000,
    RGB_DARK_GREEN   = 0x008000,
    RGB_DARK_BLUE    = 0x000080,
    RGB_DARK_CYAN    = 0x008080,
    RGB_DARK_MAGENTA = 0x800080,
    RGB_DARK_YELLOW  = 0x808000,

    RGB_LIGHT_RED     = 0xff8080,
    RGB_LIGHT_GREEN   = 0x80ff80,
    RGB_LIGHT_BLUE    = 0x8080ff,
    RGB_LIGHT_CYAN    = 0x80ffff,
    RGB_LIGHT_MAGENTA = 0xff80ff,
    RGB_LIGHT_YELLOW  = 0xffff80
  };

  static const int NUM_COLORS = 20;
  static const Color COLORS[NUM_COLORS];

  static const int NUM_DARK_COLORS = 6;
  static const int FIRST_DARK_COLOR = 1;
  static const int NUM_NORMAL_COLORS = 6;
  static const int FIRST_NORMAL_COLOR = 7;
  static const int NUM_LIGHT_COLORS = 6;
  static const int FIRST_LIGHT_COLOR = 13;

  DrawingStyle() : _pen_width(1), _pen_color(RGB_BLACK), _line_style(SOLID), _fill(false), _fill_color(RGB_WHITE) {}
  
  int get_pen_width() const { return _pen_width; }
  Color get_pen_color() const { return _pen_color; }
  LineStyle get_line_style() const { return _line_style; }
  bool get_fill() const { return _fill; }
  Color get_fill_color() const { return _fill_color; }

  void set_pen_width(int w) { _pen_width = w; }
  void set_pen_color(Color c) { _pen_color = c; }
  void set_line_style(LineStyle s) { _line_style = s; }
  void set_fill(bool f) { _fill = f; }
  void set_fill_color(Color c) { _fill_color = c; }

  void print(std::ostream &os) const
  {
    os << "(pen=(width=" << _pen_width << ", color=" << std::hex << _pen_color << std::dec << ", style=" << int(_line_style)
       << "), fill=(flag=" << int(_fill) << ", color=" << std::hex << _fill_color << std::dec << "))";
  }

  void save(std::ostream &os) const
  {
    os << _pen_width << "\t" << _pen_color << "\t" << (int)_line_style << "\t" << _fill << "\t" << _fill_color << std::endl;
  }

  void load(std::istream &is)
  {
    int style;
    is >> _pen_width >> _pen_color >> style >> _fill >> _fill_color;
    _line_style = (LineStyle)style;
  }

private:
  int _pen_width;
  Color _pen_color;
  LineStyle _line_style;
  bool _fill;
  Color _fill_color;
};

inline std::ostream &operator<<(std::ostream &os, const DrawingStyle &s)
{
  s.print(os); return os;
}

} // namespace Faw

#endif
