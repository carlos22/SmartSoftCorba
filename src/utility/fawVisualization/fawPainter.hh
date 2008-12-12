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
 
#ifndef FAW_PAINTER_H
#define FAW_PAINTER_H

#include <cmath>
#include <vector>
#include <iterator>
#include <utility>

#include "fawDrawingStyle.hh"
#include "fawDrawingTransformation.hh"

namespace Faw {

class Painter;

template<class Obj>
struct DefaultPaintMethod
{
  inline void operator()(const Obj &obj, Painter &painter, bool do_highlight) const
  {
    obj.paint(painter);
  }
  inline void operator()(const Obj &obj, Painter &painter) const
  {
    obj.paint(painter);
  }
};



/**
  A painter abstraction that generalizes graphical output to windows, to FIG-files etc.
  Drawing should be wrapped by calls to \c begin() and \c end() methods.
  \author Boris Kluge
  \date 25.06.2002
 */
class Painter
{
public:
  Painter();
  virtual ~Painter() {}

  /**
    Save the current context (pen width, colors, drawing style) onto a
    context stack.
    If you enclose your drawing operations between <tt>begin()</tt> and <tt>end()</tt>,
    you can safely change any drawing style parameters without affecting any subsequent
    drawing to the same painter.
   */
  void begin();

  /**
    Save the current context (pen width, colors, drawing style) onto a
    context stack and change the current drawing layer.
   */
  inline void begin(unsigned int layer);

  /**
    Replace the current context by the top of the context stack, and pop the stack.
    \sa begin()
   */
  void end();

  //
  // Generic methods for drawing arbitrary objects
  //

  /**
    Draw a generic object \a t of class T using the \a PaintMethod \a paint_method.
   */
  template<class T, class PaintMethod>
  inline void draw(const T &t, const PaintMethod &paint_method);

  /**
    Draw an object \a t with the default paint method <tt>DefaultPaintMethod<T></tt> for class T.
    It is a good idea to provide specialization of the DefaultPaintMethod template 
    for own drawable classes.
   */
  template<class T>
  inline void draw(const T &t);

  /**
    Draw the objects referenced by the iterators \a begin through \a end using the
    indicated \a paint_method.
   */
  template<class Iterator, class PaintMethod>
  inline void draw(Iterator begin, Iterator end, const PaintMethod &paint_method = PaintMethod());

  /**
    Draw the objects referenced by the iterators \a begin through \a end using the
    default paint method of their class.
   */
  template<class Iterator>
  inline void draw(Iterator begin, Iterator end);

  //
  // Polygons
  //

  /**
    Draw a closed polygon with the given coordinates.
   */
  template<class Iterator>
  inline void draw_polygon(Iterator x_begin, Iterator x_end, Iterator y_begin);

  /**
    Draw a closed polygon with the given point sequence.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_polygon(Iterator begin, Iterator end);

  /**
    Draw a closed polygon with the given point sequence.
    The sequence is expected to have <tt>begin()</tt> and <tt>end()</tt> methods for accessing the
    respective iterators.
   */
  template<class Sequence>
  inline void draw_polygon(const Sequence &seq);

  //
  // Polygonal chains
  //

  /**
    Draw an open polygonal chain with the given coordinates.
   */
  template<class Iterator>
  inline void draw_chain(Iterator x_begin, Iterator x_end, Iterator y_begin);

  /**
    Draw an open polygonal chain with the given point sequence.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_chain(Iterator begin, Iterator end);

  /**
    Draw an open polygonal chain with the given point sequence.
    The sequence is expected to have <tt>begin()</tt> and <tt>end()</tt> methods for accessing the
    respective iterators.
   */
  template<class Sequence>
  inline void draw_chain(const Sequence &seq);

  //
  // Points
  //

  /**
    Draw a single point at (x,y) with the given style.
   */
  inline void draw_point(double x, double y, DrawingStyle::PointStyle style);

  /**
    Draw a set of points at positions given by their coordiates using the indicated style.
   */
  template<class Iterator>
  inline void draw_points(Iterator x_begin, Iterator x_end, Iterator y_begin, DrawingStyle::PointStyle style);

