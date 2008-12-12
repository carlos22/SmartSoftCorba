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

#ifndef _SMARTSTATE_HH
#define _SMARTSTATE_HH

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <list>

#include "smartComponent.hh"
#include "smartTask.hh"
#include "smartStateServerPattern.hh"
#include "smartStateClientPattern.hh"


/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////

//
// client state class
//
namespace CHS {

  /** Client part of state pattern (master).
   *
   * @warning Provides not yet the standard interface for dynamic wiring
   *          consisting of the add / remove / connect / disconnect member
   *          functions and does not yet provide the blocking member function
   *          to abort blocking calls.
   *
   *  The configuration class provides priorized mutexes for protecting
   *  configurations. Normally, a complex robotic system requires a lot of
   *  different components which can not all run at the same time. Depending
   *  on the current task to be executed and depending on the available
   *  ressources, one has to be able to activate and deactivate components
   *  and activities within components. One the one hand deactivation should
   *  be done in such a way that the component can perform cleanup tasks.
   *  On the other hand a component must still be interruptable even if it
   *  is pending in a blocking call because the corresponding server has
   *  already been deactivated.
   *
   *  The configuration class therefore provides mechanisms for graceful
   *  activation and deactivation of sets of activities within components.
   *
   *  As is explained in the specification of the state pattern, one can
   *  activate one mainstate at a time. A mainstate can however comprise
   *  several substates which are all active at the same time. A master
   *  can only set mainstates whereas the slave can only acquire substates.
   *  This allows the simple management of sets of concurrent activities.
   *
   *  For a complete description see the printed documentation and further
   *  details in @ref example3.
   *
   */
  class SmartStateClient {
  private:
    SmartMutex        mutex;
    SmartCntCondClass cond;   // signal when answer to setWaitState is received
    StatusCode result;        // result returned from server

    // management class of the component
    SmartComponent *component;

    // the CORBA client object
    StateClient_impl *corbaClient;

    // the CORBA client reference transferred to server
    SmartStateClientPattern_ptr h;

    // the CORBA server connection
    SmartStateServerPattern_var corbaServant;

    // private functions
    static void hndAnswSetMainState(void*,int);

    SmartStateClient() throw(CHS::SmartError);
    // Default constructor

  public:
    // Initialization

    /** constructor.
     *
     *  @param component management class of the component
     *  @param service   name of the service
     */
    SmartStateClient(SmartComponent * component, const std::string & service) throw(CHS::SmartError);

    /// Destructor
    virtual ~SmartStateClient() throw();

    /** Blocking call to change the main state.
     *
     *  @param state
     *     - If "deactivated" is specified as main state, every blocking
     *       query etc. is aborted with SMART_CANCELLED to reach the
     *       "neutral" state as fast as possible.

     *     - If "neutral" is specified the neutral state will be
     *       reached without aborting pending communication
     *       calls.
     *
     * @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : requested mainstate is unknown and
     *                                 therefore no state change has been
     *                                 performed
     *   - SMART_NOTACTIVATED        : state object of server component not yet
     *                                 activated
     *   - SMART_CANCELLED           : component has been requested to reach
     *                                 neutral state, therefore pending
     *                                 setWaitState has been aborted
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode setWaitState(const std::string& state) throw();

    /** Blocking call which returns the currently active main state.
     *
     *  Is only for convenience and is not necessary since the
     *  master always knows what has been configured.
     *
     * @param state is set to the current main state as returned by the server (slave)
     *
     * @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its current state even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    StatusCode getWaitCurrentState(std::string& state) throw();

    /** Blocking call which returns a list of valid main states.
     *
     *  @param states contain all valid main states of the server (slave)
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its list of mainstates even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    StatusCode getWaitMainStates(std::list<std::string>& states) throw();

    /** Blocking call which returns a list of substates assigned to the
     *  specified main state.
     *
     *  @param mainstate is the mainstate to which we want to get the list
     *                   of substates
     *  @param states    is set to the returned list of substates
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_UNKNOWNSTATE        : the main state asked for is unknown
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its list of mainstates even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    StatusCode getWaitSubStates(const std::string& mainstate,std::list<std::string>& states) throw();
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// forward declaration
//
namespace CHS {
  class SmartStateServer;
}

//
// separate thread needed by server to avoid blocking when state changes have to
// be delayed due to user locks
//
namespace CHS {
  class StateUpdateThread : public CHS::SmartTask
  {
  private:
    SmartStateServer *stateServer;
    SmartSemaphore   *stateQueueSemaphore;

  public:
    StateUpdateThread() {};
    ~StateUpdateThread() {};
    void init(SmartStateServer*, SmartSemaphore*);
    int svc(void);
  };
}

//
// server state class
//
namespace CHS {

  /** Handler class for state changes at the slave
   *
   *  Used by SmartStateServer to notify the user about
   *  state changes.
   */
  class StateChangeHandler
  {
  public:
    // Handler methods
    /** Called when a substate is entered
     *
     *  @param state name of newly entered substate
     */
    virtual void handleEnterState(const std::string& state ) throw() = 0;

