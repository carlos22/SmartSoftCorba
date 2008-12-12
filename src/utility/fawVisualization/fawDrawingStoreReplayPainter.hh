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
 
#ifndef FAW_DRAWING_STORE_REPLAY_PAINTER_HH
#define FAW_DRAWING_STORE_REPLAY_PAINTER_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "fawPainter.hh"

namespace Faw {

/**
  An implementation of the Painter interface which stores drawing commands
  and replays them on other Painter instances.
 */
class DrawingStoreReplayPainter: public Painter
{
public:
  DrawingStoreReplayPainter(const std::string &label = "");
  virtual ~DrawingStoreReplayPainter();

  DrawingStoreReplayPainter(const DrawingStoreReplayPainter &srp);
  DrawingStoreReplayPainter &operator=(const DrawingStoreReplayPainter &srp);

  inline void set_label(const std::string &label) { _label = label; }
  inline const std::string &get_label() const { return _label; }

  /**
    Write the drawing commands to the given output stream.
   */
  void save(std::ostream &os) const;

  /**
    Save the drawing commands to the given string.
   */
  void save(std::string &s) const;
  
  /**
    Read the drawing commands from the given input stream.
   */
  void load(std::istream &is);

  /**
    Read the drawing commands from the given string.
   */
  void load(const std::string &s);

  /**
    Apply the stored drawing commands to the given painter.
   */
  void replay(Painter &painter) const;

  /**
    Read drawing commands from the input stream \a is and apply them to the \a painter.
   */
  static void replay(std::istream &is, Painter &painter);

  /**
    Read drawing commands from the string \a s and apply them to the \a painter.
   */
  static void replay(const std::string &s, Painter &painter);

  /**
    Clear the drawing command storage.
   */
  void clear();

  void print(std::ostream &os) const;

  static inline std::string identifier() { return "Faw::DrawingStoreReplayPainter"; }

protected:

  //
  // Drawing
  //

  virtual void _draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points);
  virtual void _draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points);
  virtual void _draw_cross(double x, double y);
  virtual void _draw_pixel(double x, double y);
  virtual void _draw_node(double x, double y);
  virtual void _draw_segment(double x1, double y1, double x2, double y2);
  virtual void _draw_line(double x1, double y1, double x2, double y2);
  virtual void _draw_ray(double x1, double y1, double x2, double y2);
  virtual void _draw_ellipse(double x, double y, double a, double b, double alpha);
  virtual void _draw_circle(double x, double y, double r);
  virtual void _draw_arc(double x, double y, double radius, double begin, double end);
  virtual void _draw_arrow(double x1, double y1, double x2, double y2);
  virtual void _draw_text(double x, double y, const char*);
  virtual void _draw_text(double x, double y, const char*, char orientation, char h_align, char v_align);
  virtual void _style_changed();
  virtual void _layer_changed();

