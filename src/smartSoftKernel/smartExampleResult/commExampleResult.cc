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

#include "commExampleResult.hh"

using namespace Smart;

CommExampleResult::CommExampleResult()
{
}

CommExampleResult::~CommExampleResult()
{
}

void CommExampleResult::get(CORBA::Any &a) const
{
  a <<= result;
}

void CommExampleResult::set(const CORBA::Any &a)
{
  ExampleResult *t;

  a >>= t;

  result = *t;
}

void CommExampleResult::set(int r)
{
  result.value = r;
}

void CommExampleResult::print(std::ostream &os) const
{
  os << "Result : " << result.value << std::endl;
}

