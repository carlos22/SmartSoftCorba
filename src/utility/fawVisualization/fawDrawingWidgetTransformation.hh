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
 
#ifndef FAW_DRAWING_WIDGET_TRANSFORMATION_H
#define FAW_DRAWING_WIDGET_TRANSFORMATION_H

#include <math.h>
#include <qpoint.h>

namespace Faw {

/**
  Transformation of world coordinates into window coordinates.
  This class provides functors (i.e., it overloads \c operator()) to
  convert world coordinates into window coordinates.
  \author Boris Kluge
  \date 15.05.2001
 */
class DrawingWidgetTransformation
{
public:
  DrawingWidgetTransformation(double zoom, int dx, int dy, int width, int height)
    : _zoom(zoom), _inv_zoom(1.0/zoom), _dx(dx), _dy(dy)
  {
    _left   = win2model_x(0);
    _right  = win2model_x(width - 1);
    _top    = win2model_y(0);
    _bottom = win2model_y(height - 1);
  }
  
  inline double left() const { return _left; }
  inline double right() const { return _right; }
  inline double top() const { return _top; }
  inline double bottom() const { return _bottom; }

  inline bool is_visible(double x, double y) const { return ((_left<=x) && (x<=_right) && (_bottom<=y) && (y<=_top)); }

  /**
    Transforms a point from model coordinates (millimeters) into window
    coordinates (pixels).
   */
  inline QPoint operator()(double x, double y) const { return QPoint(model2win_x(x), model2win_y(y)); }

  /**
    Transforms a model distance (millimeters) into a window distance (pixels).
   */
  inline int operator()(double d) const { return model2win_d(d); }

  inline int  model2win_x(double model_x) const { return (int)floor( _zoom*model_x + _dx + 0.5); }
  inline int  model2win_y(double model_y) const { return (int)floor(-_zoom*model_y + _dy + 0.5); }
  inline int  model2win_d(double model_d) const { return (int)floor(_zoom*model_d + 0.5); }
  inline void model2win(double model_x, double model_y, int &win_x, int &win_y) const
  {
    win_x = model2win_x(model_x);
    win_y = model2win_y(model_y);
  }

  inline double win2model_x(int win_x) const { return (win_x - _dx) * _inv_zoom; }
  inline double win2model_y(int win_y) const { return  (_dy - win_y) * _inv_zoom; }
  inline double win2model_d(int win_d) const { return win_d * _inv_zoom; }
  inline void   win2model(int win_x, int win_y, double &model_x, double &model_y) const
  {
    model_x = win2model_x(win_x);
    model_y = win2model_y(win_y);
  }

private:
  const double _zoom, _inv_zoom;
  const int _dx, _dy;
  double _left, _right, _top, _bottom;
};

} // namespace Faw

#endif
