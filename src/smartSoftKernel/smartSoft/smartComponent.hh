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

#ifndef _SMARTCOMPONENT_HH
#define _SMARTCOMPONENT_HH

#include <string>
#include <stdexcept>

#include <tao/ORB_Core.h>

#include "smartSys.hh"
#include "smartErrno.hh"
#include "smartTimerThread.hh"

/////////////////////////////////////////////////////////////////////////
//
// additional functions / classes
//
/////////////////////////////////////////////////////////////////////////

namespace CHS {

  /** @internal
   *  Condition variable with memory / more or less a semaphore which does
   *  not count.
   *
   *  The condition variable is used to avoid active waiting. It uses an
   *  internal state to indicate whether the condition variable has been
   *  signaled already (active state) and then returns immediately
   *  resetting the internal state (passive state). Can not be used to
   *  broadcast a signal since multiple signals are summarized in the
   *  active state which is consumed by the first call to the wait-method.
   *
   *  Within the communication patterns there is always one producer
   *  and one consumer of a signal. Used to handle methods like queryReceive
   *  where one has to know whether the expected answer has already been
   *  received before invoking the queryReceive method (if not yet received
   *  block until reception of answer).
   */
  class SmartCntCondClass {
  private:
    int                  v;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCntCondClass();

    /// destructor
    ~SmartCntCondClass();

    /// copy constructor
    SmartCntCondClass(const SmartCntCondClass&);

    /// copy assignment
    SmartCntCondClass& operator=(const SmartCntCondClass&);

    ///
    void signal(void);

    ///
    void wait(void);

    ///
    int test(void);
  };
}

namespace CHS {

  /** @internal condition like variable class.
   *
   *  The condition variable is used to avoid active waiting. In
   *  contrast to the SmartCntCondClass version, we really wait until
   *  the next signal is received and we do not store an already
   *  received signal. "signalAll" e.g. is used within clients of the
   *  push service to release every waiting thread as soon as an
   *  update is received.
   */
  class SmartCondClass {
  private:
    int                  v;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCondClass();

    /// destructor
    ~SmartCondClass();

    /// copy constructor
    SmartCondClass(const SmartCondClass&);

    /// copy assignment
    SmartCondClass& operator=(const SmartCondClass&);

    ///
    int signalOne(void);

    ///
    int signalAll(void);

    ///
    int wait(void);
  };
}





namespace CHS {

  /** @internal 
   *  Condition variable without memory
   *
   *  The condition variable is used to avoid active waiting. The condition
   *  variable has no memory and does not know whether it has been signalled 
   *  before or not. It is therefore used for example within the pushNewestClient
   *  to wait for the next arriving update.
   *
   *  Condition variable discriminates between blockingPattern and blockingUser.
   *  Both can enforce non-blocking. The pattern uses the non-blocking mode to
   *  abort blocking waits when getting disconnected for example. This is used 
   *  to avoid blocking calls for new updates if for example an unsubscribe is 
   *  executed in parallel to a blocking call to "getUpdateWait" in the 
   *  pushNewestClient. Please look there for further details. The user can
   *  enforce non-blocking mode to abort blocking waits for example to continue
   *  with processing without awaiting the answer.
   *
   *  One could also implement a counting blocking indicator with acquire/release
   *  member functions. It is however less error prone from the user view not
   *  to be forced to have a matching number of acquire/release calls and we
   *  therefore implemented separate flags for the pattern internal use and
   *  the user.
   *
   *  The wait() member function first checks the blocking flags. It returns
   *  immediately with SMART_CANCELLED if at least one flag enforces non-blocking
   *  mode. Otherwise it waits for either a signal or for being cancelled. If
   *  the condition variable is signalled, it returns with SMART_OK.
   */
  class SmartCVwithoutMemory {
  private:
    bool                 statusBlockingComponent;
    bool                 statusBlockingPattern;
    bool                 statusBlockingUser;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCVwithoutMemory();

    /// destructor
    ~SmartCVwithoutMemory();

    /// copy constructor
    SmartCVwithoutMemory(const SmartCVwithoutMemory&);

