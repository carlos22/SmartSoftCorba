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
 
#include "fawDrawingWidgetPainter.hh"


Faw::DrawingWidgetPainter::DrawingWidgetPainter
    (
        QPainter &painter,
        Faw::DrawingWidgetTransformation dwt
    )
  : _painter(painter),
    _dwt(dwt),
    _current_layer_visible(true),
    _viewport(_dwt.left(), _dwt.top(), _dwt.right(), _dwt.bottom())
{
  _small_length = _dwt.win2model_d(SMALL_LENGTH_PIXELS);
}


void Faw::DrawingWidgetPainter::hide_layer(unsigned int layer)
{ 
  _hidden_layers.insert(layer); 
  if(get_layer()==layer)
  {
    _current_layer_visible = false;
  }
}

void Faw::DrawingWidgetPainter::show_layer(unsigned int layer)
{ 
  _hidden_layers.erase(layer); 
  if(get_layer()==layer)
  {
    _current_layer_visible = true;
  }
}

void Faw::DrawingWidgetPainter::_draw_polygon(const std::vector<Faw::Painter::Point> &points, unsigned int num_points)
{
  if(_current_layer_visible)
  {
    if(_point_buffer.size()<num_points) _point_buffer.resize(num_points);
    for(unsigned int i=0; i<num_points; i++) _point_buffer.setPoint(i, _dwt(points[i][0], points[i][1]));
    _painter.drawPolygon(_point_buffer, false, 0, num_points);
  }
}



void Faw::DrawingWidgetPainter::_draw_chain(const std::vector<Faw::Painter::Point> &points, int unsigned num_points)
{
  if(_current_layer_visible)
  {
    if(_point_buffer.size()<num_points) _point_buffer.resize(num_points);
    for(unsigned int i=0; i<num_points; i++) _point_buffer.setPoint(i, _dwt(points[i][0], points[i][1]));
    _painter.drawPolyline(_point_buffer, 0, num_points);
  }
}



void Faw::DrawingWidgetPainter::_draw_cross(double x, double y)
{
  this->_draw_segment(x - _small_length, y - _small_length, x + _small_length, y + _small_length);
  this->_draw_segment(x - _small_length, y + _small_length, x + _small_length, y - _small_length);
}



void Faw::DrawingWidgetPainter::_draw_pixel(double x, double y)
{
  if(_current_layer_visible)
  {
    _painter.drawPoint(_dwt(x,y));
  }
}



void Faw::DrawingWidgetPainter::_draw_node(double x, double y)
{
  this->_draw_circle(x, y, _small_length);
}



void Faw::DrawingWidgetPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  if(_current_layer_visible)
  {
    _painter.drawLine(_dwt(x1,y1), _dwt(x2,y2));
  }
}



void Faw::DrawingWidgetPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  if(_current_layer_visible)
  {
    if((x1!=x2) || (y1!=y2))
    {
      double ix1,iy1,ix2,iy2;
      if(_viewport.intersection(x1,y1,x2,y2, ix1,iy1,ix2,iy2)==2)
      {
        _draw_segment(ix1,iy1,ix2,iy2);
      }
    }
  }
}



void Faw::DrawingWidgetPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  if(_current_layer_visible)
  if((x1!=x2) || (y1!=y2))
  {
    double ix1,iy1,ix2,iy2;
    if(_viewport.intersection(x1,y1,x2,y2, ix1,iy1,ix2,iy2)==2)
    {
      const double dx0 =  x2 - x1;
      const double dy0 =  y2 - y1;
      const double dx1 = ix1 - x1;
      const double dy1 = iy1 - y1;
      const double dx2 = ix2 - x1;
      const double dy2 = iy2 - y1;

      const double sp1 = dx0*dx1 + dy0*dy1;
      const double sp2 = dx0*dx2 + dy0*dy2;

      if(_dwt.is_visible(x1,y1))
      {
        if(sp1>sp2)
        {
          _draw_arrow(x1,y1,ix1,iy1);
        }
        else
        {
          _draw_arrow(x1,y1,ix2,iy2);
        }
        _draw_node(x1,y1);
      }
      else
      {
        if((sp1>=0) && (sp2>=0))
        {
          if((dx1*dx1+dy1*dy1)<(dx2*dx2+dy2*dy2))
          {
            _draw_arrow(ix1,iy1,ix2,iy2);
          }
          else
          {
            _draw_arrow(ix2,iy2,ix1,iy1);
          }
        }
      }
    }
  }
}



void Faw::DrawingWidgetPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  if(_current_layer_visible)
  {
    const int qx = _dwt.model2win_x(x);
    const int qy = _dwt.model2win_y(y);
    const int qa = _dwt(a);
    const int qb = _dwt(b);
    _painter.save();
    _painter.translate(qx, qy);
    _painter.rotate(-180*alpha/M_PI);
    _painter.drawEllipse(-qa, -qb, 2*qa, 2*qb);
    _painter.restore();  
  }
}



void Faw::DrawingWidgetPainter::_draw_circle(double x, double y, double r)
{
  if(_current_layer_visible)
  {
    _painter.drawEllipse(_dwt.model2win_x(x - r), _dwt.model2win_y(y + r), _dwt(2*r), _dwt(2*r));
  }
}