    /** called when a substate is left
     *
     *  @param state name of deactivated substate
     */
    virtual void handleQuitState(const std::string& state) throw() = 0;
  };

  /** Server part of state pattern (slave)
   *
   *  Demonstrated in @ref example3
   */
  class SmartStateServer {

  private:

    typedef enum SmartStateServerAction {
      SSA_UNDEFINED,
      SSA_CHANGE_STATE
    };

    typedef struct SmartStateEntry {
      SmartStateServerAction      action;
      std::string                 state;
      SmartStateClientPattern_ptr client;
    };

    //
    // indicates the next action to be performed on a substate
    //
    typedef enum SmartStateAction {
      STATE_ACTION_ACTIVATE,
      STATE_ACTION_DEACTIVATE,
      STATE_ACTION_NONE
    };

    //
    // indicates the current mode of a substate
    //
    typedef enum SmartStateMode {
      STATE_ACTIVATED,
      STATE_DEACTIVATED
    };

    //
    // used to manage the states of a component
    //
    typedef struct SmartSubStateEntry {
      std::list<std::string>  mainstates;
      std::string             name;
      SmartCondClass          cond;
      int                     cnt;
      SmartStateAction        action;
      SmartStateMode          state;
    };


    StateUpdateThread                    stateUpdateThread;
    SmartSemaphore*                      stateQueueSemaphore;
    ACE_Unbounded_Queue<SmartStateEntry> stateQueue;

    SmartStateEntry  desiredState;
    std::string      currentState;

    SmartMutex  mutex;

    //
    std::list<SmartSubStateEntry> stateList;

    // if flag is true => no more states can be defined
    bool running;

    // management class of the service
    SmartComponent *component;

    //
    std::string service;

    // handler for state changes
    StateChangeHandler & changeHandler;

    // the CORBA server object
    StateServer_impl *corbaServant;

    // private functions
    static void hndSetMainState(void*, const SmartStateClientPattern_ptr, std::string);
    static int hndGetMainStates(void*, StateList_out);
    static int hndGetSubStates(void*, std::string, StateList_out);

    // Default constructor
    SmartStateServer() throw(SmartError);

  public:
    // internal use only
    void updateState(void);
    void updateStateFromThread(void);

    // Initialization

    /** Constructor.
     *  @param component management class of the component
     *  @param hnd       notify this handle object when state changes occur
     */
    SmartStateServer(SmartComponent* component,StateChangeHandler & hnd) throw(SmartError);

    /// Destructor
    virtual ~SmartStateServer() throw();

    // User interface

    /** Add the substate to the mainstate.
     *
     *  As long as the state object within a component is not
     *  activated, one can configure the name of the main states
     *  and substates which are afterwards visible externally.
     *
     *  If one or both names are used the first time the
     *  corresponding states are created automatically.
     *
     *  @param main is the mainstate
     *  @param sub  is the substate to be assigned to the mainstate
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_ACTIVATED           : configuration can not be changed
     *                                 anymore once state object
     *                                 has been activated
     *   - SMART_NOTALLOWED          : tried to assign substates to the
     *                                 neutral state or something similar
     *                                 which is not allowed
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode defineStates(const std::string& main,const std::string& sub) throw();

    /** Activation is necessary since otherwise no states can be set
     *  or acquired. No more state definitions are possible after
     *  activation.
     *
     *  @return status code
     *    - SMART_OK  : configuration has been activated and no more
     *                  state definitions possible
     */
    StatusCode activate() throw();

    /** Wait until specified substate is available and acquire it.
     *
     *  (hint: acquire(nonneutral) blocks until some main state is set.)
     *
     *  @param sub is the name of the substate to be acquired.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode acquire(const std::string& sub) throw();

    /** Acquire specified substate if available, otherwise return immediately.
     *
     * (hint: tryAcquire(nonneutral) can be used e.g. in a query
     *        handler to decide whether to process the query or to
     *        immediately return an appropriate status code indicating the
     *        components inactivity.)
     *
     *  @param sub is the name of the substate to be acquired.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR : something went wrong
     */
    StatusCode tryAcquire(const std::string& sub) throw();

    /** Release specified substate.
     *
     *  @param sub is the name of the substate to be released.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode release(const std::string& sub) throw();
  };
}

#endif    // _SMARTSTATE_HH

