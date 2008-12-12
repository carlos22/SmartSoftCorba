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
 
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qpen.h>
#include <qpainter.h>
#include <qcursor.h>
#include <stdio.h>

#include "fawDrawingWidget.hh"
#include "fawDrawingRenderAction.hh"

Faw::DrawingWidget::DrawingWidget(QWidget *parent, const char *name, double length_unit)
  : QVBox(parent, name),
    _scene_root(0),
    _default_cursor(CrossCursor),
    _zooming_cursor(PointingHandCursor),
    _panning_cursor(SizeAllCursor),
    _draw_grid(true),
    _grid_pixels(60),
    _length_unit(length_unit),
    _inv_length_unit(1.0/length_unit),
    _first_resize(true),
    _fit_viewport_on_resize(false),
    _zoom(20), // pixels per meter 
    _delta_x(0),
    _delta_y(0),
    _mouse_mode(Faw::DrawingWidget::MOUSE_MODE_NORMAL)
{
  _drawing_area = new Faw::DrawingWidget::_MyDrawingArea(this);
  _drawing_area->setBackgroundMode(NoBackground);
  _drawing_area->setCursor(QCursor(_default_cursor));
  _delta_y = _drawing_area->size().height(); // origin in the lower left corner
  _drawing_buffer.resize(_drawing_area->size());
  this->setStretchFactor(_drawing_area, 1);

  _status_bar = new QStatusBar(this);
  _status_bar->setSizeGripEnabled(false);
  this->setStretchFactor(_status_bar, 0);

  _double_buffering = false;
  this->set_double_buffering(true);
}



Faw::DrawingWidget::~DrawingWidget()
{
  std::map<void*, _AutoPaintableInterface*>::iterator iter = _autopainted_things.begin();
  while(iter!=_autopainted_things.end())
  {
    delete (*iter).second;
    ++iter;
  }
  _autopainted_things.clear();
}



void Faw::DrawingWidget::set_scene(Faw::DrawingNode *root)
{
  Faw::DrawingNode *tmp_scene_root = _scene_root;
  _scene_root = root;
  if(tmp_scene_root)
  {
    delete tmp_scene_root; // notifies node container about destruction
  }
  else if(root)
  {
    model_changed();
  }
}



void Faw::DrawingWidget::clear_scene()
{
  if(_scene_root)
  {
    Faw::DrawingNode *tmp_scene_root = _scene_root;
    _scene_root = 0;
    delete tmp_scene_root; // notifies node container about destruction
  }
}
      


Faw::DrawingWidgetTransformation Faw::DrawingWidget::transformation() const
{
  return Faw::DrawingWidgetTransformation(_zoom, _delta_x, _delta_y, _drawing_area->width(), _drawing_area->height());
}



void Faw::DrawingWidget::model_changed()
{
  _drawing_area->update();
}



void Faw::DrawingWidget::render(Faw::Painter &painter)
{
  painter.begin();
  painter.set_length_unit(_length_unit);

  // draw additional background
  emit pre_redraw(painter);

  // draw auto-painted things
  std::map<void*, _AutoPaintableInterface*>::iterator iter = _autopainted_things.begin();
  while(iter!=_autopainted_things.end())
  {
    (*iter).second->paint(painter);
    ++iter;
  }

  // draw scene graph
  if(_scene_root)
  {
    Faw::DrawingRenderAction render_action(painter);
    render_action.apply(_scene_root);
  }

  // call general client painting code
  emit redraw(painter);

  // draw on top of all other objects
  emit post_redraw(painter);

  painter.end();
}



void Faw::DrawingWidget::fit_view_port(double left, double right, double bottom, double top, const double unit)
{
  if(_first_resize)
  {
    _fit_viewport_on_resize = true;
    _fit_viewport_on_resize_left = left;
    _fit_viewport_on_resize_right = right;
    _fit_viewport_on_resize_top = top;
    _fit_viewport_on_resize_bottom = bottom;
    _fit_viewport_on_resize_unit = unit;
  }
  else
  {
    const int win_width  = _drawing_area->width();
    const int win_height = _drawing_area->height();
    const double view_width = fabs(right - left)*unit;
    const double view_height = fabs(top - bottom)*unit;
    const double width_zoom = win_width / view_width;
    const double height_zoom = win_height / view_height;

    if(width_zoom<height_zoom) _zoom = width_zoom; else _zoom = height_zoom;
    center_on((left+right)/2, (top+bottom)/2, unit);
  }
}



void Faw::DrawingWidget::center_on(double x, double y, const double unit)
{
  const int width  = _drawing_area->width();
  const int height = _drawing_area->height();

  int win_x, win_y;
  _model2win(x*unit, y*unit, win_x, win_y);
  const int dx = win_x - (width/2);
  const int dy = win_y - (height/2);

  _delta_x -= dx;
  _delta_y -= dy;

  _drawing_area->update();
}