void Faw::DrawingWidgetPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  if(_current_layer_visible)
  {
    const double d = radius + radius;
    const int a = (int)rint((begin - 2*M_PI * floor(begin / (2*M_PI))) * 2880/M_PI);
    const int b = (int)rint((end   - 2*M_PI * floor(end   / (2*M_PI))) * 2880/M_PI);
    const int alen = (b>a) ? b - a : 5760 + b - a;
    _painter.drawArc(_dwt.model2win_x(x - radius), _dwt.model2win_y(y + radius), _dwt(d), _dwt(d), a, alen);
  }
}



void Faw::DrawingWidgetPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  if(_current_layer_visible)
  {
    std::vector<Faw::Painter::Point> arrowhead_polygon(4);
    _draw_segment(x1,y1,x2,y2);

    const double dx = x2-x1;
    const double dy = y2-y1;
    const double angle = atan2(dy,dx);
    const double sqr_length = dx*dx+dy*dy;
    const double delta = 20.0; // degrees
    const double rot = (180.0 - delta)*M_PI/180.0;
    const double alpha1 = angle + rot;
    const double alpha2 = angle - rot;
    const double max_size = 3*_small_length;
    const double l = (sqr_length>(4*max_size*max_size))?max_size:0.5*sqrt(sqr_length);

    const Faw::DrawingStyle s = get_style();
    set_fill_color(s.get_pen_color());
    arrowhead_polygon[0][0]  = x2; 
    arrowhead_polygon[0][1] = y2;
    arrowhead_polygon[1][0]  = x2+l*cos(alpha1); 
    arrowhead_polygon[1][1] = y2+l*sin(alpha1);
    arrowhead_polygon[2][0]  = x2-l*cos(angle)/2; 
    arrowhead_polygon[2][1] = y2-l*sin(angle)/2;
    arrowhead_polygon[3][0]  = x2+l*cos(alpha2); 
    arrowhead_polygon[3][1] = y2+l*sin(alpha2);
    _style_changed();
    _draw_polygon(arrowhead_polygon, 4);
    set_style(s);
  }
}



void Faw::DrawingWidgetPainter::_draw_text(double x, double y, const char *msg)
{
  if(_current_layer_visible)
  {
    _painter.drawText(_dwt(x,y), QString(msg));
  }
}



void Faw::DrawingWidgetPainter::_draw_text(double x, double y, const char *msg, char orientation, char h_align, char v_align)
{
  if(_current_layer_visible)
  {
    if(orientation!='h')
    {
      std::cerr << "Faw::DrawingWidgetPainter: orientation \"" << orientation << "\" not supported" << std::endl;
    }

    QPoint p = _dwt(x,y);
    QRect text_bounds = _painter.fontMetrics().boundingRect(QString(msg));
    const int text_width  = text_bounds.width();
    const int text_height = text_bounds.height();

    int wx = p.x();
    int wy = p.y();
    switch(h_align)
    {
      case 'l': wx = p.x(); break;
      case 'c': wx = p.x() - text_width/2; break;
      case 'r': wx = p.x() - text_width; break;
      default:  
      {
        std::cerr << "Faw::DrawingWidgetPainter: h_align \"" << h_align << "\" not supported" << std::endl;
      }
    }
    switch(v_align)
    {
      case 't': wy = p.y() + text_height; break;
      case 'c': wy = p.y() + text_height/2; break;
      case 'b': wy = p.y(); break;
      default:
      {
        std::cerr << "Faw::DrawingWidgetPainter: v_align \"" << v_align << "\" not supported" << std::endl;
      }
    }
    _painter.drawText(QPoint(wx,wy), QString(msg));
  }
}



void Faw::DrawingWidgetPainter::_style_changed()
{
  QPen pen;
  pen.setWidth(get_style().get_pen_width());
  const Faw::DrawingStyle::Color pen_color = get_style().get_pen_color();
  const int pr = Faw::DrawingStyle::red(pen_color);
  const int pg = Faw::DrawingStyle::green(pen_color);
  const int pb = Faw::DrawingStyle::blue(pen_color);
  pen.setColor(QColor(pr,pg,pb));
  switch(get_style().get_line_style())
  {
    case Faw::DrawingStyle::SOLID:        pen.setStyle(Qt::SolidLine); break;
    case Faw::DrawingStyle::DASH:         pen.setStyle(Qt::DashLine); break;
    case Faw::DrawingStyle::DOTTED:       pen.setStyle(Qt::DotLine); break;
    case Faw::DrawingStyle::DASH_DOT:     pen.setStyle(Qt::DashDotLine); break;
    case Faw::DrawingStyle::DASH_DOT_DOT: pen.setStyle(Qt::DashDotDotLine); break;
    default: break;
  }
  _painter.setPen(pen);

  if(get_style().get_fill())
  {
    const Faw::DrawingStyle::Color fill_color = get_style().get_fill_color();
    const int fr = Faw::DrawingStyle::red(fill_color);
    const int fg = Faw::DrawingStyle::green(fill_color);
    const int fb = Faw::DrawingStyle::blue(fill_color);
    _painter.setBrush(QColor(fr,fg,fb));
  }
  else
  {
    _painter.setBrush(QBrush::NoBrush);
  }
}



void Faw::DrawingWidgetPainter::_layer_changed()
{
  _current_layer_visible = (_hidden_layers.count(get_layer())==0);
}



void Faw::DrawingWidgetPainter::_begin()
{
  _painter.save();
}



void Faw::DrawingWidgetPainter::_end()
{
  _painter.restore();
}


QPointArray Faw::DrawingWidgetPainter::_point_buffer(1000);