    /// copy assignment
    SmartCVwithoutMemory& operator=(const SmartCVwithoutMemory&);

    ///
    void blockingUser(const bool);

    ///
    void blockingPattern(const bool);

    ///
    void blockingComponent(const bool);

    ///
    void signal();

    ///
    CHS::StatusCode wait();
  };
}

namespace CHS {

  /** @internal
   *  Condition variable with memory
   *
   *  The condition variable is used to avoid active waiting. It memorizes
   *  whether the condition variable has been signalled since the last
   *  wait. It is used within the queryPattern to memorize whether an
   *  answer has been received for the pending request. 
   *
   *  Using this condition variable makes sure that no signal and therefore
   *  no answer is missed. One otherwise would eventually block infinitely
   *  on a condition variable waiting for the next signal which will never
   *  be given since each request has exactly one response.
   *
   *  Condition variable discriminates between blockingPattern and blockingUser.
   *  Both can enforce non-blocking. The pattern uses the non-blocking mode to
   *  abort blocking waits when getting disconnected for example. The user can
   *  enforce non-blocking mode to abort blocking waits to for example continue
   *  with processing without awaiting the answer. 
   *
   *  One could also implement a counting blocking indicator with acquire/release 
   *  member functions. It is however less error prone from the user view not
   *  to be forced to have a matching number of acquire/release calls and we
   *  therefore implemented separate flags for the pattern internal use and
   *  the user.
   *
   *  Signalling this condition variable sets the signalled flag. The wait()
   *  member function returns immediately with SMART_OK in case the signalled 
   *  flag is set and consumes it. If it has not been signalled so far, it 
   *  checks the blocking flags. It returns immediately with SMART_CANCELLED 
   *  in case of at least one flag enforces non-blocking mode. Otherwise, it 
   *  waits for either a signal or for being cancelled.
   *
   */
  class SmartCVwithMemory {
  private:
    bool                 signalled;
    bool                 statusBlockingComponent;
    bool                 statusBlockingPattern;
    bool                 statusBlockingUser;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCVwithMemory();

    /// destructor
    ~SmartCVwithMemory();

    /// copy constructor
    SmartCVwithMemory(const SmartCVwithMemory&);

    /// copy assignment
    SmartCVwithMemory& operator=(const SmartCVwithMemory&);

    ///
    void blockingUser(const bool);

    ///
    void blockingPattern(const bool);

    ///
    void blockingComponent(const bool);

    ///
    void signal();

    ///
    CHS::StatusCode wait();

    ///
    void reset();
  };
}






/////////////////////////////////////////////////////////////////////////
//
// SmartSoft component class
//
/////////////////////////////////////////////////////////////////////////

namespace CHS {
  
  /** Component management class
   *
   *  Every component needs exactly one component management class which
   *  wraps basic services of the SmartSoft framework. This infrastructure
   *  is for example needed by the communication patterns.
   *
   *  The main thread of a component is used by this class to run the
   *  SmartSoft framework. Therefore all user activities must be started
   *  before in separate threads. Further details can be found in the
   *  examples.
   *
   */
  class SmartComponent : public ACE_Event_Handler {
  private:
    typedef struct CondListStruct {                  // old ...
      SmartCondClass        *cond;
      struct CondListStruct *next;
    };
    typedef struct CntCondListStruct {               // old ...
      SmartCntCondClass        *cond;
      struct CntCondListStruct *next;
    };

    typedef struct SmartCVwithMemoryStruct {
      SmartCVwithMemory              *cond;
      struct SmartCVwithMemoryStruct *next;
    };
    typedef struct SmartCVwithoutMemoryStruct {
      SmartCVwithoutMemory              *cond;
      struct SmartCVwithoutMemoryStruct *next;
    };

    /// indicates whether blocking is not allowed in whole component
    bool statusComponentBlocking;

    CondListStruct             *condList;
    CntCondListStruct          *cntCondList;
    SmartCVwithMemoryStruct    *listCVwithMemory;
    SmartCVwithoutMemoryStruct *listCVwithoutMemory;
    bool                       condCancel;
    int                        msgCnt;
    int                        hndCnt;
    std::string                componentName;
    SmartMutex                 mutex;