  /**
    Draw the given sequence of points with the indicated style.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_points(Iterator begin, Iterator end, DrawingStyle::PointStyle style);

  /**
    Draw the given sequence of points with the indicated style.
   */
  template<class Sequence>
  inline void draw_points(const Sequence &seq, DrawingStyle::PointStyle style);

  //
  // X-Crosses
  //

  /**
    Draw a single cross at (x,y).
   */
  inline void draw_cross(double x, double y);

  /**
    Draw a set of crosses at positions given by their coordinates.
   */
  template<class Iterator>
  inline void draw_crosses(Iterator x_begin, Iterator x_end, Iterator y_begin);

  /**
    Draw a sequence of crosses.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_crosses(Iterator begin, Iterator end);

  /**
    Draw a sequence of crosses.
    The sequence is expected to have <tt>begin()</tt> and <tt>end()</tt> methods for accessing the
    respective iterators.
   */
  template<class Sequence>
  inline void draw_crosses(const Sequence &seq);

  //
  // Pixels
  //

  /**
    Draw a single pixel at (x,y).
   */
  inline void draw_pixel(double x, double y);

  /**
    Draw a set of pixels at positions given by their coordinates.
   */
  template<class Iterator>
  inline void draw_pixels(Iterator x_begin, Iterator x_end, Iterator y_begin);

  /**
    Draw a sequence of pixels.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_pixels(Iterator begin, Iterator end);

  /**
    Draw a sequence of pixels.
    The sequence is expected to have <tt>begin()</tt> and <tt>end()</tt> methods for accessing the
    respective iterators.
   */
  template<class Sequence>
  inline void draw_pixels(const Sequence &seq);

  //
  // Nodes
  //

  /**
    Draw a single node at (x,y).
   */
  inline void draw_node(double x, double y);

  /**
    Draw a set of nodes at positions given by their coordinates.
   */
  template<class Iterator>
  inline void draw_nodes(Iterator x_begin, Iterator x_end, Iterator y_begin);

  /**
    Draw a sequence of nodes.
    The value type of the iterator is expected to provide the subscript operator [](int)
    for coordinate access, index 0 refering to the x-coordinate and index 1 refering
    to the y-coordinate.
   */
  template<class Iterator>
  inline void draw_nodes(Iterator begin, Iterator end);

  /**
    Draw a sequence of nodes.
    The sequence is expected to have <tt>begin()</tt> and <tt>end()</tt> methods for accessing the
    respective iterators.
   */
  template<class Sequence>
  inline void draw_nodes(const Sequence &seq);

  //
  // Geometric primitives
  //

  /**
    Draw a straight line segment from (x1,y1) to (x2,y2).
   */
  inline void draw_segment(double x1, double y1, double x2, double y2);

  /**
    Draw a straight line segment from point \a p1 to point \a p2.
    The classes of the two point arguments are expected to provide the
    subscript operator.
   */
  template<class P1, class P2>
  inline void draw_segment(const P1 &p1, const P2 &p2) { draw_segment(p1[0],p1[1], p2[0],p2[1]); }

  /**
    Draw an arrow from (x1,y1) to (x2,y2).
   */
  inline void draw_arrow(double x1, double y1, double x2, double y2);

  /**
    Draw an arrow from p1 to p2.
   */
  template<class P1, class P2>
  inline void draw_arrow(const P1 &p1, const P2 &p2) { draw_arrow(p1[0],p1[1], p2[0],p2[1]); }

  /**
    Draw a straight line containing points (x1,y1) and (x2,y2).
   */
  inline void draw_line(double x1, double y1, double x2, double y2);

  /**
    Draw a straight line passing through (x,y) with direction \a alpha.
    The positive x-axis is direction zero, the positive y-axis is direction pi/2 etc.
   */
  inline void draw_line(double x, double y, double alpha) { draw_line(x,y,x+cos(alpha),y+sin(alpha)); }

  /**
    Draw a straight line containing the points \a p1 and \a p2.
   */
  template<class P1, class P2>
  inline void draw_line(const P1 &p1, const P2 &p2) { draw_line(p1[0],p1[1], p2[0],p2[1]); }

