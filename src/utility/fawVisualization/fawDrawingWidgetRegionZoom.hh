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
 
#ifndef FAW_DRAWING_WIDGET_REGION_ZOOM_HH
#define FAW_DRAWING_WIDGET_REGION_ZOOM_HH

#include <qobject.h>

#include "fawDrawingWidget.hh"

namespace Faw {

class DrawingWidgetRegionZoom: public QObject
{
  Q_OBJECT
public:
  DrawingWidgetRegionZoom(DrawingWidget *dw);
  
public slots:
  void activate(unsigned int button_state);
  void deactivate();

signals:
  void fit_view_port(double left, double right, double bottom, double top);
  void region_changed();

private slots:
  void redraw(Faw::Painter &painter);
  void mouse_clicked(double x, double y, ButtonState bs);
  void mouse_moved(double x, double y, ButtonState bs);

private:
  bool _active;
  unsigned int _button_state;

  bool _is_selecting_region;
  double _x1,_y1;
  double _x2,_y2;

  static inline double _min(double a, double b)  { return (a<b)?a:b; }
  static inline double _max(double a, double b)  { return (a<b)?b:a; }
};

} // namespace Faw

#endif
