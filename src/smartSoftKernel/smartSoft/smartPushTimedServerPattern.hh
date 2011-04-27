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
#include "smartPushTimedPatternS.hh"

namespace CHS {
  class PushTimedServer_impl : public virtual POA_SmartPushTimedServerPattern
  {
  private:
    void (*hndSubscribePtr)(void *, SmartPushTimedClientPattern_ptr, int, int&);
    void (*hndUnsubscribePtr)(void *, SmartPushTimedClientPattern_ptr);
    CHS::StatusCode (*hndCnctPtr)(void *, SmartPushTimedClientPattern_ptr);
    void (*hndDiscPtr)(void *, SmartPushTimedClientPattern_ptr);
    void (*hndGetServerInfoPtr)(void *, double&, int&);
    void *lthis;

  public:
    PushTimedServer_impl(void *,
                         void (*)(void *, SmartPushTimedClientPattern_ptr, int, int&),
                         void (*)(void *, SmartPushTimedClientPattern_ptr),
                         CHS::StatusCode (*)(void *, SmartPushTimedClientPattern_ptr),
                         void (*)(void *, SmartPushTimedClientPattern_ptr),
                         void (*)(void *, double&, int&));

    virtual void subscribe (SmartPushTimedClientPattern_ptr,int,int&) throw (CORBA::SystemException);
    virtual void unsubscribe (SmartPushTimedClientPattern_ptr) throw (CORBA::SystemException);
    virtual int  connect (SmartPushTimedClientPattern_ptr) throw (CORBA::SystemException);
    virtual void disconnect (SmartPushTimedClientPattern_ptr) throw (CORBA::SystemException);
    virtual void getServerInfo (double&,int&) throw (CORBA::SystemException);
  };
}

