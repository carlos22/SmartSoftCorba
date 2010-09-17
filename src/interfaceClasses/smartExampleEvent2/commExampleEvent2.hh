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

#ifndef _COMM_EXAMPLE_EVENT2_HH
#define _COMM_EXAMPLE_EVENT2_HH

#include <iostream>
#include <string>

#include "exampleEvent2C.hh"

namespace Smart {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommExampleEvent2Parameter
{
protected:
  ExampleEvent2Parameter parameter;
  int state;  // 0: unknown, 1: in interval, 2: out of interval

public:
  CommExampleEvent2Parameter();
  virtual ~CommExampleEvent2Parameter();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::exampleEvent2Parameter";
  };

  void set(int,int);          // set lower/upper bound of interval
  void get(int&,int&) const;  // get lower/upper bound of interval
  void set(int);              // set state
  void get(int&) const;       // get state
  void print(std::ostream &os = std::cout) const ;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent2Parameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommExampleEvent2Result
{
protected:
  ExampleEvent2Result result;

public:
  CommExampleEvent2Result();
  virtual ~CommExampleEvent2Result();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::exampleEvent2Result";
  };

  void set(std::string);
  void get(std::string&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent2Result &c) {
  c.print(os);
  return os;
};

}

#endif

