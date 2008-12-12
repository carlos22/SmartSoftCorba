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
 
#ifndef FAW_DRAWING_GENERIC_PROXY_SHAPE_NODE_HH
#define FAW_DRAWING_GENERIC_PROXY_SHAPE_NODE_HH

#include <iostream>
#include <map>
#include <utility>

#include <fawDrawingShapeNode.hh>


namespace Faw {

class DrawingGenericProxyShapeNode: public DrawingShapeNode
{
public:
  /**
    Constructor.
   */
  DrawingGenericProxyShapeNode();

  /**
    Destructor.
   */
  ~DrawingGenericProxyShapeNode();

  /**
    Create a clone of this generic proxy shape node.
    Be careful if you clone a node n1 and eventually destroy n1 and its remote objects,
    since the clone will reference invalid memory.
   */
  virtual DrawingGenericProxyShapeNode *clone() const;
  virtual void render(Painter &painter) const;
  virtual void print(std::ostream &os) const;

  template<class Container, class PaintMethod>
  inline void add_container(Container *c, const PaintMethod &paint_method)
  {
    if(_remote_objects.find(c)==_remote_objects.end())
    {
      _PaintableInterface *paintable = new _PaintableContainer<Container, PaintMethod>(c, paint_method);
      _remote_objects.insert(make_pair(c, paintable));
    }
  }

  template<class Container>
  inline void add_container(Container *c)
  {
    DefaultPaintMethod<typename Container::value_type> paint_method;
    add_container(c, paint_method);
  }

  template<class Object, class PaintMethod>
  inline void add_object(Object *o, const PaintMethod &paint_method)
  {
    if(_remote_objects.find(o)==_remote_objects.end())
    {
      _PaintableInterface *paintable = new _PaintableObject<Object, PaintMethod>(o, paint_method);
      _remote_objects.insert(make_pair(o, paintable));
    }
  }

  template<class Object>
  inline void add_object(Object *o)
  {
    DefaultPaintMethod<Object> paint_method;
    add_object(o, paint_method);
  }

  inline void remove(void *p)
  {
    std::map<void*, _PaintableInterface*>::iterator iter;
    iter = _remote_objects.find(p);
    if(iter!=_remote_objects.end())
    {
      delete (*iter).second;
      _remote_objects.erase(iter);
    }
  }

private:

  //
  // Types
  //

  class _PaintableInterface
  {
  public:
    virtual ~_PaintableInterface() {}
    virtual void paint(Painter &painter) = 0;
    virtual _PaintableInterface *clone() const = 0;
    virtual void print(std::ostream &os) const = 0;
  };

  template<class Container, class PaintMethod>
  class _PaintableContainer: public _PaintableInterface
  {
  public:
    _PaintableContainer(Container *c, const PaintMethod &pm = PaintMethod()) : _c(c), _pm(pm) {}
    virtual void paint(Painter &painter) { painter.draw(_c->begin(), _c->end(), _pm); }
    virtual _PaintableContainer<Container,PaintMethod> *clone() const { return new _PaintableContainer<Container,PaintMethod>(_c,_pm); }
    virtual void print(std::ostream &os) const { os << "Container"; }
  private:
    Container *_c;
    PaintMethod _pm;
  };

  template<class Object, class PaintMethod>
  class _PaintableObject: public _PaintableInterface
  {
  public:
    _PaintableObject(Object *o, const PaintMethod &pm = PaintMethod()) : _o(o), _pm(pm) {}
    virtual void paint(Painter &dwp) { dwp.draw(*_o, _pm); }
    virtual _PaintableObject<Object,PaintMethod> *clone() const { return new _PaintableObject<Object,PaintMethod>(_o,_pm); }
    virtual void print(std::ostream &os) const { os << "Object"; }
  private:
    Object *_o;
    PaintMethod _pm;
  };

  //
  // Attributes
  //

  std::map<void*, _PaintableInterface*> _remote_objects;
};

} // namespace Faw

#endif
