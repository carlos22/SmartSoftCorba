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

#include "smartEventServerPattern.hh"

CHS::EventServer_impl::EventServer_impl(void *ptr,
                                        void (*callbackActivate)(void *, SmartEventClientPattern_ptr, CORBA::Long, CORBA::Long, const CORBA::Any &),
                                        void (*callbackDeactivate)(void *, SmartEventClientPattern_ptr, CORBA::Long),
                                        CHS::StatusCode (*callbackCnct)(void *, SmartEventClientPattern_ptr),
                                        void            (*callbackDisc)(void *, SmartEventClientPattern_ptr))
{
  hndActivatePtr   = callbackActivate;
  hndDeactivatePtr = callbackDeactivate;
  hndCnctPtr       = callbackCnct;
  hndDiscPtr       = callbackDisc;
  lthis            = ptr;
};

void CHS::EventServer_impl::activate(SmartEventClientPattern_ptr client,CORBA::Long mode,CORBA::Long id, const CORBA::Any &parameter) throw (CORBA::SystemException)
{
  hndActivatePtr(lthis,client,mode,id,parameter);
};

void CHS::EventServer_impl::deactivate(SmartEventClientPattern_ptr client,CORBA::Long id) throw (CORBA::SystemException)
{
  hndDeactivatePtr(lthis,client,id);
};

int CHS::EventServer_impl::connect(SmartEventClientPattern_ptr client) throw (CORBA::SystemException)
{
  return hndCnctPtr(lthis,client);
};

void CHS::EventServer_impl::disconnect(SmartEventClientPattern_ptr client) throw (CORBA::SystemException)
{
  hndDiscPtr(lthis,client);
};

