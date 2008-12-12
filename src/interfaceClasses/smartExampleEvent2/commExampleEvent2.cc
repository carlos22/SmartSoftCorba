// --------------------------------------------------------------------------
//
//  Copyright (C) 2002 Christian Schlegel
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
#include "commExampleEvent2.hh"

using namespace CHS;

CommExampleEvent2Parameter::CommExampleEvent2Parameter() : state(0)
{
}

CommExampleEvent2Parameter::~CommExampleEvent2Parameter()
{
}

void CommExampleEvent2Parameter::get(CORBA::Any &a) const
{
  a <<= parameter;
}

void CommExampleEvent2Parameter::set(const CORBA::Any &a)
{
  ExampleEvent2Parameter *t;

  a >>= t;

  parameter = *t;
}

void CommExampleEvent2Parameter::get(int& s) const
{
  s = state;
}

void CommExampleEvent2Parameter::set(int s)
{
  state = s;
}

void CommExampleEvent2Parameter::set(int l,int u)
{
  parameter.lower = l;
  parameter.upper = u;
}

void CommExampleEvent2Parameter::get(int& l,int& u) const
{
  l = parameter.lower;
  u = parameter.upper;
}

void CommExampleEvent2Parameter::print(std::ostream &os) const
{
  os << "Parameter : (lower, upper) " << parameter.lower << " " << parameter.upper << " state " << state << std::endl;
}




CommExampleEvent2Result::CommExampleEvent2Result()
{
}

CommExampleEvent2Result::~CommExampleEvent2Result()
{
}

void CommExampleEvent2Result::get(CORBA::Any &a) const
{
  a <<= result;
}

void CommExampleEvent2Result::set(const CORBA::Any &a)
{
  ExampleEvent2Result *t;

  a >>= t;

  result = *t;
}

void CommExampleEvent2Result::set(std::string s)
{
  result.state = s.c_str();
}

void CommExampleEvent2Result::get(std::string& s) const
{
  s = result.state;
}

void CommExampleEvent2Result::print(std::ostream &os) const
{
  os << "Result : " << result.state << std::endl;
}

