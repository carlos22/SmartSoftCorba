// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
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
// --------------------------------------------------------------------------

#include "commSpeechInputParameter.hh"

using namespace Smart;

CommSpeechInputParameter::CommSpeechInputParameter()
{
  cmd.tag = 0;
  cmd.parameter1 = "";
}

CommSpeechInputParameter::~CommSpeechInputParameter()
{
}

void CommSpeechInputParameter::get(CORBA::Any &a) const
{
  a <<= cmd;
}

void CommSpeechInputParameter::set(const CORBA::Any &a)
{
  SmartIDL::SpeechInputParameter *tmp_cmd;
  if(a >>= tmp_cmd)
  {
    cmd = *tmp_cmd;
  }
}

