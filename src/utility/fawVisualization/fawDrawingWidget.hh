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
 
#ifndef FAW_DRAWING_WIDGET_H
#define FAW_DRAWING_WIDGET_H

#include <math.h>
#include <iostream>
#include <map>
#include <set>
#include <utility>

#include <qwidget.h>
#include <qvbox.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qpixmap.h>

#include <fawDrawingWidgetTransformation.hh>
#include <fawDrawingWidgetPainter.hh>
#include <fawDrawingNode.hh>

namespace Faw {

/**
  A widget for 2D drawings.
  The drawing area is scrollable (panning) and zoomable.
  Scrolling and zooming is done by pressing CTRL and dragging the mouse around,
  pressing either the left or right button.
  \author Boris Kluge
  \date 10.05.2001
 */
class DrawingWidget: public QVBox
{
  Q_OBJECT

public:

  /**
    Constructor.
   */
  DrawingWidget(QWidget *parent = 0, const char * name = 0, double length_unit = 1.0);

  /**
    Destructor.
   */
  ~DrawingWidget();

  // experimental methods:
  void set_scene(DrawingNode *root);
  const DrawingNode *get_scene() const { return _scene_root; }
  DrawingNode *get_scene() { return _scene_root; }
  void clear_scene();

  template<class Container, class PaintMethod>
  inline void add_container_to_autopainting(Container *c, const PaintMethod &paint_method)
  {
    if(_autopainted_things.find(c)==_autopainted_things.end())
    {
      _AutoPaintableInterface *api = new _AutoPaintableContainer<Container, PaintMethod>(c, paint_method);
      _autopainted_things.insert(make_pair(c, api));
    }
  }

  template<class Container>
  inline void add_container_to_autopainting(Container *c)
  {
    DefaultPaintMethod<typename Container::value_type> paint_method;
    add_container_to_autopainting(c, paint_method);
  }

  template<class Object, class PaintMethod>
  inline void add_object_to_autopainting(Object *o, const PaintMethod &paint_method)
  {
    if(_autopainted_things.find(o)==_autopainted_things.end())
    {
      _AutoPaintableInterface *api = new _AutoPaintableObject<Object, PaintMethod>(o, paint_method);
      _autopainted_things.insert(make_pair(o, api));
    }
  }

  template<class Object>
  inline void add_object_to_autopainting(Object *o)
  {
    DefaultPaintMethod<Object> paint_method;
    add_object_to_autopainting(o, paint_method);
  }

  inline void remove_from_autopainting(void *p)
  {
    std::map<void*, _AutoPaintableInterface*>::iterator iter;
    iter = _autopainted_things.find(p);
    if(iter!=_autopainted_things.end())
    {
      delete (*iter).second;
      _autopainted_things.erase(iter);
    }
  }

  /**
    Returns the length unit used with this widget.
    This is a constant for each drawing widget's lifecycle.
   */
  double get_length_unit() const { return _length_unit; }

  /**
    Returns the transformation that is used to map model coordinates
    onto window coordinates.
   */
  DrawingWidgetTransformation transformation() const;

  /**
    Returns a pointer to the drawing area.
    Use this to do "dirty stuff".
   */
  QWidget *drawing_area() { return _drawing_area; }

public slots:

  /**
    Signals to the drawing widget that the model has changed.
    Activating this slots results in emitting the \c redraw signal.
   */
  void model_changed();

  /**
    Render the drawing onto the given painter.
    Use this slot for example to implement an export function:
    @code
      Faw::DrawingWidget *dw;
      // ...
      Faw::FigPainter fp;
      dw->render(fp);
      fp.save("export.eps","eps");
    @endcode
    Note that only the DrawingWidget knows which clients are connected to its
    <tt>repaint</tt> signal.
   */
  void render(Faw::Painter &painter);

  /**
    Change the view port by zooming and scrolling in order to be
    the smallest rectangle enclosing the given area.
   */
  void fit_view_port(double left, double right, double bottom, double top, const double unit);

  /**
    Change the view port by zooming and scrolling in order to be
    the smallest rectangle enclosing the given area.
   */
  void fit_view_port(double left, double right, double bottom, double top) { fit_view_port(left, right, bottom, top, _length_unit); }

