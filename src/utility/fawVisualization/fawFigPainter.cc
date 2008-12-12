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
 
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iosfwd>

#include "fawFigPainter.hh"

#if (__GNUC__ >= 3)
#include "fawFileDesOutputStream.hh"
#endif

using namespace Faw;

FigPainter::FigPainter(double scale)
  : _scale(scale*45.0*1000) // xfig: 45 units/mm * 1000 mm/m
{
  if(!_static_color_map_initialized) _initialize_static_color_map();
}

FigPainter::~FigPainter()
{
  while(!_the_drawing.empty())
  {
    delete _the_drawing.front();
    _the_drawing.pop_front();
  }
}

void FigPainter::save(std::ostream &os)
{
  this->_save(os, this->_get_bounding_box());
}

void FigPainter::save(std::ostream &os, const BoundingBox &bbox)
{
  this->_save(os, _bbox_fig(bbox));
}

void FigPainter::save(const std::string &filename, const std::string &format, const std::string &options)
{
  this->_save(filename, format, this->_get_bounding_box(), options);
}

void FigPainter::save(const std::string &filename, const std::string &format, const BoundingBox &bbox, const std::string &options)
{
  this->_save(filename, format, _bbox_fig(bbox), options);
}

void FigPainter::save(const std::string &filename_format, int counter, const std::string &format, const std::string &options)
{
  this->_save(filename_format, counter, format, this->_get_bounding_box(), options);
}

void FigPainter::save(const std::string &filename_format, int counter, const std::string &format, const BoundingBox &bbox, const std::string &options)
{
  this->_save(filename_format, counter, format, _bbox_fig(bbox), options);
}

BoundingBox FigPainter::_get_bounding_box() const
{
  std::list<FigObject*>::const_iterator iter;
  BoundingBox bbox;
  iter = _the_drawing.begin();
  while(iter!=_the_drawing.end())
  {
    const BoundingBox b = (*iter)->bounding_box();
    bbox.add(b);
    ++iter;
  }
  return bbox;
}

void FigPainter::_save(std::ostream &os, const BoundingBox &bbox_fig)
{
  std::list<FigObject*>::iterator iter;

  os << "#FIG 3.2" << std::endl
     << "#Created by Faw::FigPainter (Chr.Schlegel, B.Kluge, FAW Ulm)" << std::endl
     << "Landscape" << std::endl
     << "Center" << std::endl
     << "Metric" << std::endl
     << "A4" << std::endl
     << "100.00" << std::endl
     << "Single" << std::endl
     << "-2" << std::endl
     << "1200 2" << std::endl;

  const int left = int(floor(bbox_fig.left()));
  const int right = int(ceil(bbox_fig.right()));
  const int bottom = int(floor(bbox_fig.bottom()));
  const int top = int(ceil(bbox_fig.top()));

  iter = _the_drawing.begin();
  while(iter!=_the_drawing.end())
  {
    (*iter)->translate(-left, -bottom);
    (*iter)->save(os, 0, right-left, 0, top-bottom);
    (*iter)->translate(left, bottom);
    ++iter;
  }
  this->_write_invisible_border(os, 0, right-left, 0, top-bottom);
}

void FigPainter::_save(const std::string &filename, const std::string &format, const BoundingBox &bbox_fig, const std::string &options)
{
  const char *fig2dev_formats[] = 
  { 
    "box", "cgm", "epic", "eepic", "eepicemu", "eps", "gif",
    "ibmgl", "jpeg", "latex", "mf", "pcx", "pdf", "pic", 
    "pictex", "png", "ppm", "ps", "sld", "textyl",  "tiff", 
    "tk", "tpic", "xbm", "xpm", 0 
  };
  if(format=="fig")
  {
    std::ofstream ofs(filename.c_str());
    this->_save(ofs, bbox_fig);
  }
  else
  {
    bool format_valid = false;
    for(int i=0; (fig2dev_formats[i] && !format_valid); i++) format_valid = (format==fig2dev_formats[i]);
    if(format_valid)
    {
      const std::string cmd_line = std::string("fig2dev -L ") + format + std::string(" ") + options + std::string(" > ") + filename;
      FILE *fig2dev_pipe = popen(cmd_line.c_str(), "w");
      if(fig2dev_pipe==NULL)
      {
        std::cerr << "Faw::FigPainter::save(filename, format) : failed to execute popen(" << cmd_line << ", \"w\")" << std::endl;
        return;
      }
#if (__GNUC__ >= 3)
      FileDesOutputStream ofs(fileno(fig2dev_pipe));
#else
      std::ofstream ofs(fileno(fig2dev_pipe));
#endif
      this->_save(ofs, bbox_fig);
      if(pclose(fig2dev_pipe)<0)
      {
        std::cerr << "Faw::FigPainter::save(filename, format) : failed to execute pclose() corresponding to popen(" << cmd_line << ", \"w\")" << std::endl;
      }
    }
    else
    {
      std::cerr << "Faw::FigPainter::save(filename, format) : unknown format \"" << format << "\"" << std::endl;
    }
  }
}

