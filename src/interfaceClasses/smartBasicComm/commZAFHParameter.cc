// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------


#include "commZAFHParameter.hh"

using namespace Smart;

CommZAFHParameter::CommZAFHParameter()
{
  cmd.command = 0;
  cmd.parameter1 = 0;
  cmd.parameter2 = 0;
  cmd.parameter3 = 0;
  cmd.parameter4 = 0;
}

CommZAFHParameter::~CommZAFHParameter()
{
}

void CommZAFHParameter::get(CORBA::Any &a) const
{
  a <<= cmd;
}

void CommZAFHParameter::set(const CORBA::Any &a)
{
  SmartIDL::ZAFHParameter *tmp_cmd;
  if(a >>= tmp_cmd)
  {
    cmd = *tmp_cmd;
  }
}

