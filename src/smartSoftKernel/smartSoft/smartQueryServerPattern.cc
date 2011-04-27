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

#include "smartQueryServerPattern.hh"

CHS::QueryServer_impl::QueryServer_impl(void            *ptr,
                                        void            (*callbackRqst)(void *, const CORBA::Any &, SmartQueryClientPattern_ptr, CORBA::Long),
                                        CHS::StatusCode (*callbackCnct)(void *, SmartQueryClientPattern_ptr),
                                        void            (*callbackDisc)(void *, SmartQueryClientPattern_ptr))
{
  hndRqstPtr = callbackRqst;
  hndCnctPtr = callbackCnct;
  hndDiscPtr = callbackDisc;
  lthis      = ptr;
};

void CHS::QueryServer_impl::request(const CORBA::Any & user, SmartQueryClientPattern_ptr client, CORBA::Long id) throw (CORBA::SystemException)
{
  hndRqstPtr(lthis,user,client,id);
};

int CHS::QueryServer_impl::connect(SmartQueryClientPattern_ptr client) throw (CORBA::SystemException)
{
  return hndCnctPtr(lthis,client);
};

void CHS::QueryServer_impl::disconnect(SmartQueryClientPattern_ptr client) throw (CORBA::SystemException)
{
  hndDiscPtr(lthis,client);
};

