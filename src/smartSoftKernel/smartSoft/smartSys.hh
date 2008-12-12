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

#ifndef _SMARTSYS_HH
#define _SMARTSYS_HH

#include <iostream>
#include <iomanip>

#include "ace/ACE.h"
#include "ace/Reactor.h"
#include "ace/Synch.h"
#include "ace/Synch_T.h"
#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/Shared_Memory_SV.h"
#include "ace/Token.h"
#include "ace/Task.h"
#include "ace/OS_NS_time.h"

#include "tao/corba.h"
#include "tao/PortableServer/PortableServer.h"
#include "orbsvcs/CosNamingC.h"

namespace CHS {
  /// typedef of ACE class
  typedef ACE_Thread_Mutex SmartMutex;
  /// typedef of ACE class
  typedef ACE_RW_Thread_Mutex SmartRWMutex;
  /// typedef of ACE class
  typedef ACE_Recursive_Thread_Mutex SmartRecursiveMutex;
  /// typedef of ACE class
  typedef ACE_Semaphore SmartSemaphore;

  /// typedef of ACE class
  typedef ACE_Guard<ACE_Thread_Mutex> SmartGuard;
  /// typedef of ACE class
  typedef ACE_Read_Guard<ACE_RW_Thread_Mutex> SmartReadGuard;
  /// typedef of ACE class
  typedef ACE_Write_Guard<ACE_RW_Thread_Mutex> SmartWriteGuard;
  /// typedef of ACE class
  typedef ACE_Condition_Thread_Mutex SmartConditionMutex;

  // basic thread management
  /// typedef of ACE class
  typedef ACE_Thread_Manager SmartThreadManager;
  /// typedef of ACE class
  typedef ACE_Thread_Control SmartThreadControl;
  /// typedef of ACE class
  typedef ACE_Thread SmartThread;
  /// typedef of ACE class
  typedef ACE_THR_FUNC SMART_THR_FUNC;

  /// typedef of ACE class
  typedef ACE_thread_t SmartThreadType;
  /// typedef of ACE class
  typedef ACE_hthread_t SmartHThreadType;

  //
  // regular stuff
  //
  template<class T> typename T::_ptr_type resolve_init(CORBA::ORB_ptr orb,const char *id);
  template<class T> typename T::_ptr_type resolve_name(CosNaming::NamingContext_ptr nc, const CosNaming::Name & name);

  //
  // misc utility functions
  //
  
  /// easy printing of Time_Values
  std::ostream & operator<<(std::ostream & o, const ACE_Time_Value & t);

  // easy printing of TAO CORBA exceptions
  std::ostream & operator<<(std::ostream & o, const CORBA::Exception & e);
}

#include "smartSys.th"

#endif // _SMARTSYS_HH

