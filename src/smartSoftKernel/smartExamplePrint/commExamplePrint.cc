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

#include "commExamplePrint.hh"

using namespace Smart;

CommExamplePrint::CommExamplePrint()
{
}

CommExamplePrint::~CommExamplePrint()
{
}

void CommExamplePrint::get(CORBA::Any &a) const
{
  a <<= message;
}

void CommExamplePrint::set(const CORBA::Any &a)
{
  ExamplePrint *t;

  a >>= t;

  message = *t;
}

void CommExamplePrint::get(int &h,int &m,int &s,std::string& t) const
{
  h = message.hour;
  m = message.minute;
  s = message.second;
  t = message.text;
}

void CommExamplePrint::set(int h,int m,int s,std::string t)
{
  message.hour   = h;
  message.minute = m;
  message.second = s;
  message.text   = t.c_str();
}

void CommExamplePrint::print(std::ostream &os) const
{
  os << "Time in Greenwich is "
     << std::setw (2) << std::setfill ('0') << message.hour << ":"
     << std::setw (2) << std::setfill ('0') << message.minute << ":"
     << std::setw (2) << std::setfill ('0') << message.second << " text:" << message.text << std::endl;
}

