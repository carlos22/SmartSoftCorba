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

#include "fawDrawingStoreReplayPainter.hh"

std::map<int, Faw::DrawingStoreReplayPainter::DrawingOperation*> Faw::DrawingStoreReplayPainter::_prototypes;

bool Faw::DrawingStoreReplayPainter::_prototypes_initialized = false;

Faw::DrawingStoreReplayPainter::DrawingStoreReplayPainter(const std::string &label)
: _label(label)
{
  if(!_prototypes_initialized) _initialize_prototypes();
}

Faw::DrawingStoreReplayPainter::~DrawingStoreReplayPainter()
{
  for(unsigned int i=0; i<_the_drawing.size(); ++i) delete _the_drawing[i];
  _the_drawing.clear();
}

Faw::DrawingStoreReplayPainter::DrawingStoreReplayPainter(const Faw::DrawingStoreReplayPainter &srp)
{
  _label = srp._label;
  _the_drawing.resize(srp._the_drawing.size());
  for(unsigned int i=0; i<_the_drawing.size(); ++i)
  {
    _the_drawing[i] = srp._the_drawing[i]->clone();
  }
}

Faw::DrawingStoreReplayPainter &Faw::DrawingStoreReplayPainter::operator=(const Faw::DrawingStoreReplayPainter &srp)
{
  if(&srp!=this)
  {
    clear();
    _label = srp._label;
    _the_drawing.resize(srp._the_drawing.size());
    for(unsigned int i=0; i<_the_drawing.size(); ++i)
    {
      _the_drawing[i] = srp._the_drawing[i]->clone();
    }
  }
  return *this;
}

void Faw::DrawingStoreReplayPainter::save(std::ostream &os) const
{
  os << _label.size() << " " << _label << std::endl;
  os << _the_drawing.size() << std::endl;
  for(unsigned int i=0; i<_the_drawing.size(); ++i)
  {
    os << _the_drawing[i]->id() << std::endl;
    _the_drawing[i]->save(os);
  }
}

void Faw::DrawingStoreReplayPainter::save(std::string &s) const
{
  std::ostringstream os;
  save(os);
  s = os.str();
}

void Faw::DrawingStoreReplayPainter::load(std::istream &is)
{
  unsigned int size;
  char ch;
  DrawingOperation *dop;

  // read the label
  is >> size;
  is.get(ch); // consume separating space (see save() method)
  _label.resize(size);
  for(unsigned int i=0; i<size; ++i) is.get(_label[i]);

  // read the drawing
  for(unsigned int i=0; i<_the_drawing.size(); ++i) delete _the_drawing[i];
  _the_drawing.clear();
  
  is >> size;
  for(unsigned int i=0; (i<size) && (is.good()); ++i)
  {
    int id;
    is >> id;
    std::map<int, DrawingOperation*>::iterator proto_iter = _prototypes.find(id);
    if(proto_iter!=_prototypes.end())
    {
      dop = (*proto_iter).second->clone();
      dop->load(is);
      _the_drawing.push_back(dop);
    }
  }
}

void Faw::DrawingStoreReplayPainter::load(const std::string &s)
{
  if(s.length()>0)
  {
    std::istringstream is(s.c_str());
    load(is);
  }
}

void Faw::DrawingStoreReplayPainter::replay(Faw::Painter &painter) const
{
  painter.begin();
  for(unsigned int i=0; i<_the_drawing.size(); ++i)
  {
    _the_drawing[i]->replay(painter);
  }
  painter.end();
}


void Faw::DrawingStoreReplayPainter::replay(std::istream &is, Faw::Painter &painter)
{
  unsigned int size;
  DrawingOperation *dop;

  if(!_prototypes_initialized) _initialize_prototypes();

  is >> size;
  painter.begin();
  for(unsigned int i=0; i<size; ++i)
  {
    int id;
    is >> id;
    std::map<int, DrawingOperation*>::iterator proto_iter = _prototypes.find(id);
    if(proto_iter!=_prototypes.end())
    {
      dop = (*proto_iter).second->clone();
      dop->load(is);
      dop->replay(painter);
      delete dop;
    }
  }
  painter.end();
}


void Faw::DrawingStoreReplayPainter::replay(const std::string &s, Faw::Painter &painter)
{
  std::istringstream is(s.c_str());
  replay(is, painter);
}


void Faw::DrawingStoreReplayPainter::clear()
{
  for(unsigned int i=0; i<_the_drawing.size(); ++i) delete _the_drawing[i];
  _the_drawing.clear();
}