  /**
    Change the viewport by scrolling in order to show the given world point
    in the center of the widget.
   */
  void center_on(double x, double y, const double unit);

  /**
    Change the viewport by scrolling in order to show the given world point
    in the center of the widget.
   */
  void center_on(double x, double y) { center_on(x, y, _length_unit); }

  /**
    Hide the indicated drawing layer
   */
  void hide_layer(unsigned int layer) { _hidden_layers.insert(layer); }
  
  /**
    Show the indicated drawing layer.
   */
  void show_layer(unsigned int layer) { _hidden_layers.erase(layer); }

  /**
    Indicates wether a grid is to be drawn in the background.
   */
  void set_draw_grid(bool b) { if(_draw_grid!=b) { _draw_grid = b; _drawing_area->update(); } }

  /**
    Set the resolution of the background grid.
   */
  void set_grid_size(int pixels) { _grid_pixels = abs(pixels); _drawing_area->update(); }

  /**
    Toggle double buffering.
   */
  void set_double_buffering(bool b);

signals:

  /**
    This signal is emitted before \c redraw(Painter&) whenever the widget has to be 
    repainted.
    Use this to paint additional background (on top of the grid).
   */
  void pre_redraw(Faw::Painter &painter);

  /**
    This signal is emitted if the widget has to be repainted.
    There should be an appropriate observer of this signal.
   */
  void redraw(Faw::Painter &painter);

  /**
    This signal is emitted after \c redraw(Painter&) has been emitted.
    Use this to draw something on top of the regular drawing, for example a rubberband.
   */
  void post_redraw(Faw::Painter &painter);

  /**
    Reports a mouse moved event.
    Coordinates x and y are in  units.
   */
  void mouse_moved(double x, double y, ButtonState bs);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void mouse_button_pressed(double x, double y, ButtonState bs);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void left_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void middle_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void right_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void shift_left_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void shift_middle_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void shift_right_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void ctrl_left_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void ctrl_middle_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void ctrl_right_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void alt_left_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void alt_middle_click(double x, double y);

  /**
    Reports a mouse button pressed event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void alt_right_click(double x, double y);

  /**
    Reports a mouse button released event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void mouse_button_released(double x, double y, ButtonState bs);

  /**
    Reports a mouse double click event.
    Coordinates x and y are in the drawing widget's length units.
   */
  void mouse_double_clicked(double x, double y, ButtonState bs);

  /**
    Reports a mouse click event generically.
    Coordinates x and y are in the drawing widget's length units.
   */
  void mouse_clicked(double x, double y, ButtonState bs);

private:

  //
  // Types
  //

  class _MyDrawingArea: public QWidget
  {
  public:
    _MyDrawingArea(DrawingWidget *parent) : QWidget(parent), _parent(parent) { setMouseTracking(true); }
  protected:
    void paintEvent(QPaintEvent *pe) { _parent->_paint_event(pe); }
    void mousePressEvent(QMouseEvent *me) { _parent->_mouse_press_event(me); }
    void mouseReleaseEvent(QMouseEvent *me) { _parent->_mouse_release_event(me); }
    void mouseDoubleClickEvent(QMouseEvent *me) { _parent->_mouse_double_click_event(me); }
    void mouseMoveEvent(QMouseEvent *me) { _parent->_mouse_move_event(me); }
    void resizeEvent(QResizeEvent *re) { _parent->_resize_event(re); }
  private:
    DrawingWidget *_parent;
  };
  friend class _MyDrawingArea;

  enum MouseMode
  {
    MOUSE_MODE_NORMAL,
    MOUSE_MODE_ZOOMING,
    MOUSE_MODE_PANNING
  };

  class _AutoPaintableInterface
  {
  public:
    virtual ~_AutoPaintableInterface() {}
    virtual void paint(Painter &painter) = 0;
  };

  template<class Container, class PaintMethod>
  class _AutoPaintableContainer: public _AutoPaintableInterface
  {
  public:
    _AutoPaintableContainer(Container *c, const PaintMethod &pm = PaintMethod()) : _c(c), _pm(pm) {}
    virtual void paint(Painter &painter) { painter.draw(_c->begin(), _c->end(), _pm); }
  private:
    Container *_c;
    PaintMethod _pm;
  };

