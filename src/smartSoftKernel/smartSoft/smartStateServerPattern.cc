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

#include "smartStateServerPattern.hh"


CHS::StateServer_impl::StateServer_impl(void *ptr,
                                        void (*callback1)(void *, SmartStateClientPattern_ptr, std::string),
                                        int  (*callback2)(void *, StateList_out),
                                        int  (*callback3)(void *, std::string, StateList_out))
{
  hndSetStatePtr      = callback1;
  hndGetMainStatesPtr = callback2;
  hndGetSubStatesPtr  = callback3;

  lthis = ptr;
}

void CHS::StateServer_impl::setState(const char *mainstate, SmartStateClientPattern_ptr client) throw (CORBA::SystemException)
{
  hndSetStatePtr(lthis,client,mainstate);
}

int CHS::StateServer_impl::getMainStates(StateList_out mainstates) throw (CORBA::SystemException)
{
  return hndGetMainStatesPtr(lthis,mainstates);
}

int CHS::StateServer_impl::getSubStates(const char *mainstate, StateList_out substates) throw (CORBA::SystemException)
{
  return hndGetSubStatesPtr(lthis,mainstate,substates);
}

