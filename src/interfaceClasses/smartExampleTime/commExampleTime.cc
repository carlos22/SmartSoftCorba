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

#include "commExampleTime.hh"

using namespace CHS;

CommExampleTime::CommExampleTime()
{
}

CommExampleTime::~CommExampleTime()
{
}

void CommExampleTime::get(CORBA::Any &a) const
{
  a <<= time;
}

void CommExampleTime::set(const CORBA::Any &a)
{
  ExampleTime *t;

  a >>= t;

  time = *t;
}

void CommExampleTime::get(int &h,int &m,int &s)
{
  h = time.hour;
  m = time.minute;
  s = time.second;
}

void CommExampleTime::set(int h,int m,int s)
{
  time.hour   = h;
  time.minute = m;
  time.second = s;
}

void CommExampleTime::print(std::ostream &os) const
{
  os << "Time in Greenwich is "
     << std::setw (2) << std::setfill ('0') << time.hour << ":"
     << std::setw (2) << std::setfill ('0') << time.minute << ":"
     << std::setw (2) << std::setfill ('0') << time.second << std::endl;
}