  /**
    Draw a straight ray starting at point (x1,y1) and passing through (x2,y2).
   */
  inline void draw_ray(double x1, double y1, double x2, double y2);

  /**
    Draw a straight ray starting at (x,y) with direction \a alpha.
    The positive x-axis is direction zero, the positive y-axis is direction pi/2 etc.
   */
  inline void draw_ray(double x, double y, double alpha) { draw_ray(x,y,x+cos(alpha),y+sin(alpha)); }

  /**
    Draw a straight ray starting at point \a p1 and passing through point \a p2.
   */
  template<class P1, class P2>
  inline void draw_ray(const P1 &p1, const P2 &p2) { draw_ray(p1[0],p1[1], p2[0],p2[1]); }

  /**
    Draws a rotated ellipse.
    The centre of the ellipse and of the rotation is \c (x,y).
    The unrotated axes of the ellipse are parallel to the coordinate axes, and the 
    lengths of its (unrotated) half-axes are \c a and \c b in x- and y-direction, respectively.
    The ellipse is rotated in counterclockwise orientation by angle \a alpha around its center.
    The line of the ellipse is drawn with the given \c style.
   */
  inline void draw_ellipse(double x, double y, double a, double b, double alpha = 0);

  /**
    Draw a circle centered at (x,y) with radius \a radius.
   */
  inline void draw_circle(double x, double y, double radius);

  /**
    Draw a box (the rectangular polygon ((left,bottom),(right,bottom),(right,top),(left,top))).
   */
  inline void draw_box(double left, double bottom, double right, double top);

  /**
    Draws a circular arc.
    The center of the circle is \c (x,y), its radius is \c radius.
    The arc starts at angle \c begin and ends at angle \c end.
    The 3 o'clock position (direction of the x-axis) is the zero angle.
   */
  inline void draw_arc(double x, double y, double radius, double begin, double end);

  /**
    Draw text at position (x,y).
   */
  inline void draw_text(double x, double y, const char*);

  /**
    Draw text at position (x,y) with the given alignment.
    Possible values for \a h_align are 
      'l' (left end of text at horizontal position \a x), 
      'c' (center of text at \a x), and
      'r' (right end of text at \a x).
    Possible values for \a v_align are 
      't' (top of text at height \a y), 
      'c' (center of text at height \a y), and
      'b' (bottom of text at height \a y).
   */
  inline void draw_text(double x, double y, const char*, char orientation, char h_align, char v_align);

  //
  // Style & Transformation
  //

  /**
    Set the length unit for subsequent drawing operations.
    The default length unit are meters.
   */
  inline void set_length_unit(const double unit);

  /**
    Change the current drawing layer.
    Some output device may distinguish different layers, others won't.
   */
  inline void set_layer(unsigned int layer);
  
  /**
    Return the current drawing layer.
   */
  inline unsigned int get_layer() const { return _layer; }

  /**
    Set the pen width which will be used for line drawings.
   */
  inline void set_pen_width(int w);
  
  /** 
    Set the pen color which will be used for line drawings.
   */
  inline void set_pen_color(int r, int g, int b);

  /** 
    Set the pen color which will be used for line drawings.
   */
  inline void set_pen_color(DrawingStyle::Color c);

  /** 
    Set the line drawing style.
   */
  inline void set_line_style(DrawingStyle::LineStyle s);

  /** 
    Set the color which will be used to fill closed objects (circles, polygons, ellipses).
   */
  inline void set_fill_color(int r, int g, int b);

  /** 
    Set the color which will be used to fill closed objects (circles, polygons, ellipses).
   */
  inline void set_fill_color(DrawingStyle::Color c);

  /** 
    Reset filling to disabled.
   */
  inline void no_filling();

  /**
    Return the current pen width for line drawing.
   */
  inline int get_pen_width() const;
  
  /**
    Return the current pen color for line drawing.
   */
  inline DrawingStyle::Color get_pen_color() const;
  
  /**
    Return the current line drawing style.
   */
  inline DrawingStyle::LineStyle get_line_style() const;

