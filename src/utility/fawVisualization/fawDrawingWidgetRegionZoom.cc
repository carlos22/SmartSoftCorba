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
 
#include "fawDrawingWidgetRegionZoom.hh"

using namespace Faw;

DrawingWidgetRegionZoom::DrawingWidgetRegionZoom(DrawingWidget *dw)
  : QObject(dw),
    _active(false),
    _is_selecting_region(false)
{
  QObject::connect(this, SIGNAL(fit_view_port(double, double, double, double)), dw, SLOT(fit_view_port(double, double, double, double)));
  QObject::connect(this, SIGNAL(region_changed()), dw, SLOT(model_changed()));
  QObject::connect(dw, SIGNAL(post_redraw(Faw::Painter&)), this, SLOT(redraw(Faw::Painter&)));
  QObject::connect(dw, SIGNAL(mouse_clicked(double, double, ButtonState)), this, SLOT(mouse_clicked(double, double, ButtonState)));
  QObject::connect(dw, SIGNAL(mouse_moved(double, double, ButtonState)), this, SLOT(mouse_moved(double, double, ButtonState)));
}

void DrawingWidgetRegionZoom::activate(unsigned int button_state)
{
  if(!_active || (_button_state!=button_state))
  {
    if(_is_selecting_region)
    {
      _is_selecting_region = false;
      emit region_changed();
    }
    _active = true;
    _button_state = button_state;
  }
}

void DrawingWidgetRegionZoom::deactivate()
{
  if(_active)
  {
    if(_is_selecting_region)
    {
      _is_selecting_region = false;
      emit region_changed();
    }
    _active = false;
  }
}

void DrawingWidgetRegionZoom::redraw(Painter &painter)
{
  if(_is_selecting_region)
  {
    painter.begin();
    DrawingStyle style;
    style.set_line_style(DrawingStyle::DOTTED);
    painter.set_style(style);
    painter.draw_segment(_x1,_y1,_x1,_y2);
    painter.draw_segment(_x1,_y2,_x2,_y2);
    painter.draw_segment(_x2,_y2,_x2,_y1);
    painter.draw_segment(_x2,_y1,_x1,_y1);
    painter.end();
  }
}

void DrawingWidgetRegionZoom::mouse_clicked(double x, double y, Qt::ButtonState bs)
{
  if((bs & _button_state)==_button_state)
  {
    if(_active)
    {
      if(_is_selecting_region)
      {
        _is_selecting_region = false;
        emit fit_view_port(_min(_x1,x), _max(_x1,x), _min(_y1,y), _max(_y1,y));
      }
      else
      {
        _x1 = x; _y1 = y;
        _is_selecting_region = true;
      }
    }
  }
  else
  {
    if(_is_selecting_region)
    {
      _is_selecting_region = false;
      emit region_changed();
    }
  }
}

void DrawingWidgetRegionZoom::mouse_moved(double x, double y, Qt::ButtonState)
{
  if(_is_selecting_region)
  {
    _x2 = x; _y2 = y;
    emit region_changed();
  }
}

