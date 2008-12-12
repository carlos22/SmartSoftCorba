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

#ifndef FAW_FIG_PAINTER_HH
#define FAW_FIG_PAINTER_HH

#include <iostream>
#include <string>
#include <list>

#if (__GNUC__ >= 3)
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#include "fawFigObjects.hh"
#include "fawPainter.hh"

namespace Faw {

/**
  An implementation of the Painter interface with xfig files as drawing target.
  With the help of <tt>fig2dev</tt>, this painter exports drawings to various 
  graphics formats.
 */
class FigPainter: public Painter
{
public:

  /**
    Create a FigPainter with the given scaling.
   */
  FigPainter(double scale = 0.01);

  /* *
    Provide information about the extent of the drawing.
    FIG uses a flipped coordinate system: the x axis points to the right and 
    the y axis points downward.
    FigPainter uses the bounding box information to reflect and shift the 
    drawn objects such that they appear as you expect them when loaded into xfig.
   */
//  void set_bounding_box(const BoundingBox &bbox);

  /* *
    Provide information about used colors.
    FIG requires color pseudo objects to appear before all other objects.
    Thus color information must be provided in advance.
    Dereferencing an iterator is expected to deliver a \c DrawingStyle::Color object.
   */
//  template<class Iterator>
//  void set_color_map(Iterator begin, Iterator end);

  virtual ~FigPainter();

  /**
    Save the drawing in xfig format to the given output stream.
   */
  void save(std::ostream &os);

  /**
    Save the drawing in xfig format to the given output stream, 
    with at least the area of the boundig box \a bbox visible.
    If the drawing fits into \a bbox, exactly the \a bbox area is visible,
    otherwise the visible area is larger.
   */
  void save(std::ostream &os, const BoundingBox &bbox);

  /**
    Save the drawing to an image file.
    The drawing is saved to a file named \a filename in format \a format
    which is one of "box", "cgm", "epic", "eepic", "eepicemu", "eps", "gif",
    "ibmgl", "jpeg", "latex", "mf", "pcx", "pdf", "pic",
    "pictex", "png", "ppm", "ps", "sld", "textyl",  "tiff",
    "tk", "tpic", "xbm", or "xpm".
    This method requires fig2dev to be properly installed on the system.
    The \a options are passed to fig2dev, use them for example for magnified output to
    bitmap graphic formats.
   */
  void save(const std::string &filename, const std::string &format, const std::string &options = "");

  /**
    Save the drawing to an image file,
    with at least the area of the boundig box \a bbox visible.
    If the drawing fits into \a bbox, exactly the \a bbox area is visible,
    otherwise the visible area is larger.
   */
  void save(const std::string &filename, const std::string &format, const BoundingBox &bbox, const std::string &options = "");

  /**
    Save the drawing to an image file.
    Same as above, but the filename is created by \c snprintf(filename,filename_format,counter).
   */
  void save(const std::string &filename_format, int counter, const std::string &file_format, const std::string &options = "");

  /**
    Save the drawing to an image file,
    with at least the area of the boundig box \a bbox visible.
    If the drawing fits into \a bbox, exactly the \a bbox area is visible,
    otherwise the visible area is larger.
   */
  void save(const std::string &filename_format, int counter, const std::string &file_format, const BoundingBox &bbox, const std::string &options = "");

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

private:

  /**
    Small length in meters used for drawing crosses and nodes.
    
   */
  static inline double SMALL_LENGTH() { return 0.01; }

  const double _scale;
  inline int _x_fig(double x) const { return (int)rint( _scale*x); }
  inline int _y_fig(double y) const { return (int)rint(-_scale*y); }
  inline BoundingBox _bbox_fig(const BoundingBox &bbox) const 
  {
    return BoundingBox(_x_fig(bbox.left()), _y_fig(bbox.bottom()), _x_fig(bbox.right()), _y_fig(bbox.top()));
  }

  /**
    Get a bounding box of the drawing in xfig coordinates.
   */
  BoundingBox _get_bounding_box() const;

  void _save(std::ostream &os, const BoundingBox &bbox_fig);
  void _save(const std::string &filename, const std::string &format, const BoundingBox &bbox_fig, const std::string &options = "");
  void _save(const std::string &filename_format, int counter, const std::string &file_format, const BoundingBox &bbox_fig, const std::string &options = "");

  /**
    Write an invisible border.
    Pen width is 0, color is white, depth is 999.
    Uses xfig coordinates.
   */
  void _write_invisible_border(std::ostream &os, int left, int right, int bottom, int top);

#if (__GNUC__ >= 3)
  typedef __gnu_cxx::hash_map<DrawingStyle::Color,int,DrawingStyle::ColorHashFunc> HashMap;
#else
  typedef std::hash_map<DrawingStyle::Color,int,DrawingStyle::ColorHashFunc> HashMap;
#endif
  HashMap _color_map;
  static const unsigned int FIRST_USER_DEFINED_COLOR = 32;
  static const unsigned int MAX_USER_DEFINED_COLORS = 512;
  static HashMap _static_color_map;
  static bool _static_color_map_initialized;
  static void _initialize_static_color_map();
  int _get_color_index(DrawingStyle::Color color);

  std::list<FigObject*> _the_drawing;
//  BoundingBox _bounding_box;

  int _pen_color;
  int _pen_width;
  int _line_style;
  int _area_fill;
  int _fill_color;
};

} // namespace Faw

#endif
