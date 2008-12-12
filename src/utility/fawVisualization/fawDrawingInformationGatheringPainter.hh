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
 
#ifndef FAW_DRAWING_INFORMATION_GATHERING_PAINTER_HH
#define FAW_DRAWING_INFORMATION_GATHERING_PAINTER_HH

#include <set>

#include "fawPainter.hh"

namespace Faw {

class DrawingInformationGatheringPainter: public Painter
{
public:

  DrawingInformationGatheringPainter() {}

  inline void clear() { _used_colors.clear(); _used_layers.clear(); }

  typedef std::set<DrawingStyle::Color>::const_iterator color_iterator;
  inline color_iterator used_colors_begin() const { return _used_colors.begin(); }
  inline color_iterator used_colors_end()   const { return _used_colors.end();   }

  typedef std::set<unsigned int>::const_iterator layer_iterator;
  inline layer_iterator used_layers_begin() const { return _used_layers.begin(); }
  inline layer_iterator used_layers_end()   const { return _used_layers.end();   }

protected:

  virtual void _draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points) {}
  virtual void _draw_cross(double x, double y) {}
  virtual void _draw_pixel(double x, double y) {}
  virtual void _draw_node(double x, double y) {}
  virtual void _draw_segment(double x1, double y1, double x2, double y2) {}
  virtual void _draw_ellipse(double x, double y, double a, double b, double alpha) {}
  virtual void _draw_circle(double x, double y, double r) {}
  virtual void _draw_arc(double x, double y, double radius, double begin, double end) {}
  virtual void _draw_arrow(double x1, double y1, double x2, double y2) {}
  virtual void _draw_text(double x, double y, const char*) {}

  virtual void _style_changed()
  {
    _used_colors.insert(get_pen_color());
    _used_colors.insert(get_fill_color());
  }
  
  virtual void _layer_changed()
  {
    _used_layers.insert(get_layer());
  }

private:

  std::set<DrawingStyle::Color> _used_colors;
  std::set<unsigned int> _used_layers;
};

} // namespace Faw

#endif
