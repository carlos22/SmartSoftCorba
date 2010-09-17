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
 
#include <sstream>
#include <iosfwd>
#include <string.h>
#include "fawStreamPainter.hh"

using namespace Faw;

StreamPainter::StreamPainter()
: _os(0)
{
}

StreamPainter::StreamPainter(std::ostream &os)
: _os(0)
{
  this->open(os);
}

StreamPainter::~StreamPainter()
{
  if(_os) this->close();
}

void StreamPainter::open(std::ostream &os)
{
  if(_os) this->close();
  _os = &os;
}

void StreamPainter::close()
{
  *_os << FAW_STREAM_PAINTER_DONE << std::endl;
  _os = 0;
}

void StreamPainter::replay(std::istream &is, Painter &painter)
{
  int elem;
  unsigned int n;
  std::vector<Painter::Point> points;
  double x1,y1,x2,y2;
  double r1,r2,a,b;
  while(true)
  {
    is >> elem;
    if(is.eof() || !is.good() || (elem==FAW_STREAM_PAINTER_DONE)) break;
    switch(elem)
    {
      case FAW_STREAM_PAINTER_POLYGON:
      {
        is >> n;
        if(n > points.size()) points.resize(n);
        for(unsigned int i=0; i<n; ++i)
        {
          is >> points[i][0] >> points[i][1];
        }
        painter.draw_polygon(points.begin(), points.begin() + n);
        break;
      }
      case FAW_STREAM_PAINTER_CHAIN:
      {
        is >> n;
        if(n > points.size()) points.resize(n);
        for(unsigned int i=0; i<n; ++i)
        {
          is >> points[i][0] >> points[i][1];
        }
        painter.draw_chain(points.begin(), points.begin() + n);
        break;
      } 
      case FAW_STREAM_PAINTER_CROSS:
      {
        is >> x1 >> y1;
        painter.draw_cross(x1,y1);
        break;
      } 
      case FAW_STREAM_PAINTER_PIXEL:
      {
        is >> x1 >> y1;
        painter.draw_pixel(x1,y1);
        break;
      } 
      case FAW_STREAM_PAINTER_NODE:
      {
        is >> x1 >> y1;
        painter.draw_node(x1,y1);
        break;
      } 
      case FAW_STREAM_PAINTER_SEGMENT:
      {
        is >> x1 >> y1 >> x2 >> y2;
        painter.draw_segment(x1,y1,x2,y2);
        break;
      } 
      case FAW_STREAM_PAINTER_LINE:
      {
        is >> x1 >> y1 >> x2 >> y2;
        painter.draw_line(x1,y1,x2,y2);
        break;
      } 
      case FAW_STREAM_PAINTER_RAY:
      {
        is >> x1 >> y1 >> x2 >> y2;
        painter.draw_ray(x1,y1,x2,y2);
        break;
      } 
      case FAW_STREAM_PAINTER_ELLIPSE:
      {
        is >> x1 >> y1 >> r1 >> r2 >> a;
        painter.draw_ellipse(x1,y1,r1,r2,a);
        break;
      } 
      case FAW_STREAM_PAINTER_CIRCLE:
      {
        is >> x1 >> y1 >> r1;
        painter.draw_circle(x1,y1,r1);
        break;
      } 
      case FAW_STREAM_PAINTER_ARC:
      {
        is >> x1 >> y1 >> r1 >> a >> b;
        painter.draw_arc(x1,y2,r1,a,b);
        break;
      } 
      case FAW_STREAM_PAINTER_ARROW:
      {
        is >> x1 >> y1 >> x2 >> y2;
        painter.draw_arrow(x1,y1,x2,y2);
        break;
      } 
      case FAW_STREAM_PAINTER_TEXT:
      {
        char ch;
        is >> x1 >> y1 >> n;
        is.get(ch); // separating space
        std::string txt(n,'\0');
        for(unsigned int i=0; i<n; ++i)
        {
          is.get(txt[i]);
        }
        painter.draw_text(x1,y1,txt.c_str());
        break;
      } 
      case FAW_STREAM_PAINTER_TEXT2:
      {
        char ch;
        is >> x1 >> y1 >> n;
        is.get(ch); // separating space
        std::string txt(n,'\0');
        for(unsigned int i=0; i<n; ++i)
        {
          is.get(txt[i]);
        }
        int o,h,v;
        is >> o >> h >> v;
        painter.draw_text(x1,y1,txt.c_str(),o,h,v);
        break;
      } 
      case FAW_STREAM_PAINTER_STYLE:
      {
        DrawingStyle ds;
        ds.load(is);
        painter.set_style(ds);
        break;
      } 
      case FAW_STREAM_PAINTER_LAYER:
      {
        is >> n;
        painter.set_layer(n);
        break;
      } 
      case FAW_STREAM_PAINTER_BEGIN:
      {
        painter.begin();
        break;
      } 
      case FAW_STREAM_PAINTER_END:
      {
        painter.end();
        break;
      } 
    }
  }
}

