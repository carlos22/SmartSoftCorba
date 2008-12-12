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

#ifndef _SMARTTASK_HH
#define _SMARTTASK_HH

#include "smartSys.hh"

namespace CHS {
  /** Wrapper for active objects.
   */
  class SmartTask : public ACE_Task<ACE_MT_SYNCH>
    // = TITLE
  {
  public:
    // = Initialization

    /// Constructor
    SmartTask ();

    /// Destructor
    virtual ~SmartTask();

    // = User interface

    /** Method which runs as separated thread if activated.
     *
     *  The svc() method has to be provided by the user and contains
     *  the activity of the task object.
     */
    virtual int svc (void);

    /** Hook called to open a task.
     *
     *  @param args can be used to pass arbitrary information into open.
     */
    virtual int open (void *args = NULL);

    /** Hook called during thread exit.
     *  In general, this method shouldn't be called directly by an
     *  application, particularly if the task is running as an active
     *  object.
     */
    virtual int close (u_long flags=0);
  };
  
}


#endif // _SMARTTASK_HH