  protected:

    /** Signal handler method for SIGINT.
     *  The Default constructor registers this method to SIGINT.
     *
     *  This method calls exit(0) on SIGINT, resulting in a
     *  uncontrolled shutdown, no destructors will be called etc.
     *
     *  Overriding this method in a subclass is recommended to enable
     *  a clean shutdown after a Ctrl-C.
     *
     *  \warning this code runs in a signal handler 
     *    (see http://www.cs.wustl.edu/~schmidt/signal-patterns.html for the approach)
     */
    virtual int handle_signal (int signum, siginfo_t *, ucontext_t *);

  protected:
    // the components timer thread
    TimerThread timerThread;

  public:
    CORBA::ORB_var                 orb;
    CORBA::Object_var              obj;
    PortableServer::POA_var        poa;
    PortableServer::POAManager_var mgr;

    CosNaming::NamingContext_var   namingService;

    // = Initialization

    /** Initializes the component.
     *
     *   @param server       unique name of the whole component, which is used by the clients to
     *                       address this server
     *   @param argc         # of command line arguments
     *   @param argv         array of command line arguments
     *   @param compileDate  compile date, just for debugging, don't touch
     *   @param compileTime  compile time, just for debugging, don't touch
     *
     */
    SmartComponent( const std::string &server,
                    int & argc,
                    char ** argv,
                    const char *compileDate = __DATE__ ,
                    const char *compileTime = __TIME__  );

    /** Destructor.
     */
    virtual ~SmartComponent();

    /** Runs the SmartSoft framework within a component which includes handling
     *  intercomponent communication etc. This method is called in the main()-routine
     *  of a component after all initializations including activation of user threads
     *  are finished.
     *
     *  @return status code
     *    - SMART_ERROR_UNKNOWN: unknown error (probably a corba problem)
     *    - SMART_OK: gracefully terminated
     */
    StatusCode run(void);

    /** returns the timer thread of this component.
     *
     *  It can be used to register timer handlers (callbacks) and schedule
     *  them for single or periodic calls.
     *
     *  The handlers added here should be fast and nonblocking, since this
     *  TimerThread is also used internally.
     *
     *  A handler that just signal a working thread should be used if the handler
     *  is computationally expensive or uses blocking functions.
     */
    TimerThread & getTimerThread()
      { return timerThread; };

    /** Allow or abort and reject blocking calls in communication patterns of this component.
     *
     *  if blocking is set to false all blocking calls of all communication patterns
     *  of this component return with SMART_CANCELLED. This can be used to abort blocking
     *  calls of ALL communication patterns inside a component.
     *
     *  @param blocking  true/false 
     *
     *  @return status code
     *    - SMART_OK    : new mode set
     *    - SMART_ERROR : something went wrong
     */
    StatusCode blocking(const bool b) throw();
   
    /** returns the name of the component (used internally)
     *
     */
    std::string getComponentName(void) throw();

    /** @internal
     *  called by the communication patterns to add their condition variable to the
     *  component. Used to set the cancelled state for all communication patterns of
     *  a component by a single call to the blocking() member function of this class.
     *  This is for example used by the state pattern.
     */
    CHS::StatusCode addCV(SmartCVwithMemory* cond) throw();

    /** @internal
     *  see above
     */
    CHS::StatusCode removeCV(SmartCVwithMemory* cond) throw();

    /** @internal
     *  see above
     */
    CHS::StatusCode addCV(SmartCVwithoutMemory* cond) throw();

    /** @internal
     *  see above
     */
    CHS::StatusCode removeCV(SmartCVwithoutMemory* cond) throw();

    


    /** @internal
     *  provide interruptable wait on condition variable.
     */
    StatusCode waitForCond( SmartCondClass& cond );

    /** @internal
     *  provide interruptable wait on condition variable.
     */
    StatusCode waitForCond( SmartCntCondClass& cond );
  };
}

#endif

