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
 
#ifndef FAW_STREAM_PAINTER_HH
#define FAW_STREAM_PAINTER_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "fawPainter.hh"

namespace Faw {

/**
  An implementation of the Painter interface which 
  writes drawing commands to an ostream or replay 
  drawing commands from an istream onto another Painter.
  There is no intermediate storage of drawing commands.
 */
class StreamPainter: public Painter
{
public:
  /**
    Create a stream painter which is not yet connected to an ostream.
    Before using this painter, connect it to a stream.
   */
  StreamPainter();

  /**
    Create a stream painter connected to the ostream \a os.
   */
  StreamPainter(std::ostream &os);

  ~StreamPainter();

  /**
    Connect this stream painter to the ostream \a os.
    Disconnects it first, if necessary.
   */
  void open(std::ostream &os);

  /**
    If connected to an ostream, this method closes the connection.
   */
  void close();

  /**
    Replay drawing commands from istream \a is to the painter \a painter.
   */
  static void replay(std::istream &is, Painter &painter);

protected:

  //
  // Drawing
  //

  virtual void _begin();
  virtual void _end();
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

  std::ostream *_os;

  enum
  {
    FAW_STREAM_PAINTER_POLYGON	= 0x01,
    FAW_STREAM_PAINTER_CHAIN	= 0x02,
    FAW_STREAM_PAINTER_CROSS	= 0x03, 
    FAW_STREAM_PAINTER_PIXEL	= 0x04, 
    FAW_STREAM_PAINTER_NODE		= 0x05, 
    FAW_STREAM_PAINTER_SEGMENT	= 0x06, 
    FAW_STREAM_PAINTER_LINE		= 0x07, 
    FAW_STREAM_PAINTER_RAY		= 0x08, 
    FAW_STREAM_PAINTER_ELLIPSE	= 0x09, 
    FAW_STREAM_PAINTER_CIRCLE	= 0x10, 
    FAW_STREAM_PAINTER_ARC		= 0x11, 
    FAW_STREAM_PAINTER_ARROW	= 0x12, 
    FAW_STREAM_PAINTER_TEXT		= 0x13, 
    FAW_STREAM_PAINTER_TEXT2	= 0x14, 

    FAW_STREAM_PAINTER_STYLE	= 0xe1, 
    FAW_STREAM_PAINTER_LAYER	= 0xe2, 

    FAW_STREAM_PAINTER_BEGIN	= 0xf1,
    FAW_STREAM_PAINTER_END		= 0xf2,

    FAW_STREAM_PAINTER_DONE	    = 0xff
  };
};

} // namespace Faw

#endif
