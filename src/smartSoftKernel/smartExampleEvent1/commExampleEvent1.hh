// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2010 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#ifndef _COMM__EXAMPLE_EVENT1_HH
#define _COMM__EXAMPLE_EVENT1_HH

#include <iostream>
#include <string>
#include "exampleEvent1C.hh"

namespace Smart {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommExampleEvent1Parameter
{
protected:
  ExampleEvent1Parameter parameter;

public:
  CommExampleEvent1Parameter();
  virtual ~CommExampleEvent1Parameter();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::exampleEvent1Parameter";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent1Parameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommExampleEvent1Result
{
protected:
  ExampleEvent1Result result;

public:
  CommExampleEvent1Result();
  virtual ~CommExampleEvent1Result();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::exampleEvent1Result";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent1Result &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event state part
//
// --------------------------------------------------------------------------
class ExampleEventState
{
protected:
  int state;

public:
  ExampleEventState();
  virtual ~ExampleEventState();

  void set(int);
  void get(int&) const;
};

}

#endif