private:

  class DrawingOperation
  {
  public: 
    virtual ~DrawingOperation() {}
    virtual void replay(Painter &painter) const = 0;
    virtual void save(std::ostream &os) const = 0;
    virtual void load(std::istream &is) = 0;
    virtual void print(std::ostream &os) const = 0;
    virtual DrawingOperation *clone() const = 0;
    virtual int id() const = 0;
    enum
    {
      POLYGON_ID, CHAIN_ID, POINT_ID, SEGMENT_ID, ELLIPSE_ID, CIRCLE_ID, ARC_ID, TEXT_ID, STYLE_ID, LAYER_ID
    };
  };

  class DrawPolygon: public DrawingOperation
  {
  public:
    DrawPolygon() {}
    DrawPolygon(const std::vector<Painter::Point> &points, unsigned int num_points)
      : _points(points.begin(), points.begin() + num_points) {}
    virtual void replay(Painter &painter) const { painter.draw_polygon(_points); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const;
    virtual DrawingOperation *clone() const { return new DrawPolygon(*this); }
    virtual int id() const { return POLYGON_ID; }
  private:
    std::vector<Painter::Point> _points;
  };

  class DrawChain: public DrawingOperation
  {
  public:
    DrawChain() {}
    DrawChain(const std::vector<Painter::Point> &points, unsigned int num_points)
      : _points(points.begin(), points.begin() + num_points) {}
    virtual void replay(Painter &painter) const { painter.draw_chain(_points); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const;
    virtual DrawingOperation *clone() const { return new DrawChain(*this); }
    virtual int id() const { return CHAIN_ID; }
  private:
    std::vector<Painter::Point> _points;
  };

  class DrawPoint: public DrawingOperation
  {
  public:
    DrawPoint() {}
    DrawPoint(double x, double y, DrawingStyle::PointStyle style)
      : _x(x), _y(y), _point_style(style) {}
    virtual void replay(Painter &painter) const { painter.draw_point(_x, _y, _point_style); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "point("<< _x << "," << _y << ")"; }
    virtual DrawingOperation *clone() const { return new DrawPoint(*this); }
    virtual int id() const { return POINT_ID; }
  private:
    double _x, _y;
    DrawingStyle::PointStyle _point_style;
  };

  class DrawSegment: public DrawingOperation
  {
  public:
    enum Style { SEGMENT_STYLE, RAY_STYLE, LINE_STYLE, ARROW_STYLE };
    DrawSegment() {}
    DrawSegment(double x1, double y1, double x2, double y2, DrawSegment::Style style)
      : _x1(x1), _y1(y1), _x2(x2), _y2(y2), _style(style) {}
    virtual void replay(Painter &painter) const;
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "segment(" << _x1 << "," << _y1 << "," << _x2 << "," << _y2 << ", style=" << _style << ")"; }
    virtual DrawingOperation *clone() const { return new DrawSegment(*this); }
    virtual int id() const { return SEGMENT_ID; }
  private:
    double _x1, _y1, _x2, _y2;
    Style _style;
  };

  class DrawEllipse: public DrawingOperation
  {
  public:
    DrawEllipse() {}
    DrawEllipse(double x, double y, double a, double b, double alpha)
      : _x(x), _y(y), _a(a), _b(b), _alpha(alpha) {}
    virtual void replay(Painter &painter) const { painter.draw_ellipse(_x, _y, _a, _b, _alpha); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "ellipse(" << _x << "," << _y << "," << _a << "," << _b << "," << _alpha << ")"; }
    virtual DrawingOperation *clone() const { return new DrawEllipse(*this); }
    virtual int id() const { return ELLIPSE_ID; }
  private:
    double _x, _y, _a, _b, _alpha;
  };

  class DrawCircle: public DrawingOperation
  {
  public:
    DrawCircle() {}
    DrawCircle(double x, double y, double r)
      : _x(x), _y(y), _r(r) {}
    virtual void replay(Painter &painter) const { painter.draw_circle(_x, _y, _r); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "circle(" << _x << "," << _y << "," << _r << ")"; }
    virtual DrawingOperation *clone() const { return new DrawCircle(*this); }
    virtual int id() const { return CIRCLE_ID; }
  private:
    double _x, _y, _r;
  };

  class DrawArc: public DrawingOperation
  {
  public:
    DrawArc() {}
    DrawArc(double x, double y, double radius, double begin, double end)
      : _x(x), _y(y), _radius(radius), _begin(begin), _end(end) {}
    virtual void replay(Painter &painter) const { painter.draw_arc(_x, _y, _radius, _begin, _end); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "arc(" << _x << "," << _y << "," << _radius << "," << _begin << "," << _end  << ")"; }
    virtual DrawingOperation *clone() const { return new DrawArc(*this); }
    virtual int id() const { return ARC_ID; }
  private:
    double _x, _y, _radius, _begin, _end;
  };


  class DrawText: public DrawingOperation
  {
  public:
    DrawText() {}
    DrawText(double x, double y, const std::string &txt)
      : _x(x), _y(y), _orientation('h'), _h_align('l'), _v_align('b'), _txt(txt) {}
    DrawText(double x, double y, const std::string &txt, char orientation, char h_align, char v_align)
      : _x(x), _y(y), _orientation(orientation), _h_align(h_align), _v_align(v_align), _txt(txt) {}
    virtual void replay(Painter &painter) const { painter.draw_text(_x, _y, _txt.c_str(), _orientation, _h_align, _v_align); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "text(" << _x << "," << _y << ",\"" << _txt << "\")"; }
    virtual DrawingOperation *clone() const { return new DrawText(*this); }
    virtual int id() const { return TEXT_ID; }
  private:
    double _x, _y;
    char _orientation;
    char _h_align, _v_align;
    std::string _txt;
  };

  class DrawingStyleChange: public DrawingOperation
  {
  public:
    DrawingStyleChange() {}
    DrawingStyleChange(const DrawingStyle &style)
      : _style(style) {}
    virtual void replay(Painter &painter) const { painter.set_style(_style); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "style(" << _style << ")"; }
    virtual DrawingOperation *clone() const { return new DrawingStyleChange(*this); }
    virtual int id() const { return STYLE_ID; }
  private:
    DrawingStyle _style;
  };

  class DrawingLayerChange: public DrawingOperation
  {
  public:
    DrawingLayerChange() {}
    DrawingLayerChange(unsigned int layer)
      : _layer(layer) {}
    virtual void replay(Painter &painter) const { painter.set_layer(_layer); }
    virtual void save(std::ostream &os) const;
    virtual void load(std::istream &is);
    virtual void print(std::ostream &os) const { os << "layer(" << _layer << ")"; }
    virtual DrawingOperation *clone() const { return new DrawingLayerChange(*this); }
    virtual int id() const { return LAYER_ID; }
  private:
    unsigned int _layer;
  };

  static std::map<int, DrawingOperation*> _prototypes;
  static bool _prototypes_initialized;
  static void _initialize_prototypes();

  std::vector<DrawingOperation*> _the_drawing;

  std::string _label;
};

inline std::ostream &operator<<(std::ostream &os, const DrawingStoreReplayPainter &srp)
{
  srp.print(os); return os;
}

} // namespace Faw

#endif
