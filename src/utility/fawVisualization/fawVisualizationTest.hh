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
 
#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <iostream>
#include <list>
#include <vector>
#include <utility>

#include <qmainwindow.h>

#include "fawDrawingWidget.hh"
#include "fawDrawingNode.hh"

class DrawingWidgetTest: public QMainWindow
{
  Q_OBJECT

public:
  DrawingWidgetTest();
  ~DrawingWidgetTest();

public slots:
  void mouse_button_pressed(double x, double y, ButtonState bs);
  void mouse_moved(double x, double y, ButtonState bs);
  void mouse_button_released(double x, double y, ButtonState bs);
  void redraw(Faw::Painter &painter);

  void save_fig();
  void save_eps();

signals:
  void drawing_changed();

private:
  Faw::DrawingWidget *_drawing_widget;
  double _mouse_pos_x, _mouse_pos_y;

  std::vector<Faw::Painter::Point> _started_drawing;
  std::vector<std::vector<Faw::Painter::Point> > _old_drawings;
  // you can take any point class you like as far as it provides an operator[](int)
  // to access x coordinated on index 0 and y coordinates on index 1

  Faw::DrawingNode *_create_model() const;
};

#endif