  template<class Object, class PaintMethod>
  class _AutoPaintableObject: public _AutoPaintableInterface
  {
  public:
    _AutoPaintableObject(Object *o, const PaintMethod &pm = PaintMethod()) : _o(o), _pm(pm) {}
    virtual void paint(Painter &painter) { painter.draw(*_o, _pm); }
  private:
    Object *_o;
    PaintMethod _pm;
  };

  //
  // Methods
  //

  void _paint_event(QPaintEvent *pe);
  void _mouse_press_event(QMouseEvent *me);
  void _mouse_release_event(QMouseEvent *me);
  void _mouse_double_click_event(QMouseEvent *me);
  void _mouse_move_event(QMouseEvent *me);
  void _resize_event(QResizeEvent *re);

  /** Used to round grid to nice numbers.
   */
  double _tick_round();

  /**
    Transform window coordinates (pixels) into model coordinates (millimeters).
   */
  inline void _win2model(int win_x, int win_y, double &model_x, double &model_y) const;

  /**
    Transform window coordinates (pixels) into model coordinates (millimeters).
   */
  inline void _win2model(QPoint win_pos, double &model_x, double &model_y) const { _win2model(win_pos.x(), win_pos.y(), model_x, model_y); }

  /**
    Transform model coordinates (millimeters) into indow coordinates (pixels).
   */
  inline void _model2win(double model_x, double model_y, int &win_x, int &win_y) const;

  /**
    Transform model coordinates (millimeters) into window coordinates (pixels).
   */
  inline QPoint _model2win(double model_x, double model_y) const { int x,y; _model2win(model_x, model_y, x, y); return QPoint(x,y); }

  //
  // Attributes
  //

  _MyDrawingArea *_drawing_area;
  QStatusBar *_status_bar;

  DrawingNode *_scene_root;

  std::map<void*, _AutoPaintableInterface*> _autopainted_things;

  const int _default_cursor;
  const int _zooming_cursor;
  const int _panning_cursor;

  bool _double_buffering;
  QPixmap _drawing_buffer;

  std::set<unsigned int> _hidden_layers;

  /**
    Draw a grid in the background.
   */
  bool _draw_grid;

  /**
    Minimum grid line distance in pixels.
   */
  int _grid_pixels;

  /**
    The length unit that is used as default length unit for any Painter associated with this
    widget and as default length unit for slots and signals of this widget (viewport setting slot,
    mouse event signals and so on).
    Note that clients to the Painter may change the currently length unit as they wish, 
    and the length unit to be used with slots may be set at each call to a slot.
    In order not to change the meaning of signal parameters during the lifecycle of this widget,
    this parameter is declared const and must be set as argument to the constructor on creation
    of this widget.
   */
  const double _length_unit;

  /**
    The inverse of the _length_unit.
    This is computed on creation of the widget and avoids subsequent divisions.
   */
  const double _inv_length_unit;

  bool _first_resize;
  bool _fit_viewport_on_resize;
  double _fit_viewport_on_resize_left;
  double _fit_viewport_on_resize_right;
  double _fit_viewport_on_resize_bottom;
  double _fit_viewport_on_resize_top;
  double _fit_viewport_on_resize_unit;

  double _zoom;
  int _delta_x;
  int _delta_y;

  MouseMode _mouse_mode;
  int _mouse_action_start_x;
  int _mouse_action_start_y;
  double _original_zoom;
  int _original_delta_x;
  int _original_delta_y;
};

inline void DrawingWidget::_win2model(int win_x, int win_y, double &model_x, double &model_y) const
{
  model_x =  (win_x - _delta_x) / _zoom;
  model_y =  (_delta_y - win_y) / _zoom;
}

inline void DrawingWidget::_model2win(double model_x, double model_y, int &win_x, int &win_y) const
{
  win_x = (int)rint( _zoom*model_x + _delta_x);
  win_y = (int)rint(-_zoom*model_y + _delta_y);
}

} // namespace Faw

#endif