void Faw::DrawingStoreReplayPainter::print(std::ostream &os) const
{
  os << "Faw::DrawingStoreReplayPainter" << std::endl;
  for(unsigned int i=0; i<_the_drawing.size(); ++i)
  {
    os << "  ";
    _the_drawing[i]->print(os);
    os << std::endl;
  }
}


void Faw::DrawingStoreReplayPainter::_draw_polygon(const std::vector<Faw::Painter::Point> &points, unsigned int num_points)
{
  _the_drawing.push_back(new DrawPolygon(points, num_points));
}

void Faw::DrawingStoreReplayPainter::_draw_chain(const std::vector<Faw::Painter::Point> &points, unsigned int num_points)
{
  _the_drawing.push_back(new DrawChain(points, num_points));
}

void Faw::DrawingStoreReplayPainter::_draw_cross(double x, double y)
{
  _the_drawing.push_back(new DrawPoint(x,y,Faw::DrawingStyle::CROSS));
}

void Faw::DrawingStoreReplayPainter::_draw_pixel(double x, double y)
{
  _the_drawing.push_back(new DrawPoint(x,y,Faw::DrawingStyle::POINT));
}

void Faw::DrawingStoreReplayPainter::_draw_node(double x, double y)
{
  _the_drawing.push_back(new DrawPoint(x,y,Faw::DrawingStyle::NODE));
}

void Faw::DrawingStoreReplayPainter::_draw_segment(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new DrawSegment(x1,y1,x2,y2,DrawSegment::SEGMENT_STYLE));
}

void Faw::DrawingStoreReplayPainter::_draw_line(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new DrawSegment(x1,y1,x2,y2,DrawSegment::LINE_STYLE));
}

void Faw::DrawingStoreReplayPainter::_draw_ray(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new DrawSegment(x1,y1,x2,y2,DrawSegment::RAY_STYLE));
}

void Faw::DrawingStoreReplayPainter::_draw_ellipse(double x, double y, double a, double b, double alpha)
{
  _the_drawing.push_back(new DrawEllipse(x,y,a,b,alpha));
}

void Faw::DrawingStoreReplayPainter::_draw_circle(double x, double y, double r)
{
  _the_drawing.push_back(new DrawCircle(x,y,r));
}

void Faw::DrawingStoreReplayPainter::_draw_arc(double x, double y, double radius, double begin, double end)
{
  _the_drawing.push_back(new DrawArc(x,y,radius,begin,end));
}

void Faw::DrawingStoreReplayPainter::_draw_arrow(double x1, double y1, double x2, double y2)
{
  _the_drawing.push_back(new DrawSegment(x1,y1,x2,y2,DrawSegment::ARROW_STYLE));
}

void Faw::DrawingStoreReplayPainter::_draw_text(double x, double y, const char *txt)
{
  _the_drawing.push_back(new DrawText(x,y,txt));
}

void Faw::DrawingStoreReplayPainter::_draw_text(double x, double y, const char *txt, char orientation, char h_align, char v_align)
{
  _the_drawing.push_back(new DrawText(x,y,txt,orientation,h_align,v_align));
}

void Faw::DrawingStoreReplayPainter::_style_changed()
{
  _the_drawing.push_back(new DrawingStyleChange(get_style()));
}

void Faw::DrawingStoreReplayPainter::_layer_changed()
{
  _the_drawing.push_back(new DrawingLayerChange(get_layer()));
}

void Faw::DrawingStoreReplayPainter::_initialize_prototypes()
{
  if(!_prototypes_initialized)
  {
    Faw::DrawingStoreReplayPainter::DrawingOperation *dop;
    dop = new DrawPolygon; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawChain; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawPoint; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawSegment; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawEllipse; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawCircle; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawArc; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawText; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawingStyleChange; _prototypes.insert(std::make_pair(dop->id(), dop));
    dop = new DrawingLayerChange; _prototypes.insert(std::make_pair(dop->id(), dop));
    _prototypes_initialized = true;
  }
}