  /**
    Return the current fill color.
   */
  inline DrawingStyle::Color get_fill_color() const;

  /**
    Check if filling is currently active.
   */
  inline bool get_filling() const;

  /**
    Set the complete drawing style.
   */
  inline void set_style(const DrawingStyle &style);

  /**
    Get the complete drawing style.
   */
  inline const DrawingStyle &get_style() const;

  /**
    Reset the drawing style to its default value.
   */
  inline void reset_style();

  /**
    Change drawing coordinate system by moving its origin to the indicated pose.
   */
  inline void set_transformation(double dx, double dy, double alpha = 0);

  /**
    Change drawing coordinate system by moving its origin to the indicated pose
    in the given units.
   */
  inline void set_transformation(double dx, double dy, double alpha, const double unit);

  /**
    Change drawing coordinate system by moving its origin to the indicated pose.
   */
  inline void set_transformation(const DrawingTransformation &xform);

  /**
    Return the current drawing coordinate system which is the product of a sequence of
    relative changes.
   */
  inline const DrawingTransformation &get_accumulated_transformation() const;

  /**
    Point class which is internally used.
    You can use your own point class with this painter if it provides
    the subscript operator for coordinate access.
   */
  struct Point
  {
    Point() : x(0), y(0) {}
    Point(double xx, double yy) : x(xx), y(yy) {}

    /**
      Subscript operator for coordinate access.
      Index 0 refers to the x-coordinate, index 1 refers to the y-coordinate.
     */
    inline const double &operator[](int i) const { return ((const double*)this)[i%2]; }

    /**
      Subscript operator for coordinate access.
      Index 0 refers to the x-coordinate, index 1 refers to the y-coordinate.
     */
    inline double &operator[](int i) { return ((double*)this)[i%2]; }
    double x,y;
  };
  
protected:

  virtual void _begin() {}
  virtual void _end() {}
  
  virtual void _draw_polygon(const std::vector<Point> &points, unsigned int num_points);
  virtual void _draw_chain(const std::vector<Point> &points, unsigned int num_points);
  virtual void _draw_cross(double x, double y) = 0;
  virtual void _draw_pixel(double x, double y) = 0;
  virtual void _draw_node(double x, double y) = 0;
  virtual void _draw_segment(double x1, double y1, double x2, double y2) = 0;
  virtual void _draw_line(double x1, double y1, double x2, double y2);
  virtual void _draw_ray(double x1, double y1, double x2, double y2);
  virtual void _draw_ellipse(double x, double y, double a, double b, double alpha) = 0;
  virtual void _draw_circle(double x, double y, double r);
  virtual void _draw_arc(double x, double y, double radius, double begin, double end) = 0;
  virtual void _draw_arrow(double x1, double y1, double x2, double y2) = 0;
  virtual void _draw_text(double x, double y, const char*) = 0;
  virtual void _draw_text(double x, double y, const char*, char orientation, char h_align, char v_align) = 0;

  virtual void _style_changed() {}
  virtual void _layer_changed() {}

private:


  struct PainterState
  {
    PainterState() : unit(1.0), layer(0) {}
    PainterState(double u, unsigned int l, const DrawingStyle &s, const DrawingTransformation &t) : unit(u), layer(l), style(s), base_xform(t) {}
    double unit;
    unsigned int layer; // "layer" in Painter means the same as "depth" in xfig
    DrawingStyle style;
    DrawingTransformation base_xform;
  };

  inline double _xform_x(double x, double y) { return _accu_xform.xform_x(x,y); }
  inline double _xform_y(double x, double y) { return _accu_xform.xform_y(x,y); }

  /**
    Stack of Painter states.
   */
  std::vector<PainterState> _painter_state_stack;

  /**
    Current length unit for interpreting drawing operations.
   */
  double _unit;

  /**
    Current layer onto which drawing occurs.
   */
  unsigned int _layer;

  /**
    Current drawing style.
   */
  DrawingStyle _style;
  bool _style_propagated;
  inline void _propagate_style();
  
  /**
    Current base transformation.
   */
  DrawingTransformation _base_xform;

