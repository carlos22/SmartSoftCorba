// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Alex Lotz, Andreas Steck
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
// --------------------------------------------------------------------------

#include "smartCommState.hh"

/////////////////////////////////////////////////////////////////////////
//
// Request
//
/////////////////////////////////////////////////////////////////////////
void CHS::SmartCommStateRequest::get(CORBA::Any &a) const
{
  a <<= smartStateRequest;
}

void CHS::SmartCommStateRequest::set(const CORBA::Any &a)
{
  SmartStateRequest *s;

  a >>= s;

  smartStateRequest = *s;
}



/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////
void CHS::SmartCommStateResponse::get(CORBA::Any &a) const
{
  a <<= smartStateResponse;
}

void CHS::SmartCommStateResponse::set(const CORBA::Any &a)
{
  SmartStateResponse *s;

  a >>= s;

  smartStateResponse = *s;
}