void FigPainter::_save(const std::string &filename_format, int counter, const std::string &format, const BoundingBox &bbox_fig, const std::string &options)
{
  const unsigned int buffer_length = filename_format.length() + 40;
  char buffer[buffer_length];
  buffer[buffer_length - 1] = '\0';
  snprintf(buffer, buffer_length - 1, filename_format.c_str(), counter);
  this->_save(std::string(buffer), format, bbox_fig, options);
}

void FigPainter::_write_invisible_border(std::ostream &os, int left, int right, int bottom, int top)
{
  FigPolygon border(4 /* num point */, 
    FigObject::DEFAULT /* style */, 
    0 /* pen_width */, 
    7 /* pen_color, 7 = white, see color map initialization */, 
    FigObject::UNFILLED /* area_fill */, 
    FigObject::DEFAULT /* fill_color */, 
    999 /* layer */);
  border.set_point(0, left, bottom);
  border.set_point(1, right, bottom);
  border.set_point(2, right, top);
  border.set_point(3, left, top);
  border.save(os, left, right, bottom, top);
}


void FigPainter::_draw_polygon(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  FigPolygon *polygon = new FigPolygon(num_points, _line_style, _pen_width, _pen_color, _area_fill, _fill_color, get_layer());
  for(unsigned int i=0; i<num_points; i++) 
  {
    polygon->set_point(i, _x_fig(points[i][0]), _y_fig(points[i][1]));
  }
  _the_drawing.push_back(polygon);
}

void FigPainter::_draw_chain(const std::vector<Painter::Point> &points, unsigned int num_points)
{
  FigChain *chain = new FigChain(num_points, _line_style, _pen_width, _pen_color, get_layer());
  for(unsigned int i=0; i<num_points; i++) chain->set_point(i, _x_fig(points[i][0]), _y_fig(points[i][1]));
  _the_drawing.push_back(chain);
}

void FigPainter::_draw_cross(double x, double y)
{
  _draw_segment(x-SMALL_LENGTH(), y-SMALL_LENGTH(), x+SMALL_LENGTH(), y+SMALL_LENGTH());
  _draw_segment(x-SMALL_LENGTH(), y+SMALL_LENGTH(), x+SMALL_LENGTH(), y-SMALL_LENGTH());
}

void FigPainter::_draw_pixel(double x, double y)
{
  _draw_segment(x,y,x,y);
}

void FigPainter::_draw_node(double x, double y)
{
  _draw_circle(x,y,SMALL_LENGTH());
}

void FigPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new FigSegment(_x_fig(x1), _y_fig(y1), _x_fig(x2), _y_fig(y2), _line_style, _pen_width, _pen_color, get_layer()));
}

void FigPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new FigSegment(_x_fig(x1), _y_fig(y1), _x_fig(x2), _y_fig(y2), _line_style, _pen_width, _pen_color, get_layer(), FigSegment::LINE));
}

void FigPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new FigSegment(_x_fig(x1), _y_fig(y1), _x_fig(x2), _y_fig(y2), _line_style, _pen_width, _pen_color, get_layer(), FigSegment::RAY));
}

void FigPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  _the_drawing.push_back(new FigEllipse(_x_fig(x), _y_fig(y), _x_fig(a), _x_fig(b), alpha, _line_style, _pen_width, _pen_color, _area_fill, _fill_color, get_layer()));
}

void FigPainter::_draw_circle(double x, double y, double r)
{
  _the_drawing.push_back(new FigEllipse(_x_fig(x), _y_fig(y), _x_fig(r), _x_fig(r), 0, _line_style, _pen_width, _pen_color, _area_fill, _fill_color, get_layer()));
}

void FigPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  _the_drawing.push_back(new FigArc(_x_fig(x), _y_fig(y), _x_fig(radius), begin, end, _line_style, _pen_width, _pen_color, _area_fill, _fill_color, get_layer()));
}

void FigPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new FigArrow(_x_fig(x1), _y_fig(y1), _x_fig(x2), _y_fig(y2), _line_style, _pen_width, _pen_color, get_layer()));
}

void FigPainter::_draw_text(double x, double y, const char *msg)
{
  _the_drawing.push_back(new FigText(_x_fig(x), _y_fig(y), msg, _pen_color, get_layer()));
}

void FigPainter::_draw_text(double x, double y, const char *msg, char orientation, char h_align, char v_align)
{
  FigText *t = new FigText(_x_fig(x), _y_fig(y), msg, _pen_color, get_layer());
  t->set_align(orientation, h_align, v_align);
  _the_drawing.push_back(t);
}

