// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008/2009/2010 Christian Schlegel, Alex Lotz
//
//        schlegel@hs-ulm.de
//        lotz@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
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

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <list>

#include "smartComponent.hh"
#include "smartCommState.hh"
#include "smartWiring.hh"
#include "smartQuery.hh"
#include "smartTask.hh"


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
    /// mutexConnection protects critical sections from being executed in
    ///                 parallel. These are all related to changing connections
    ///                 while client/server interactions are active. The mutex
    ///                 avoids racing conditions when for example a subscribe
    ///                 and a disconnect are called in parallel.
    SmartRecursiveMutex mutexConnection;

    // management class of the component
    SmartComponent *component;

    //<alexej date="2009-10-27">
    /// Main Query port - handles all communication of state pattern
    QueryClient<SmartCommStateRequest, SmartCommStateResponse> state_proxy;
    //</alexej>

    /// 0/1 to indicate whether client is not connected / is connected to server
    int statusConnected;

    /// 0/1 to indicate whether port is not contained / is contained in the set
    /// of externally accessible client wiring
    int statusManagedPort;

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// stores pointer to wiring slave when managed port
    WiringSlave *wiringslave;

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    SmartStateClient() throw(CHS::SmartError);

    // Default constructor
    /** (Interface used by wiring service. Requires ordinary pointer since
     *   client patterns have different types and even have different template
     *   parameters. Therefore member function pointers in wiring service would
     *   require type information. For further details on return status see connect()
     *   and disconnect() methods.)
     *
     *   internalConnect @return status code
     *     - SMART_OK                  : everything is OK
     *     - SMART_SERVICEUNAVAILABLE  : the requested server/service is not available and therefore
     *                                   no connection can be established.
     *     - SMART_INCOMPATIBLESERVICE : the denoted service is incompatible (wrong communication
     *                                   pattern or wrong communication objects) and can therefore
     *                                   not be connected. Client is now not connected to any server.
     *     - SMART_ERROR               : something went wrong
     *
     *   internalDisconnect @return status code
     *     - SMART_OK                  : everything is OK
     *     - SMART_ERROR_COMMUNICATION : something went wrong
     */
    static StatusCode internalConnect(void* ptr, const std::string& server, const std::string& service);
    static StatusCode internalDisconnect(void* ptr);

  public:
    // Initialization
    /** Constructor (exposed as port and wireable from outside by other components).
     *  add()/remove() and connect()/disconnect() can always be used to change the
     *  status of the instance. Instance is not connected to a service provider.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use, instance not created
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param port       name of the wireable port
     * @param slave      wiring slave of this component
     */
    SmartStateClient(SmartComponent* component, const std::string& port, WiringSlave* slave) throw(CHS::SmartError);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    SmartStateClient(SmartComponent* component) throw(CHS::SmartError);

    /** Constructor (wired with specified service provider).
     *  Connects to the denoted service and blocks until the connection
     *  has been established. Blocks infinitely if denoted service becomes
     *  not available since constructor performs retries. Blocking is useful to
     *  simplify startup of components which have mutual dependencies.
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance.
     *
     *  Throws exception if denoted service is incompatible (wrong communication
     *  pattern or wrong communication objects).
     *    - SMART_INCOMPATIBLESERVICE : the denoted service is incompatible (wrong communication
     *                                  pattern or wrong communication objects) and can therefore
     *                                  not be connected. Instance is not created.
     *    - SMART_ERROR               : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param server     name of the server
     * @param service    name of the service
     */
    SmartStateClient(SmartComponent* component, const std::string& server, const std::string& service="state") throw(CHS::SmartError);

    /// Destructor
    virtual ~SmartStateClient() throw();

   /** Add this instance to the set of ports wireable via the
     *  wiring pattern from outside the component. Already
     *  established connections keep valid. If this service
     *  requestor is already exposed as port, it is first
     *  removed and then added with the new port name.
     *  add()/remove() and connect()/disconnect() can always
     *  be used to change the status of this instance.
     *
     *  @param slave  wiring slave of this component
     *  @param port   name of port used for wiring
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and this instance
     *                                 added to the set of ports wireable
     *                                 from outside the component
     *   - SMART_PORTALREADYUSED     : port name already in use and this
     *                                 instance now not available as port
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode add(WiringSlave* slave, const std::string& port) throw();

    /** Remove this service requestor from the set of ports wireable
     *  via the wiring pattern from outside the component. Already
     *  established connections keep valid but can now be changed
     *  only from inside and not from outside this component anymore.
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and instance not
     *                                 exposed as port anymore (or was not
     *                                 registered as port).
     *   - SMART_ERROR               : something went wrong but this instance
     *                                 is removed from the set of ports in
     *                                 any case.
     */
    StatusCode remove() throw();

    /** Connect this service requestor to the denoted service provider. An
     *  already established connection is first disconnected. See disconnect()
     *  for implications on pending data reception in that case.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to getUpdate() / getUpdateWait().
     *
     *  @param server   name of the server
     *  @param service  name of the service
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and connected to the specified service.
     *   - SMART_SERVICEUNAVAILABLE  : the specified service is currently not available and the
     *                                 requested connection can not be established. Service
     *                                 requestor is now not connected to any service provider.
     *   - SMART_INCOMPATIBLESERVICE : the specified service provider is not compatible (wrong communication
     *                                 pattern or wrong communication objects) to this service requestor and
     *                                 can therefore not be connected. Service requestor is now not connected
     *                                 to any service provider.
     *   - SMART_ERROR_COMMUNICATION : communication problems, service requestor is now not connected to any
     *                                 service provider.
     *   - SMART_ERROR               : something went wrong, service requestor is now not connected to any
     *                                 service provider.
     */
    StatusCode connect(const std::string& server, const std::string& service="state") throw();

    /** Disconnect the service requestor from the service provider.
     *
     *  A disconnect always first performs an unsubscribe. See unsubsribe() for
     *  implications on getUpdate() and getUpdateWait().
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to getUpdate() / getUpdateWait().
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and service requestor is disconnected from
     *                                 the service provider.
     *   - SMART_ERROR_COMMUNICATION : something went wrong at the level of the intercomponent
     *                                 communication. At least the service requestor is in the
     *                                 disconnected state irrespective of the service provider
     *                                 side clean up procedures.
     *   - SMART_ERROR               : something went wrong. Again at least the service requestor
     *                                 is in the disconnected state.
     */
    StatusCode disconnect() throw();

    /** Allow or abort and reject blocking calls.
     *
     *  If blocking is set to false all blocking calls return with SMART_CANCELLED. This can be
     *  used to abort blocking calls.
     *
     *  @param blocking  true/false
     *
     *  @return status code
     *   - SMART_OK                  : new mode set
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode blocking(const bool b) throw();

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
    virtual ~StateChangeHandler() {  };

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

  /// forward declaration
  class SmartStateServer;


  /** @internal
   *  Handler for slave part of wiring pattern.
   *
   *  The wiring handler is called by the internally used query pattern
   *  and connects / disconnects a port with a server.
   */
  class StateServerHandler : public QueryServerHandler<SmartCommStateRequest, SmartCommStateResponse>
  {
  private:
    /// used to access the SmartStateServer from the handler
    SmartStateServer * stateServer;

    /// default constructor
    StateServerHandler() throw(SmartError);

  public:
    /** Constructor.
     *
     * @param slave  <I>SmartStateServer</I> needed to access it from the handler
     */
    StateServerHandler(SmartStateServer* state) throw();

    /// Destructor
    virtual ~StateServerHandler() throw();

      virtual void handleQuery(QueryServer<SmartCommStateRequest,SmartCommStateResponse>& server, 
         const QueryId id, const SmartCommStateRequest& request) throw();
  };

  /** Server part of state pattern (slave)
   *
   *  Demonstrated in @ref example3
   */
  class SmartStateServer {
     friend class StateServerHandler;

  private:

    typedef enum SmartStateServerAction {
      SSA_UNDEFINED,
      SSA_CHANGE_STATE
    }SmartStateServerAction;

    typedef struct SmartStateEntry {
      SmartStateServerAction      action;
      std::string                 state;
      //<alexej date="2009-10-27">
      QueryServer<SmartCommStateRequest,SmartCommStateResponse> *server_proxy;
      QueryId                     qid;
      //</alexej>
    }SmartStateEntry;

    //
    // indicates the next action to be performed on a substate
    //
    typedef enum SmartStateAction {
      STATE_ACTION_ACTIVATE,
      STATE_ACTION_DEACTIVATE,
      STATE_ACTION_NONE
    }SmartStateAction;

    //
    // indicates the current mode of a substate
    //
    typedef enum SmartStateMode {
      STATE_ACTIVATED,
      STATE_DEACTIVATED
    }SmartStateMode;

    //
    // used to manage the states of a component
    //
    typedef struct SmartSubStateEntry {
      std::list<std::string>  mainstates;
      std::string             name;
      //<alexej date="2010-09-08">
      //SmartCondClass        cond;
      SmartConditionMutex     *cond;
      //</alexej>
      int                     cnt;
      SmartStateAction        action;
      SmartStateMode          state;
    }SmartSubStateEntry;


    StateUpdateThread                    stateUpdateThread;
    SmartSemaphore*                      stateQueueSemaphore;
    ACE_Unbounded_Queue<SmartStateEntry> stateQueue;

    SmartStateEntry  desiredState;
    std::string      currentState;

    SmartMutex  mutex;

    ///
    std::list<SmartSubStateEntry> stateList;

    /// if flag is true => no more states can be defined
    bool running;

    // management class of the service
    SmartComponent *component;

    /// service name
    std::string service;

    /// handler for state changes
    StateChangeHandler & changeHandler;

    /// QueryHandler handles incomming state-requests from StateClient
    StateServerHandler query_handler;

    /// QueryServer as main port of StateServer
    QueryServer<SmartCommStateRequest, SmartCommStateResponse> query_server;


    /// private handler functions
    static void hndSetMainState(void*, QueryServer<SmartCommStateRequest,SmartCommStateResponse> *server, const QueryId &qid, const std::string&);
    static StatusCode hndGetMainStates(void*, std::list<std::string>&);
    static StatusCode hndGetSubStates(void*, const std::string&, std::list<std::string>&);

    /// Default constructor
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
    SmartStateServer(SmartComponent* component, StateChangeHandler & hnd) throw(SmartError);

    /** Constructor.
     *  @param component management class of the component
     *  @param service   service name
     *  @param hnd       notify this handle object when state changes occur
     */
    SmartStateServer(SmartComponent* component, const std::string& service, StateChangeHandler & hnd) throw(SmartError);

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
    StatusCode defineStates(const std::string& mainstate, const std::string& substate) throw();

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