  /**
    Current accumulated transformation, that is the composition of the
    current base transformation and the last set transformation.
   */
  DrawingTransformation _accu_xform;

  /**
    Static (shared and reused) buffer for point data.
    Used to draw polygons and polylines (chains).
   */
  static std::vector<Point> _point_array;
};



//////////////////////////////////////////////////////////////////////
////////// Implementation ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

inline void Painter::begin(unsigned int layer)
{
  this->begin();
  this->set_layer(layer);
}

//
// Arbitrary objects
//

template<class T, class PaintMethod>
inline void Painter::draw(const T &t, const PaintMethod &paint_method)
{
  begin();
  paint_method(t, *this);
  end();
}

template<class T>
inline void Painter::draw(const T &t) 
{
  draw(t, DefaultPaintMethod<T>());
}

template<class Iterator, class PaintMethod>
inline void Painter::draw(Iterator iter, Iterator end, const PaintMethod &paint_method)
{
  while(iter!=end)
  {
    draw(*iter, paint_method);
    ++iter;
  }
}

template<class Iterator>
inline void Painter::draw(Iterator begin, Iterator end) 
{ 
  draw(begin, end, DefaultPaintMethod<typename std::iterator_traits<Iterator>::value_type>()); 
}

//
// Polygons
//

template<class Iterator>
inline void Painter::draw_polygon(Iterator x_iter, Iterator x_end, Iterator y_iter)
{
  const unsigned int count = std::distance(x_iter, x_end);
  if(_point_array.size()<count) _point_array.resize(count);
  for(unsigned int i=0; i<count; i++)
  {
    const double x = *x_iter * _unit;
    const double y = *y_iter * _unit;
    _point_array[i] = Point(_xform_x(x,y), _xform_y(x,y));
    ++x_iter;
    ++y_iter;
  }
  _propagate_style();
  this->_draw_polygon(_point_array, count);
}

template<class Iterator>
inline void Painter::draw_polygon(Iterator iter, Iterator end)
{
  const unsigned int count = std::distance(iter, end);
  if(_point_array.size()<count) _point_array.resize(count);
  for(unsigned int i=0; i<count; i++)
  {
    const double x = (*iter)[0] * _unit;
    const double y = (*iter)[1] * _unit;
    _point_array[i] = Point(_xform_x(x,y), _xform_y(x,y));
    ++iter;
  }
  _propagate_style();
  this->_draw_polygon(_point_array, count);
}


template<class Sequence>
inline void Painter::draw_polygon(const Sequence &seq)
{
  this->draw_polygon(seq.begin(), seq.end());
}


//
// Polygonal chains
//

template<class Iterator>
inline void Painter::draw_chain(Iterator x_iter, Iterator x_end, Iterator y_iter)
{
  const unsigned int count = std::distance(x_iter, x_end);
  if(_point_array.size()<count) _point_array.resize(count);
  for(unsigned int i=0; i<count; i++) 
  {
    const double x = *x_iter * _unit;
    const double y = *y_iter * _unit;
    _point_array[i] = Point(_xform_x(x,y), _xform_y(x,y));
    ++x_iter;
    ++y_iter;
  }
  _propagate_style();
  this->_draw_chain(_point_array, count);
}

template<class Iterator>
inline void Painter::draw_chain(Iterator iter, Iterator end)
{
  const unsigned int count = std::distance(iter, end);
  if(_point_array.size()<count) _point_array.resize(count);
  for(unsigned int i=0; i<count; i++) 
  {
    const double x = (*iter)[0] * _unit;
    const double y = (*iter)[1] * _unit;
    _point_array[i] = Point(_xform_x(x,y), _xform_y(x,y));
    ++iter;
  }
  _propagate_style();
  this->_draw_chain(_point_array, count);
}

template<class Sequence>
inline void Painter::draw_chain(const Sequence &seq)
{
  this->draw_chain(seq.begin(), seq.end());
}


//
// Points
//

inline void Painter::draw_point(double x, double y, DrawingStyle::PointStyle style)
{
  switch(style)
  {
    case DrawingStyle::POINT: draw_pixel(x,y); return;
    case DrawingStyle::CROSS: draw_cross(x,y); return;
    case DrawingStyle::NODE:  draw_node(x,y);  return;
    default: return;
  }
}

template<class Iterator>
inline void Painter::draw_points(Iterator x_iter, Iterator x_end, Iterator y_iter, DrawingStyle::PointStyle style)
{
  switch(style)
  {
    case DrawingStyle::POINT: draw_pixels(x_iter,x_end,y_iter); return;
    case DrawingStyle::CROSS: draw_crosses(x_iter,x_end,y_iter); return;
    case DrawingStyle::NODE:  draw_nodes(x_iter,x_end,y_iter);  return;
    default: return;
  }
}

template<class Iterator>
inline void Painter::draw_points(Iterator iter, Iterator end, DrawingStyle::PointStyle style)
{
  switch(style)
  {
    case DrawingStyle::POINT: draw_pixels(iter,end); return;
    case DrawingStyle::CROSS: draw_crosses(iter,end); return;
    case DrawingStyle::NODE:  draw_nodes(iter,end);  return;
    default: return;
  }
}

template<class Sequence>
inline void Painter::draw_points(const Sequence &seq, DrawingStyle::PointStyle style)
{
  this->draw_points(seq.begin(), seq.end(), style);
}


//
// X-Crosses
//

inline void Painter::draw_cross(double x, double y)
{ 
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  begin();
  set_style(s);
  _propagate_style();
  x *= _unit;
  y *= _unit;
  _draw_cross(_xform_x(x,y), _xform_y(x,y)); 
  end();
}

template<class Iterator>
inline void Painter::draw_crosses(Iterator x_iter, Iterator x_end, Iterator y_iter)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  begin();
  set_style(s);
  _propagate_style();
  while(x_iter!=x_end)
  {
    const double x = *x_iter * _unit;
    const double y = *y_iter * _unit;
    _draw_cross(_xform_x(x,y), _xform_y(x,y));
    ++x_iter;
    ++y_iter;
  }
  end();
}

