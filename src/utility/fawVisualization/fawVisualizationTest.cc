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
 
#include <stdio.h>
#include <fstream>

#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qtimer.h>

#include "fawVisualizationTest.hh"
#include "fawPainter.hh"
#include "fawFigPainter.hh"
#include "fawDrawingStoreReplayPainter.hh"
#include "fawDrawingNode.hh"
#include "fawDrawingGroupNode.hh"
#include "fawDrawingPolygonNode.hh"
#include "fawDrawingTransformationNode.hh"
#include "fawDrawingStyleNode.hh"
#include "fawDrawingPrintAction.hh"
#include "fawDrawingWidgetRegionZoom.hh"
#include "fawClippingPainter.hh"

using namespace Faw;

DrawingWidgetTest::DrawingWidgetTest()
  : QMainWindow(0, "DrawingWidgetTest"),
    _drawing_widget(0)
{
  setCaption("Test for Faw::DrawingWidget");

  // Main Widget
  QVBox *box = new QVBox(this);
  setCentralWidget(box);

  _drawing_widget = new DrawingWidget(box, "drawing widget");

  QPushButton *save_fig_button = new QPushButton("Save as FIG", box);
  QPushButton *save_eps_button = new QPushButton("Save as EPS", box);

  QObject::connect(save_fig_button, SIGNAL(clicked()), this, SLOT(save_fig()));
  QObject::connect(save_eps_button, SIGNAL(clicked()), this, SLOT(save_eps()));

  QObject::connect(_drawing_widget, SIGNAL(mouse_button_pressed(double, double, ButtonState)), this, SLOT(mouse_button_pressed(double, double, ButtonState)));
  QObject::connect(_drawing_widget, SIGNAL(mouse_moved(double, double, ButtonState)), this, SLOT(mouse_moved(double, double, ButtonState)));
  QObject::connect(_drawing_widget, SIGNAL(mouse_button_released(double, double, ButtonState)), this, SLOT(mouse_button_released(double, double, ButtonState)));
  QObject::connect(_drawing_widget, SIGNAL(redraw(Faw::Painter&)), this, SLOT(redraw(Faw::Painter&)));

  QObject::connect(this, SIGNAL(drawing_changed()), _drawing_widget, SLOT(model_changed()));

  _drawing_widget->setFocus();

  DrawingWidgetRegionZoom *region_zoom = new DrawingWidgetRegionZoom(_drawing_widget);
  region_zoom->activate(Qt::MidButton | Qt::ControlButton);

  _drawing_widget->fit_view_port(-3, 3, -3, 3);

  DrawingNode *root_node = _create_model();
  DrawingPrintAction print_scene(std::cout);
  print_scene.apply(root_node);
  _drawing_widget->set_scene(root_node);
  
  resize( 500, 500 );
}



DrawingWidgetTest::~DrawingWidgetTest()
{
  
}

void DrawingWidgetTest::mouse_button_pressed(double x, double y, ButtonState bs)
{
  if(bs & Qt::LeftButton)
  {
    _started_drawing.push_back(Painter::Point(x,y));
    emit drawing_changed();
  }
}

void DrawingWidgetTest::mouse_moved(double x, double y, ButtonState bs)
{
  if(bs & Qt::LeftButton)
  {
    _started_drawing.push_back(Painter::Point(x,y));
    emit drawing_changed();
  }
}

void DrawingWidgetTest::mouse_button_released(double x, double y, ButtonState bs)
{
  if(bs & Qt::LeftButton)
  {
    if(!_started_drawing.empty())
    {
      _old_drawings.push_back(_started_drawing);
      _started_drawing.clear();
    }
  }
}


void DrawingWidgetTest::redraw(Painter &painter)
{
  ClippingPainter cp(painter, -1,1,-1,1);
  cp.begin();
  cp.draw_chain(_started_drawing);
  for(unsigned int i=0; i<_old_drawings.size(); ++i)
  {
    cp.draw_chain(_old_drawings[i]);
  }
  cp.end();
/*
  painter.draw_chain(_started_drawing);
  for(unsigned int i=0; i<_old_drawings.size(); ++i)
  {
    painter.draw_chain(_old_drawings[i]);
  }
*/
  painter.draw_segment(10,10,20,10);
  painter.draw_text(10,10,"Hi there!", 'h', 'r', 'b');
  painter.draw_text(20,10,"Hi there!", 'h', 'l', 'c');
  painter.draw_text(15,10,"Hi there!", 'h', 'c', 'b');
}



void DrawingWidgetTest::save_fig()
{
  std::ofstream ofs("drawing.fig");
  FigPainter painter;
  _drawing_widget->render(painter);
  painter.save(ofs);
}



void DrawingWidgetTest::save_eps()
{
  FigPainter painter;
  _drawing_widget->render(painter);
  painter.save("drawing.eps", "eps");
}



DrawingNode *DrawingWidgetTest::_create_model() const
{
  DrawingGroupNode *group_node;
  DrawingTransformation xform;
  DrawingStyle style;
  DrawingPolygon polygon;

  polygon.append( 1, 1);
  polygon.append(-1, 1);
  polygon.append(-1,-1);
  polygon.append( 1,-1);

  DrawingGroupNode *root_node = new DrawingGroupNode;

  root_node->add(new DrawingPolygonNode(polygon));

  root_node->add(group_node = new DrawingGroupNode);
  xform.set(2,2,0);
  style.set_pen_width(3);
  style.set_pen_color(0xff0000);
  group_node->add(new DrawingStyleNode(style));
  group_node->add(new DrawingTransformationNode(xform));
  group_node->add(new DrawingPolygonNode(polygon));

  root_node->add(group_node = new DrawingGroupNode);
  xform.set(-2,2,22.5*M_PI/180);
  style.set_pen_width(2);
  style.set_pen_color(0x00ff00);
  group_node->add(new DrawingStyleNode(style));
  group_node->add(new DrawingTransformationNode(xform));
  group_node->add(new DrawingPolygonNode(polygon));

  root_node->add(group_node = new DrawingGroupNode);
  xform.set(-2,-2,45*M_PI/180);
  style.set_pen_width(1);
  style.set_pen_color(0x0000ff);
  group_node->add(new DrawingStyleNode(style));
  group_node->add(new DrawingTransformationNode(xform));
  group_node->add(new DrawingPolygonNode(polygon));

  root_node->add(group_node = new DrawingGroupNode);
  xform.set(2,-2,67.5*M_PI/180);
  style.set_pen_width(2);
  style.set_pen_color(0xff00ff);
  style.set_fill_color(0xccccff);
  style.set_fill(true);
  group_node->add(new DrawingStyleNode(style));
  group_node->add(new DrawingTransformationNode(xform));
  group_node->add(new DrawingPolygonNode(polygon));

  return root_node;
}


int main( int argc, char **argv )
{
  int ret = 0;

  QApplication::setColorSpec(QApplication::CustomColor);
  QApplication app(argc,argv);

  DrawingWidgetTest w;
  app.setMainWidget(&w);
  w.show();
  ret = app.exec();

  return ret;
}