void StreamPainter::_begin()
{
  *_os << FAW_STREAM_PAINTER_BEGIN << std::endl;
}

void StreamPainter::_end()
{
  *_os << FAW_STREAM_PAINTER_END << std::endl;
}

void StreamPainter::_draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  *_os << FAW_STREAM_PAINTER_POLYGON << " " << num_points;
  for(unsigned int i=0; i<num_points; ++i)
  {
    *_os << " " << points[i][0] << " " << points[i][1];
  }
  *_os << std::endl;
}

void StreamPainter::_draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  *_os << FAW_STREAM_PAINTER_CHAIN << " " << num_points;
  for(unsigned int i=0; i<num_points; ++i)
  {
    *_os << " " << points[i][0] << " " << points[i][1];
  }
  *_os << std::endl;
}

void StreamPainter::_draw_cross(double x, double y)
{
  *_os << FAW_STREAM_PAINTER_CROSS << " " << x << " " << y << std::endl;
}

void StreamPainter::_draw_pixel(double x, double y)
{
  *_os << FAW_STREAM_PAINTER_PIXEL << " " << x << " " << y << std::endl;
}

void StreamPainter::_draw_node(double x, double y)
{
  *_os << FAW_STREAM_PAINTER_NODE << " " << x << " " << y << std::endl;
}

void StreamPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  *_os << FAW_STREAM_PAINTER_SEGMENT << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
}

void StreamPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  *_os << FAW_STREAM_PAINTER_LINE << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
}

void StreamPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  *_os << FAW_STREAM_PAINTER_RAY << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
}

void StreamPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  *_os << FAW_STREAM_PAINTER_ELLIPSE << " " << x << " " << y << " " << a << " " << b << " " << alpha << std::endl;
}

void StreamPainter::_draw_circle(double x, double y, double r)
{
  *_os << FAW_STREAM_PAINTER_CIRCLE << " " << x << " " << y << " " << r << std::endl;
}

void StreamPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  *_os << FAW_STREAM_PAINTER_ARC << " " << x << " " << y << " " << radius << " " << begin << " " << end << std::endl;
}

void StreamPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  *_os << FAW_STREAM_PAINTER_ARROW << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
}

void StreamPainter::_draw_text(double x, double y, const char *txt)
{
  *_os << FAW_STREAM_PAINTER_TEXT << " " << x << " " << y << " " << strlen(txt) << " " << txt << std::endl;
}

void StreamPainter::_draw_text(double x, double y, const char *txt, char orientation, char h_align, char v_align)
{
  *_os << FAW_STREAM_PAINTER_TEXT2 << " " << x << " " << y << " " << strlen(txt) << " " << txt << " " << int(orientation) << " " << int(h_align) << " " << int(v_align) << std::endl;
}

void StreamPainter::_style_changed()
{
  *_os << FAW_STREAM_PAINTER_STYLE << " ";
  get_style().save(*_os);
}

void StreamPainter::_layer_changed()
{
  *_os << FAW_STREAM_PAINTER_LAYER << " " << get_layer() << std::endl;
}