void FigPainter::_style_changed()
{
  _pen_width = get_style().get_pen_width();
  _pen_color = _get_color_index(get_style().get_pen_color());
  _line_style = get_style().get_line_style();

  if(get_style().get_fill())
  {
    _fill_color = _get_color_index(get_style().get_fill_color());
    _area_fill = FigObject::FULL_COLOR_SATURATION;
  }
  else
  {
    _fill_color = FigObject::DEFAULT;
    _area_fill = FigObject::UNFILLED;
  }
}
  
FigPainter::HashMap FigPainter::_static_color_map;
bool FigPainter::_static_color_map_initialized = false;

void FigPainter::_initialize_static_color_map()
{
  _static_color_map.clear();
  _static_color_map.resize(32);
  _static_color_map[DrawingStyle::Color(0x000000)] = 0;
  _static_color_map[DrawingStyle::Color(0x0000ff)] = 1;
  _static_color_map[DrawingStyle::Color(0x00ff00)] = 2;
  _static_color_map[DrawingStyle::Color(0x00ffff)] = 3;
  _static_color_map[DrawingStyle::Color(0xff0000)] = 4;
  _static_color_map[DrawingStyle::Color(0xff00ff)] = 5;
  _static_color_map[DrawingStyle::Color(0xffff00)] = 6;
  _static_color_map[DrawingStyle::Color(0xffffff)] = 7;

  _static_color_map[DrawingStyle::Color(0x00008e)] = 8;
  _static_color_map[DrawingStyle::Color(0x0000af)] = 9;
  _static_color_map[DrawingStyle::Color(0x0000d1)] = 10;
  _static_color_map[DrawingStyle::Color(0x87ceff)] = 11;
  _static_color_map[DrawingStyle::Color(0x008e00)] = 12;
  _static_color_map[DrawingStyle::Color(0x00af00)] = 13;
  _static_color_map[DrawingStyle::Color(0x00d100)] = 14;
  _static_color_map[DrawingStyle::Color(0x008e8e)] = 15;
  
  _static_color_map[DrawingStyle::Color(0x00afaf)] = 16;
  _static_color_map[DrawingStyle::Color(0x00d1d1)] = 17;
  _static_color_map[DrawingStyle::Color(0x8e0000)] = 18;
  _static_color_map[DrawingStyle::Color(0xaf0000)] = 19;
  _static_color_map[DrawingStyle::Color(0xd10000)] = 20;
  _static_color_map[DrawingStyle::Color(0x8e008e)] = 21;
  _static_color_map[DrawingStyle::Color(0xaf00af)] = 22;
  _static_color_map[DrawingStyle::Color(0xd100d1)] = 23;
  
  _static_color_map[DrawingStyle::Color(0x7f3000)] = 24;
  _static_color_map[DrawingStyle::Color(0xa03f00)] = 25;
  _static_color_map[DrawingStyle::Color(0xbf6000)] = 26;
  _static_color_map[DrawingStyle::Color(0xff7f7f)] = 27;
  _static_color_map[DrawingStyle::Color(0xffa0a0)] = 28;
  _static_color_map[DrawingStyle::Color(0xffbfbf)] = 29;
  _static_color_map[DrawingStyle::Color(0xffe0e0)] = 30;
  _static_color_map[DrawingStyle::Color(0xffd600)] = 31;
  
  _static_color_map_initialized = true;
}

int FigPainter::_get_color_index(DrawingStyle::Color color)
{
  FigPainter::HashMap::iterator iter;
  iter = _static_color_map.find(color);
  if(iter!=_static_color_map.end()) return (*iter).second;
  iter = _color_map.find(color);
  if(iter!=_color_map.end()) return (*iter).second;
  if(_color_map.size()<MAX_USER_DEFINED_COLORS)
  {
    const int new_color_index = _color_map.size() + FIRST_USER_DEFINED_COLOR;
    _color_map[color] = new_color_index;
    _the_drawing.push_front(new FigColor(color,new_color_index));
    return new_color_index;
  }
  else
  {
    std::cerr << "Faw::FigPainter: ran out of colors, ";
    int min_sqr_dist = -1;
    DrawingStyle::Color closest_color = 0x000000;
    const int query_red = DrawingStyle::red(color);
    const int query_green = DrawingStyle::green(color);
    const int query_blue = DrawingStyle::blue(color);
    iter = _color_map.begin();
    while(iter!=_color_map.end())
    {
      const int d1 = query_red   - int(DrawingStyle::red((*iter).first));
      const int d2 = query_green - int(DrawingStyle::green((*iter).first));
      const int d3 = query_blue  - int(DrawingStyle::blue((*iter).first));
      const int dist = d1*d1 + d2*d2 + d3*d3;
      if((min_sqr_dist<0) || (dist<min_sqr_dist))
      {
        closest_color = (*iter).first;
      }
      ++iter;
    }
    std::cerr << "using 0x" << std::hex << std::setw(6) << std::setfill('0') << closest_color 
              << " instead of 0x" << std::setw(6) << std::setfill('0') << color << std::dec << std::endl;
    return _color_map[closest_color];
  }
}