template<class Iterator>
inline void Painter::draw_crosses(Iterator iter, Iterator end)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  this->begin();
  set_style(s);
  _propagate_style();
  while(iter!=end)
  {
    const double x = (*iter)[0] * _unit;
    const double y = (*iter)[1] * _unit;
    _draw_cross(_xform_x(x,y), _xform_y(x,y));
    ++iter;
  }
  this->end();
}

template<class Sequence>
inline void Painter::draw_crosses(const Sequence &seq)
{
  this->draw_crosses(seq.begin(), seq.end());
}


//
// Pixels
//

inline void Painter::draw_pixel(double x, double y)
{ 
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  begin();
  set_style(s);
  _propagate_style();
  x *= _unit;
  y *= _unit;
  _draw_pixel(_xform_x(x,y), _xform_y(x,y)); 
  end();
}

template<class Iterator>
inline void Painter::draw_pixels(Iterator x_iter, Iterator x_end, Iterator y_iter)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  begin();
  set_style(s);
  _propagate_style();
  while(x_iter!=x_end)
  {
    const double x = *x_iter * _unit;
    const double y = *y_iter * _unit;
    _draw_pixel(_xform_x(x,y), _xform_y(x,y));
    ++x_iter;
    ++y_iter;
  }
  end();
}

template<class Iterator>
inline void Painter::draw_pixels(Iterator iter, Iterator end)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  this->begin();
  set_style(s);
  _propagate_style();
  while(iter!=end)
  {
    const double x = (*iter)[0] * _unit;
    const double y = (*iter)[1] * _unit;
    _draw_pixel(_xform_x(x,y), _xform_y(x,y));
    ++iter;
  }
  this->end();
}

template<class Sequence>
inline void Painter::draw_pixels(const Sequence &seq)
{
  this->draw_pixels(seq.begin(), seq.end());
}


//
// Nodes
//

inline void Painter::draw_node(double x, double y)
{ 
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  s.set_fill(true);
  s.set_fill_color(s.get_pen_color());
  begin();
  set_style(s);
  _propagate_style();
  x *= _unit;
  y *= _unit;
  _draw_node(_xform_x(x,y), _xform_y(x,y)); 
  end();
}