void Faw::DrawingWidget::set_double_buffering(bool b)
{
  if(b!=_double_buffering)
  {
    if(b)
    {
      _drawing_area->setBackgroundMode(NoBackground);
    }
    else
    {
      _drawing_area->setBackgroundColor(QColor("white"));
    }
    _double_buffering = b;
  }
}

double Faw::DrawingWidget::_tick_round()
{
  const double min_grid_dist = _grid_pixels / _zoom;
  const int exponent = (int)floor(log10(min_grid_dist));
  const double pow10_exponent = pow(10,exponent);
  const double mantissa = min_grid_dist / pow10_exponent; // 1 <= mantissa < 10
  
  if(mantissa>5)
  {
    return 10*pow10_exponent;
  }
  else
  {
    return 5*pow10_exponent;
  }
}

void Faw::DrawingWidget::_paint_event(QPaintEvent *pe)
{
  Faw::DrawingWidgetTransformation dwt = transformation();
  QPainter painter;
  if(_double_buffering)
  {
    _drawing_buffer.fill();//_drawing_area->backgroundColor());
    painter.begin(&_drawing_buffer, _drawing_area);
  }
  else
  {
    _drawing_area->erase();
    painter.begin(_drawing_area);
  }

  Faw::DrawingWidgetPainter dwp(painter, dwt);
  std::set<unsigned int>::iterator iter;
  for(iter = _hidden_layers.begin(); iter!=_hidden_layers.end(); ++iter)
  {
    dwp.hide_layer(*iter);
  }
  
  Faw::Painter &faw_painter(dwp);

  if(_draw_grid)
  {
    faw_painter.begin(); // grid drawing
    faw_painter.set_pen_color(60,60,60);
    faw_painter.set_line_style(Faw::DrawingStyle::DOTTED);

    static const int BUFLEN = 20;
    char number[BUFLEN];
    const char *format;

    const double one_pixel = 1.0/_zoom;
    const double grid = _tick_round();
    double scale = 1;
    if(grid<0.999e-9)
    {
      format = "%.2gm";
    }
    else if(grid<0.999e-6)
    {
      format = "%.0fnm";
      scale = 1e+9;
    }
    else if(grid<0.999e-3)
    {
      format = "%.0fµm";
      scale = 1e+6;
    }
    else if(grid<0.999e-2)
    {
      format = "%.0fmm";
      scale = 1e+3;
    }
    else if(grid<0.099)
    {
      format = "%.0fcm";
      scale = 100;
    }
    else if(grid<0.999)
    {
      format = "%.1fm";
    }
    else if(grid<499)
    {
      format = "%.0fm";
    }
    else if(grid<999)
    {
      format = "%.1fkm";
      scale = 1e-3;
    }
    else
    {
      format = "%.0fkm";
      scale = 1e-3;
    }

    int first_line, last_line;

    first_line = (int)ceil(dwt.bottom()/grid);
    last_line  = (int)floor(dwt.top()/grid);
    for(int i=first_line; i<=last_line; i++)
    {
      snprintf(number, BUFLEN-1, format, i*grid*scale);
      faw_painter.draw_line(dwt.right(),i*grid,dwt.left(),i*grid);
      faw_painter.draw_text(dwt.left() + one_pixel, i*grid + one_pixel, number);
    }

    first_line = (int)ceil(dwt.left()/grid);
    last_line  = (int)floor(dwt.right()/grid);
    for(int i=first_line; i<=last_line; i++)
    {
      snprintf(number, BUFLEN-1, format, i*grid*scale);
      faw_painter.draw_line(i*grid, dwt.bottom(), i*grid, dwt.top());
      faw_painter.draw_text(i*grid + one_pixel, dwt.bottom() + one_pixel, number);
    }
    faw_painter.end();  // grid drawing
  }

  this->render(faw_painter);

  if(_double_buffering)
  {
    painter.end();
    bitBlt(_drawing_area, 0, 0, &_drawing_buffer);
  }
  else
  {
    painter.end();
  }
}



void Faw::DrawingWidget::_mouse_press_event(QMouseEvent *me)
{
  double x,y;
  _win2model(me->pos(), x, y);

  Qt::ButtonState bs = (Qt::ButtonState)(me->state() | me->button());
  if((bs & ControlButton) && (bs & RightButton))
  {
    _mouse_mode = Faw::DrawingWidget::MOUSE_MODE_ZOOMING;
    _mouse_action_start_x = me->x();
    _mouse_action_start_y = me->y();
    _original_delta_x = _delta_x;
    _original_delta_y = _delta_y;
    _original_zoom = _zoom;
    _drawing_area->setCursor(QCursor(_zooming_cursor));
  }
  else if((bs & ControlButton) && (bs & LeftButton))
  {
    _mouse_mode = Faw::DrawingWidget::MOUSE_MODE_PANNING;
    _mouse_action_start_x = me->x();
    _mouse_action_start_y = me->y();
    _original_delta_x = _delta_x;
    _original_delta_y = _delta_y;
    _drawing_area->setCursor(QCursor(_panning_cursor));
  }
  else
  {
    emit mouse_button_pressed(x*_inv_length_unit, y*_inv_length_unit, bs);
  }
}



