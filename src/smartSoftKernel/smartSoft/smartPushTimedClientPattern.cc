// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004 Christian Schlegel
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
//  (partly based on joint work with Robert W�rz)
//
// --------------------------------------------------------------------------

#include "smartPushTimedClientPattern.hh"

CHS::PushTimedClient_impl::PushTimedClient_impl(void *ptr,
                                                void (*callbackUpdate)(void *,const CORBA::Any &),
                                                void (*callbackDisconnect)(void *),
                                                void (*callbackStateChange)(void *,int &))
{
  hndUpdatePtr = callbackUpdate;
  hndDiscPtr   = callbackDisconnect;
  hndStatePtr  = callbackStateChange;
  lthis        = ptr;
};

void CHS::PushTimedClient_impl::update ( const CORBA::Any & user) throw (CORBA::SystemException)
{
  hndUpdatePtr(lthis,user);
};

void CHS::PushTimedClient_impl::serverInitiatedDisconnect(void) throw (CORBA::SystemException)
{
  hndDiscPtr(lthis);
};

void CHS::PushTimedClient_impl::serverStateChange(int state) throw (CORBA::SystemException)
{
  hndStatePtr(lthis,state);
};