template<class Iterator>
inline void Painter::draw_nodes(Iterator x_iter, Iterator x_end, Iterator y_iter)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  s.set_fill(true);
  s.set_fill_color(s.get_pen_color());
  begin();
  set_style(s);
  _propagate_style();
  while(x_iter!=x_end)
  {
    const double x = *x_iter * _unit;
    const double y = *y_iter * _unit;
    _draw_node(_xform_x(x,y), _xform_y(x,y));
    ++x_iter;
    ++y_iter;
  }
  end();
}

template<class Iterator>
inline void Painter::draw_nodes(Iterator iter, Iterator end)
{
  DrawingStyle s = get_style();
  s.set_line_style(DrawingStyle::SOLID);
  s.set_pen_width(1);
  s.set_fill(true);
  s.set_fill_color(s.get_pen_color());
  this->begin();
  set_style(s);
  _propagate_style();
  while(iter!=end)
  {
    const double x = (*iter)[0] * _unit;
    const double y = (*iter)[1] * _unit;
    _draw_node(_xform_x(x,y), _xform_y(x,y));
    ++iter;
  }
  this->end();
}

template<class Sequence>
inline void Painter::draw_nodes(const Sequence &seq)
{
  this->draw_nodes(seq.begin(), seq.end());
}


//
// Other primitives
//

inline void Painter::draw_segment(double x1, double y1, double x2, double y2)
{
  _propagate_style();
  x1 *= _unit; y1 *= _unit; x2 *= _unit; y2 *= _unit;
  _draw_segment(_xform_x(x1,y1), _xform_y(x1,y1), _xform_x(x2,y2), _xform_y(x2,y2));
}

inline void Painter::draw_line(double x1, double y1, double x2, double y2)
{
  _propagate_style();
  x1 *= _unit; y1 *= _unit; x2 *= _unit; y2 *= _unit;
  _draw_line(_xform_x(x1,y1), _xform_y(x1,y1), _xform_x(x2,y2), _xform_y(x2,y2));
}

inline void Painter::draw_ray(double x1, double y1, double x2, double y2)
{
  _propagate_style();
  x1 *= _unit; y1 *= _unit; x2 *= _unit; y2 *= _unit;
  _draw_ray(_xform_x(x1,y1), _xform_y(x1,y1), _xform_x(x2,y2), _xform_y(x2,y2));
}

inline void Painter::draw_ellipse(double x, double y, double a, double b, double alpha)
{
  _propagate_style();
  x *= _unit; y *= _unit; a *= _unit; b *= _unit;
  const double xform_alpha = alpha + get_accumulated_transformation().get_alpha();
  _draw_ellipse(_xform_x(x,y), _xform_y(x,y), a, b, xform_alpha);
}

inline void Painter::draw_circle(double x, double y, double radius)
{
  _propagate_style();
  x *= _unit; y *= _unit; radius *= _unit;
  _draw_circle(_xform_x(x,y), _xform_y(x,y), radius);
}

inline void Painter::draw_box(double left, double bottom, double right, double top)
{
  if(_point_array.size()<4) _point_array.resize(4);

  const double x1 = left   * _unit;
  const double x2 = right  * _unit;
  const double y1 = bottom * _unit;
  const double y2 = top    * _unit;

  if((left - right) * (top-bottom) >= 0)
  {
    _point_array[0] = Point(_xform_x(x1,y1), _xform_y(x1,y1));
    _point_array[1] = Point(_xform_x(x2,y1), _xform_y(x2,y1));
    _point_array[2] = Point(_xform_x(x2,y2), _xform_y(x2,y2));
    _point_array[3] = Point(_xform_x(x1,y2), _xform_y(x1,y2));
  }
  else
  {
    _point_array[3] = Point(_xform_x(x1,y1), _xform_y(x1,y1));
    _point_array[2] = Point(_xform_x(x2,y1), _xform_y(x2,y1));
    _point_array[1] = Point(_xform_x(x2,y2), _xform_y(x2,y2));
    _point_array[0] = Point(_xform_x(x1,y2), _xform_y(x1,y2));
  }

  _propagate_style();
  this->_draw_polygon(_point_array, 4);
}