void Faw::DrawingWidget::_mouse_release_event(QMouseEvent *me)
{
  double x,y;
  _win2model(me->pos(), x, y);

  x *= _inv_length_unit;
  y *= _inv_length_unit;

  _mouse_mode = Faw::DrawingWidget::MOUSE_MODE_NORMAL;
  _drawing_area->setCursor(QCursor(_default_cursor));

  Qt::ButtonState bs = (Qt::ButtonState)(me->state() | me->button());
  emit mouse_button_released(x, y, bs);
  emit mouse_clicked(x, y, bs);
  if(bs & LeftButton)
  {
    if((bs & ShiftButton) == ShiftButton)
    {
      emit shift_left_click(x,y);
    }
    else if((bs & ControlButton) == ControlButton)
    {
      emit ctrl_left_click(x,y);
    }
    else if((bs & AltButton) == AltButton)
    {
      emit alt_left_click(x,y);
    }
    else if(bs==LeftButton)
    {
      emit left_click(x,y);
    }
  }
  else if(bs & MidButton)
  {
    if((bs & ShiftButton) == ShiftButton)
    {
      emit shift_middle_click(x,y);
    }
    else if((bs & ControlButton) == ControlButton)
    {
      emit ctrl_middle_click(x,y);
    }
    else if((bs & AltButton) == AltButton)
    {
      emit alt_middle_click(x,y);
    }
    else if(bs==MidButton)
    {
      emit middle_click(x,y);
    }
  }
  else if(bs & RightButton)
  {
    if((bs & ShiftButton) == ShiftButton)
    {
      emit shift_right_click(x,y);
    }
    else if((bs & ControlButton) == ControlButton)
    {
      emit ctrl_right_click(x,y);
    }
    else if((bs & AltButton) == AltButton)
    {
      emit alt_right_click(x,y);
    }
    else if(bs==RightButton)
    {
      emit right_click(x,y);
    }
  }
}



void Faw::DrawingWidget::_mouse_double_click_event(QMouseEvent *me)
{
  double x,y;
  _win2model(me->pos(), x, y);
  emit mouse_double_clicked(x*_inv_length_unit, y*_inv_length_unit, (Qt::ButtonState)(me->state() | me->button()));
}



void Faw::DrawingWidget::_mouse_move_event(QMouseEvent *me)
{
  double x,y;
  _win2model(me->pos(), x, y);

  QString msg = "";
  msg.sprintf("(%.3f,%.3f)", x, y);
  _status_bar->message(msg);

  if(_mouse_mode==MOUSE_MODE_ZOOMING)
  {
    _zoom = _original_zoom * pow(2, 0.03*(_mouse_action_start_y - me->y()));
    if(_zoom < 1e-6) _zoom = 1e-6;

    if(_zoom!=_original_zoom)
    {
      _delta_x = (int)floor(0.5 + (_mouse_action_start_x - _zoom/_original_zoom * (_mouse_action_start_x - _original_delta_x)));
      _delta_y = (int)floor(0.5 + (_mouse_action_start_y - _zoom/_original_zoom * (_mouse_action_start_y - _original_delta_y)));
      _drawing_area->update();
    }
  }
  else if(_mouse_mode==MOUSE_MODE_PANNING)
  {
    _delta_x = _original_delta_x + me->x() - _mouse_action_start_x;
    _delta_y = _original_delta_y + me->y() - _mouse_action_start_y;
    _drawing_area->update();
  }
  else
  {
    emit mouse_moved(x*_inv_length_unit, y*_inv_length_unit, (Qt::ButtonState)(me->state() | me->button()));
  }
}



void Faw::DrawingWidget::_resize_event(QResizeEvent *re)
{
  _first_resize = false;
  if(_fit_viewport_on_resize)
  {
    _fit_viewport_on_resize = false;
    fit_view_port(_fit_viewport_on_resize_left, _fit_viewport_on_resize_right, _fit_viewport_on_resize_bottom, _fit_viewport_on_resize_top, _fit_viewport_on_resize_unit);
  }
  else
  {
    int h_diff = re->size().height() - re->oldSize().height();
    _delta_y += h_diff;
  }
  _drawing_buffer.resize(_drawing_area->size());
}