void Faw::DrawingStoreReplayPainter::DrawPolygon::save(std::ostream &os) const
{
  os << _points.size() << std::endl;
  for(unsigned int i=0; i<_points.size(); ++i) os << _points[i][0] << "\t" << _points[i][1] << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawPolygon::load(std::istream &is)
{
  unsigned int size;
  is >> size;
  _points.resize(size);
  for(unsigned int i=0; i<_points.size(); ++i) is >> _points[i][0] >> _points[i][1];
}

void Faw::DrawingStoreReplayPainter::DrawPolygon::print(std::ostream &os) const
{
  os << "polygon(size=" << _points.size() << ", points=(";
  for(unsigned int i=0; i<_points.size(); ++i)
  {
    if(i>0) os << ",";
    os << "(" << _points[i][0] << "," << _points[i][1] << ")";
  } 
  os << "))";
}


void Faw::DrawingStoreReplayPainter::DrawChain::save(std::ostream &os) const
{
  os << _points.size() << std::endl;
  for(unsigned int i=0; i<_points.size(); ++i) os << _points[i][0] << "\t" << _points[i][1] << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawChain::load(std::istream &is)
{
  unsigned int size;
  is >> size;
  _points.resize(size);
  for(unsigned int i=0; i<_points.size(); ++i) is >> _points[i][0] >> _points[i][1];
}

void Faw::DrawingStoreReplayPainter::DrawChain::print(std::ostream &os) const
{
  os << "chain(size=" << _points.size() << ", points=(";
  for(unsigned int i=0; i<_points.size(); ++i)
  {
    if(i>0) os << ",";
    os << "(" << _points[i][0] << "," << _points[i][1] << ")";
  } 
  os << "))";
}


void Faw::DrawingStoreReplayPainter::DrawPoint::save(std::ostream &os) const
{
  os << _x << "\t" << _y << "\t" << (int)_point_style << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawPoint::load(std::istream &is)
{
  int style;
  is >> _x >> _y >> style;
  _point_style = (Faw::DrawingStyle::PointStyle)style;
}


void Faw::DrawingStoreReplayPainter::DrawSegment::replay(Faw::Painter &painter) const
{
  switch(_style)
  {
    case SEGMENT_STYLE: painter.draw_segment(_x1,_y1,_x2,_y2); return;
    case RAY_STYLE:     painter.draw_ray(_x1,_y1,_x2,_y2); return;
    case LINE_STYLE:    painter.draw_line(_x1,_y1,_x2,_y2); return;
    case ARROW_STYLE:   painter.draw_arrow(_x1,_y1,_x2,_y2); return;
    default: return;
  }
}

void Faw::DrawingStoreReplayPainter::DrawSegment::save(std::ostream &os) const
{
  os << _x1 << "\t" << _y1 << "\t" << _x2 << "\t" << _y2 << "\t" << (int)_style << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawSegment::load(std::istream &is)
{
  int style;
  is >> _x1 >> _y1 >> _x2 >> _y2 >> style;
  _style = (DrawSegment::Style)style;
}


void Faw::DrawingStoreReplayPainter::DrawEllipse::save(std::ostream &os) const
{
  os << _x << "\t" << _y << "\t" << _a << "\t" << _b << "\t" << _alpha << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawEllipse::load(std::istream &is)
{
  is >> _x >> _y >> _a >> _b >> _alpha;
}


void Faw::DrawingStoreReplayPainter::DrawCircle::save(std::ostream &os) const
{
  os << _x << "\t" << _y << "\t" << _r << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawCircle::load(std::istream &is)
{
  is >> _x >> _y >> _r;
}


void Faw::DrawingStoreReplayPainter::DrawArc::save(std::ostream &os) const
{
  os << _x << "\t" << _y << "\t" << _radius << "\t" << _begin << "\t" << _end << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawArc::load(std::istream &is)
{
  is >> _x >> _y >> _radius >> _begin >> _end;
}

void Faw::DrawingStoreReplayPainter::DrawText::save(std::ostream &os) const
{
  os << _x << "\t" << _y << "\t" << _txt.length() << std::endl;
  os << _txt << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawText::load(std::istream &is)
{
  int len;
  is >> _x >> _y >> len >> std::ws;
  char buffer[len];
  for(int i=0; i<len; ++i) is.get(buffer[i]);
  _txt = std::string(buffer, len);
}


void Faw::DrawingStoreReplayPainter::DrawingStyleChange::save(std::ostream &os) const
{
  _style.save(os);
}

void Faw::DrawingStoreReplayPainter::DrawingStyleChange::load(std::istream &is)
{
  _style.load(is);
}


void Faw::DrawingStoreReplayPainter::DrawingLayerChange::save(std::ostream &os) const
{
  os << _layer << std::endl;
}

void Faw::DrawingStoreReplayPainter::DrawingLayerChange::load(std::istream &is)
{
  is >> _layer;
}


