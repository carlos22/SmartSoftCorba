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
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#include "smartPushTimedServerPattern.hh"

CHS::PushTimedServer_impl::PushTimedServer_impl(void *ptr,
                                                void            (*callbackSubsc)(void *, SmartPushTimedClientPattern_ptr, int, int&),
                                                void            (*callbackUnsub)(void *, SmartPushTimedClientPattern_ptr),
                                                CHS::StatusCode (*callbackCnct) (void *, SmartPushTimedClientPattern_ptr),
                                                void            (*callbackDisc) (void *, SmartPushTimedClientPattern_ptr),
                                                void            (*callbackInfo) (void *, double&, int&))
{
  hndSubscribePtr     = callbackSubsc;
  hndUnsubscribePtr   = callbackUnsub;
  hndCnctPtr          = callbackCnct;
  hndDiscPtr          = callbackDisc;
  hndGetServerInfoPtr = callbackInfo;
  lthis               = ptr;
};

void CHS::PushTimedServer_impl::subscribe(SmartPushTimedClientPattern_ptr client,int n,int& running) throw (CORBA::SystemException)
{
  hndSubscribePtr(lthis,client,n,running);
};

void CHS::PushTimedServer_impl::unsubscribe(SmartPushTimedClientPattern_ptr client) throw (CORBA::SystemException)
{
  hndUnsubscribePtr(lthis,client);
};

int CHS::PushTimedServer_impl::connect(SmartPushTimedClientPattern_ptr client) throw (CORBA::SystemException)
{
  return hndCnctPtr(lthis,client);
};

void CHS::PushTimedServer_impl::disconnect(SmartPushTimedClientPattern_ptr client) throw (CORBA::SystemException)
{
  hndDiscPtr(lthis,client);
};

void CHS::PushTimedServer_impl::getServerInfo(double& cycle, int& running) throw (CORBA::SystemException)
{
  hndGetServerInfoPtr(lthis, cycle, running);
};

