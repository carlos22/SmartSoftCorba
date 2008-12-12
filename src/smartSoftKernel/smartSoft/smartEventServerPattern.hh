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

#include "smartErrno.hh"
#include "smartEventPatternS.hh"

namespace CHS {
  class EventServer_impl : public virtual POA_SmartEventServerPattern
  {
  private:
    void (*hndActivatePtr)(void *, SmartEventClientPattern_ptr, CORBA::Long, CORBA::Long, const CORBA::Any &);
    void (*hndDeactivatePtr)(void *, SmartEventClientPattern_ptr, CORBA::Long);
    CHS::StatusCode (*hndCnctPtr)(void *, SmartEventClientPattern_ptr);
    void (*hndDiscPtr)(void *, SmartEventClientPattern_ptr);
    void *lthis;

  public:
    EventServer_impl(void *,
                     void (*)(void *, SmartEventClientPattern_ptr, CORBA::Long, CORBA::Long, const CORBA::Any &),
                     void (*)(void *, SmartEventClientPattern_ptr, CORBA::Long),
                     CHS::StatusCode (*)(void *, SmartEventClientPattern_ptr),
                     void (*)(void *, SmartEventClientPattern_ptr));

    virtual void activate (SmartEventClientPattern_ptr, CORBA::Long, CORBA::Long, const CORBA::Any &) throw (CORBA::SystemException);
    virtual void deactivate (SmartEventClientPattern_ptr, CORBA::Long) throw (CORBA::SystemException);
    virtual int  connect (SmartEventClientPattern_ptr) throw (CORBA::SystemException);
    virtual void disconnect (SmartEventClientPattern_ptr) throw (CORBA::SystemException);
  };
}