inline void Painter::draw_arc(double x, double y, double radius, double begin, double end)
{
  _propagate_style();
  x *= _unit; y *= _unit; radius *= _unit;
  const double alpha = get_accumulated_transformation().get_alpha();
  _draw_arc(_xform_x(x,y), _xform_y(x,y), radius, begin + alpha, end + alpha);
}

inline void Painter::draw_arrow(double x1, double y1, double x2, double y2)
{
  _propagate_style();
  x1 *= _unit; y1 *= _unit; x2 *= _unit; y2 *= _unit;
  _draw_arrow(_xform_x(x1,y1), _xform_y(x1,y1), _xform_x(x2,y2), _xform_y(x2,y2));
}

inline void Painter::draw_text(double x, double y, const char *msg)
{
  _propagate_style();
  x *= _unit; y *= _unit;
  _draw_text(_xform_x(x,y), _xform_y(x,y), msg);
}

inline void Painter::draw_text(double x, double y, const char *msg, char orientation, char h_align, char v_align)
{
  _propagate_style();
  x *= _unit; y *= _unit;
  _draw_text(_xform_x(x,y), _xform_y(x,y), msg, orientation, h_align, v_align);
}

//
// Style, transformations, length unit, layers
//

inline void Painter::set_length_unit(const double unit)
{
  _unit = unit;
}

inline void Painter::set_layer(unsigned int l)
{
  _layer = l;
  _layer_changed();
}

inline void Painter::set_pen_width(int w)
{
  DrawingStyle style = get_style();
  style.set_pen_width(w);
  set_style(style);
}

inline void Painter::set_pen_color(int r, int g, int b)
{
  set_pen_color(DrawingStyle::color(r,g,b));
}

inline void Painter::set_pen_color(DrawingStyle::Color c)
{
  _style.set_pen_color(c);
  _style_propagated = false;
}

inline void Painter::set_line_style(DrawingStyle::LineStyle s)
{
  _style.set_line_style(s);
  _style_propagated = false;
}

inline void Painter::set_fill_color(int r, int g, int b)
{
  set_fill_color(DrawingStyle::color(r,g,b));
}

inline void Painter::set_fill_color(DrawingStyle::Color c)
{
  _style.set_fill_color(c);
  _style.set_fill(true);
  _style_propagated = false;
}

inline void Painter::no_filling()
{
  _style.set_fill(false);
  _style_propagated = false;
}

inline int Painter::get_pen_width() const
{
  return _style.get_pen_width();
}
inline DrawingStyle::Color Painter::get_pen_color() const
{
  return _style.get_pen_color();
}
inline DrawingStyle::LineStyle Painter::get_line_style() const
{
  return _style.get_line_style();
}
inline DrawingStyle::Color Painter::get_fill_color() const
{
  return _style.get_fill_color();
}
inline bool Painter::get_filling() const
{
  return _style.get_fill();
}

inline void Painter::set_style(const DrawingStyle &style) 
{ 
  _style = style; 
  _style_propagated = false; 
}

inline const DrawingStyle &Painter::get_style() const 
{ 
  return _style; 
}

inline void Painter::reset_style()
{ 
  _style = DrawingStyle();
  _style_propagated = false;
}

inline void Painter::set_transformation(double dx, double dy, double alpha)
{
  set_transformation(DrawingTransformation(dx,dy,alpha,_unit));
}

inline void Painter::set_transformation(double dx, double dy, double alpha, const double unit)
{
  set_transformation(DrawingTransformation(dx,dy,alpha,unit));
}

inline void Painter::set_transformation(const DrawingTransformation &xform) 
{
//  _accu_xform = xform * _base_xform; 
  _accu_xform = _base_xform * xform;
}

inline const DrawingTransformation &Painter::get_accumulated_transformation() const 
{ 
  return _accu_xform; 
}

inline void Painter::_propagate_style()
{
  if(!_style_propagated)
  {
    _style_changed();
    _style_propagated = true;
  }
}

} // namespace Faw

#endif
