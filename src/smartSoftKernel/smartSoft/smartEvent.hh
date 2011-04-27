// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2010 Christian Schlegel
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

#ifndef _SMARTEVENT_HH
#define _SMARTEVENT_HH

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include "smartComponent.hh"
#include "smartEventServerPattern.hh"
#include "smartEventClientPattern.hh"

//
// needed for the add() / remove() interface for wiring
//
#include "smartCommWiring.hh"

namespace CHS {
  class WiringSlave;
}

namespace CHS {
  typedef int EventId;

  /// Mode of event
  enum EventMode {
    single,      ///< fire event only once.
    continuous   ///< event fires whenever the condition is met.
  };
}

/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////


//
// client event handler object template
//
namespace CHS {

  /** Handler class to process fired events asynchronously at the client.
   *
   *  Register it with the constructor of the event client.
   *
   *  Template parameters:
   *    - \b E: Event answer class (Communication Object)
   *
   *  Demonstrated in @ref example5
   */
  template<class E> class EventHandler {
  public:
    // User interface

    /** Handler which is called by the event client pattern with every
     *  fired event.
     *
     *  The handler method has to be provided by the user by subclassing.
     *
     *  @param id contains the activation id to be able to find out which
     *         activation caused the current event
     *  @param event is the event answer class (Communication Object)
     *         received due to the firing of the activation with identifier id.
     */
    virtual void handleEvent( const EventId id, const E& event ) throw() = 0;
  };
}


namespace CHS {

  /** Handles the event service on client side.
   *
   *  Template parameters:
   *    - \b P: Activation parameter class (Communication Object)
   *            contains individual parameters of the event activation.
   *    - \b E: Event answer class (Communication Object)
   *            is returned when an event fires and can contain further
   *            details why and under which circumstances an event fired.
   *
   *  Demonstrated in @ref example5
   */
  template<class P, class E> class EventClient {
  private:
    typedef struct EventClientList {
      EventId              id;
      EventMode            mode;
      SmartCVwithMemory    condW;           /// see comments in getEvent() / getNextEvent()
      SmartCVwithoutMemory condWO;
      E                    e;
      int                  eventFired;
      int                  eventAvailable;
      EventClientList      *next;
    }EventClientList;

    EventHandler<E>   *hndObject;

    /// mutexEventList   protects the list of active events while being accessed
    ///                  since this list is being manipulated from various member
    ///                  functions and handlers called from server side in parallel
    /// mutexConnection  protects critical sections from being executed in
    ///                  parallel. These are all related to changing connections
    ///                  while client/server interactions are active. The mutex
    ///                  avoids racing conditions when for example an activate
    ///                  is being executed and a disconnect is called in parallel.
    EventClientList     *events;
    SmartRecursiveMutex mutexEventList;
    SmartRecursiveMutex mutexConnection;
    int                 eventCnt;

    /// management class of the component
    SmartComponent *component;

    /// the CORBA client object
    EventClient_impl *corbaClient;

    /// the CORBA client reference transferred to server
    SmartEventClientPattern_ptr h;

    /// the CORBA server connection
    SmartEventServerPattern_var corbaServant;

    /// 0/1 to indicate whether client is not connected / is connected to server
    int statusConnected;

    /// 0/1 to indicate whether port is not contained / is contained in the set
    /// of externally accessible client wiring
    int statusManagedPort;

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// indicates the user status with respect to blocking, true/false blocking/non-blocking, default: true
    /// needs to be stored in pattern since every activate generates new condition variable which needs
    /// proper initialization
    bool statusUserBlocking;

    /// stores pointer to wiring slave when managed port
    WiringSlave *wiringslave;

    /// private static functions to be accessed from the CORBA layer
    static void hndEvent(void*, const CORBA::Any &, CORBA::Long);

    /// private static functions to be accessed from the CORBA layer
    static void hndServDisc(void*);

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    EventClient() throw(CHS::SmartError);

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

    /** (helper method to clean up list of events)
     *
     *  @param id  provides the identifier of the event
     *
     *  @return status code:
     *     - SMART_OK       : everything ok and id removed from list
     *     - SMART_WRONGID  : id not found in list
     *     - SMART_ERROR    : something went wrong
     */
    CHS::StatusCode removeActivationId(const CHS::EventId id) throw();

  public:
    /** Constructor (exposed as port wireable from outside by other components and without handler).
     *
     *  Constructs an EventClient for method oriented event processing.
     *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
     *
     *  add()/remove() and connect()/disconnect() can always be used to change the
     *  status of the instance. Instance is not connected to a service provider.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use, instance not created
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     *  @param component  management class of the component
     *  @param port       name of the wireable port
     *  @param slave      wiring slave of this component
     */
     EventClient(SmartComponent* component, const std::string& port, WiringSlave* slave) throw(CHS::SmartError);

     /** Constructor (not wired with service provider, not exposed as port and without handler).
      *
      *  Constructs an EventClient for method oriented event processing.
      *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
      *
      *  add()/remove() and connect()/disconnect() can always be used to change the
      *  status of the instance. Instance is not connected to a service provider and
      *  is not exposed as port wireable from outside the component.
      *
      *  (Currently exception not thrown)
      *
      *  @param component  management class of the component
      */
     EventClient(SmartComponent* component) throw(CHS::SmartError);

     /** Constructor (wired with specified service provider and without handler)
      *
      *  Constructs an EventClient for method oriented event processing.
      *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
      *  Connects to the denoted service and blocks until the connection has been
      *  established. Blocks infinitely if denoted service becomes not available
      *  since constructor performs retries. Blocking is useful to simplify startup
      *  of components which have mutual dependencies.
      *
      *  add()/remove() and connect()/disconnect() can always be used to change the
      *  status of the instance.
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
     EventClient(SmartComponent* component, const std::string& server, const std::string& service) throw(CHS::SmartError);

    /** Constructor (exposed as port wireable from outside by other components and with handler).
     *
     *  Constructs an EventClient for handler based event processing.
     *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
     *
     *  add()/remove() and connect()/disconnect() can always be used to change the
     *  status of the instance. Instance is not connected to a service provider.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use, instance not created
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     *  @param component  management class of the component
     *  @param port       name of the wireable port
     *  @param slave      wiring slave of this component
     *  @param handler    that processes incoming events
     */
     EventClient(SmartComponent* component, const std::string& port, WiringSlave* slave, EventHandler<E>& handler) throw(CHS::SmartError);

     /** Constructor (not wired with service provider, not exposed as port and with handler).
      *
      *  Constructs an EventClient for handler based event processing.
      *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
      *
      *  add()/remove() and connect()/disconnect() can always be used to change the
      *  status of the instance. Instance is not connected to a service provider and
      *  is not exposed as port wireable from outside the component.
      *
      *  (Currently exception not thrown)
      *
      *  @param component  management class of the component
      *  @param handler    that processes incoming events
      */
     EventClient(SmartComponent* component, EventHandler<E>& handler) throw(CHS::SmartError);

     /** Constructor (wired with specified service provider and with handler)
      *
      *  Constructs an EventClient for handler based event processing.
      *  Use tryEvent()/getEvent()/getNextEvent() to process incoming events.
      *  Connects to the denoted service and blocks until the connection has been
      *  established. Blocks infinitely if denoted service becomes not available
      *  since constructor performs retries. Blocking is useful to simplify startup
      *  of components which have mutual dependencies.
      *
      *  add()/remove() and connect()/disconnect() can always be used to change the
      *  status of the instance.
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
      * @param handler    that processes incoming events
      */
     EventClient(SmartComponent* component, const std::string& server, const std::string& service, EventHandler<E>& handler) throw(CHS::SmartError);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly
     *  deactivates every still active event and removes the instance from the
     *  set of wireable ports.
     */
    virtual ~EventClient() throw();

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
     *  for implications on active events in that case.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to tryEvent()/getEvent() or
     *  getNextEvent().
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
    StatusCode connect(const std::string& server, const std::string& service) throw();

    /** Disconnect the service requestor from the service provider.
     *
     *  All blocking calls are aborted with the SMART_DISCONNECTED status
     *  code. All events are deactivated.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to tryEvent() / getEvent()
     *  or getNextEvent().
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

    /** Activate an event with the provided parameters in either "single" or "continuous" mode.
     *
     *  @param mode        "single" or "continuous" mode
     *  @param parameter   activation parameter class (Communication Object)
     *  @param id          is set to the unique id of the event activation
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok, event is activated and <I>id</I> contains
     *                                  a valid activation identifier.
     *    - SMART_DISCONNECTED        : activation not possible since not connected to a server. No
     *                                  valid activation identifier <I>id</I> returned.
     *    - SMART_ERROR_COMMUNICATION : communication problems, event not activated, <I>id</I> is not
     *                                  a valid activation identifier.
     *    - SMART_ERROR               : something went wrong, event not activated, <I>id</I> is not
     *                                  a valid activation identifier.
     */
    StatusCode activate(const EventMode mode , const P& parameter, EventId& id) throw();

    /** Deactivate the event with the specified identifier.
     *
     *  An event must always be deactivated, even if it has already
     *  fired in single mode. This is just necessary for cleanup
     *  procedures and provides a uniform user API independently of the
     *  event mode. Calling deactivate() while there are blocking calls
     *  aborts them with the appropriate status code.
     *
     *  @param id of event to be disabled
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and event is deactivated
     *    - SMART_WRONGID             : there is no active event available with this id
     *    - SMART_ERROR_COMMUNICATION : communication problems, event not deactivated
     *    - SMART_ERROR               : something went wrong, event not deactivated
     *
     * (Hint: can not return SMART_DISCONNECTED since then each event is for sure also
     *        deactivated resulting in SMART_WRONGID)
     */
    StatusCode deactivate(const EventId id) throw();

    /** Check whether event has already fired and return immediately
     *  with status information.
     *
     *  This method does not consume an available event.
     *
     *  @param id of the event activation to be checked
     *
     *  @return status code
     *    - single mode:
     *      - SMART_OK                : event fired already, is still available and can be consumed by
     *                                  calling getEvent(),
     *      - SMART_ACTIVE            : event has not yet fired
     *      - SMART_PASSIVE           : event fired already and is already consumed.
     *      - SMART_WRONGID           : there is no activation available with this <I>id</I>
     *    - continuous mode:
     *      - SMART_OK                : unconsumed event is available. Since events are
     *                                  overwritten this means that at least one new
     *                                  event has been received since the last event
     *                                  consumption.
     *      - SMART_ACTIVE            : currently there is no unconsumed event available.
     *      - SMART_WRONGID           : there is no activation available with this <I>id</I>
     */
    StatusCode tryEvent(const EventId id) throw();

    /** Blocking call which waits for the event to fire and then consumes the event.
     *
     *  This method consumes an event. Returns immediately if an unconsumed event is 
     *  available. Blocks otherwise till event becomes available. If method is called
     *  concurrently from several threads with the same <I>id</I> and method is blocking, 
     *  then every call returns with the same <I>event</I> once the event fired. If there is 
     *  however already an unconsumed event available, then only one out of the concurrent 
     *  calls consumes the event and the other calls return with appropriate status codes.
     *
     *  @param id of the event activation
     *  @param event is set to the returned event if fired (Communication Object)
     *
     *  - <b>single mode</b>:
     *      <p>
     *      Since an event in single mode fires only once, return immediately
     *      if the event is already consumed. Also return immediately with an
     *      available and unconsumed event and consume it. Otherwise wait until
     *      the event fires.
     *      </p>
     *      <p>
     *      <b>Returns status code</b>:
     *      </p>
     *      <p>
     *        - SMART_OK            : event fired and event is consumed and returned.
     *        - SMART_PASSIVE       : event fired and got consumed already. Returns immediately without
     *                                valid event since it can not fire again in single mode.
     *        - SMART_CANCELLED     : waiting for the event to fire has been aborted or blocking is not
     *                                not allowed anymore. Therefore no valid <I>event</I> is returned.
     *        - SMART_DISCONNECTED  : client is disconnected or got disconnected while waiting and 
     *                                therefore no valid <I>event</I> is returned and the activation
     *                                identifier <I>id</I> is also not valid any longer due to
     *                                automatic deactivation.
     *        - SMART_NOTACTIVATED  : got deactivated while waiting and therefore <I>event</I> not valid and
     *                                also <I>id</I> not valid any longer.
     *        - SMART_WRONGID       : there is no activation available with this <I>id</I> and therefore
     *                                <I>event</I> not valid.
     *      </p>
     *
     *  - <b>continuous mode</b>:
     *     <p>
     *     Returns immediately if an unconsumed event is
     *     available. Returns the newest unconsumed event since
     *     activation. Otherwise waits until the event fires again.
     *     </p>
     *     <p>
     *     <b>Returns status code</b>:
     *     </p>
     *     <p>
     *        - SMART_OK            : unconsumed event is available and event is consumed and returned.
     *                                Due to the overwriting behavior, only the latest event is available.
     *        - SMART_CANCELLED     : blocking is not allowed anymore therefore blocking call has been aborted and
     *                                <I>event</I> is not valid.
     *        - SMART_DISCONNECTED  : got disconnected while waiting and therefore <I>event</I> not valid and 
     *                                also <I>id</I> not valid any longer due to automatic deactivation.
     *        - SMART_NOTACTIVATED  : got deactivated while waiting and therefore <I>event</I> not valid and
     *                                also <I>id</I> not valid any longer.
     *        - SMART_WRONGID       : there is no activation available with this <I>id</I> and therefore
     *                                <I>event</I> not valid.
     *     </p>
     */
    StatusCode getEvent(const EventId id, E& event) throw();

    /** Blocking call which waits for the next event.
     *
     *  This methods waits for the <I>next</I> arriving event to make sure that only events arriving 
     *  after entering the method are considered. Method consumes event. An old event that has been
     *  fired is ignored (in contrary to getEvent()). If method is called concurrently from several
     *  threads with the same <I>id</I>, then every call returns with the same <I>event</I> once the 
     *  event fired.
     *
     *  @param id of the event activation
     *  @param event is set to the returned event if fired (Communication Object)
     *
     *  - <b>single mode</b>:
     *    <p>
     *    In single mode one misses the event if it fired before entering this member function.
     *    </p>
     *    <p>
     *    <b>Returns status code</b>:
     *    </p>
     *    <p>
     *      - SMART_OK            : event fired while waiting for the event and event is consumed
     *                              and returned
     *      - SMART_PASSIVE       : event already fired between activation and calling this member
     *                              function and is therefore missed or event has already been
     *                              consumed and can not fire again in single mode. Does not block
     *                              indefinitely and returns no valid <I>event</I>.
     *      - SMART_CANCELLED     : event not yet fired and waiting for the event has been aborted or
     *                              blocking is not allowed anymore. No valid <I>event</I> is returned.
     *      - SMART_DISCONNECTED  : got disconnected while waiting and therefore <I>event</I> not valid
     *                              and also <I>id</I> not valid any longer due to automatic deactivation.
     *      - SMART_NOTACTIVATED  : got deactivated while waiting and therefore <I>event</I> not valid and
     *                              also <I>id</I> not valid any longer.
     *      - SMART_WRONGID       : there is no activation available with this <I>id</I> and therefore
     *                              <I>event</I> not valid.
     *    </p>
     *
     *  - <b>continuous mode</b>:
     *    <p>
     *    Makes sure that only events fired after entering this member function are considered.
     *    </p>
     *    <p>
     *    <b>Returns status code</b>:
     *    </p>
     *    <p>
     *      - SMART_OK            : event fired while waiting for the event and event is consumed
     *                              and returned
     *      - SMART_CANCELLED     : waiting for the next event has been aborted or blocking is not 
     *                              allowed anymore. No valid <I>event</I> is returned.
     *      - SMART_DISCONNECTED  : got disconnected while waiting and therefore <I>event</I> not valid
     *                              and also <I>id</I> not valid any longer due to automatic deactivation.
     *      - SMART_NOTACTIVATED  : got deactivated while waiting and therefore <I>event</I> not valid and
     *                              also <I>id</I> not valid any longer.
     *      - SMART_WRONGID       : there is no activation available with this <I>id</I> and therefore
     *                              <I>event</I> not valid.
     *    </p>
     */
    StatusCode getNextEvent(const EventId id, E& event) throw();
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// server event template
//
namespace CHS {


  /** Condition Test Handler (decides at server whether event fires or not).
   *
   */
  template<class P, class E, class S> class EventTestHandler
  {
  public:
    /** This is the test method which decides whether the event fires or
     *  not.
     *
     *  As soon as the EventServer::put() is called, the pattern calls
     *  EventTestHandler::testEvent() to decide for each event
     *  activation parameter set individually whether this parameter
     *  set requires the event to fire.
     *
     *  The parameters p are provided by the client individually with
     *  every event activation, the current state s is provided via
     *  the EventServer::put() method when the server wants the event
     *  conditions to be checked.
     *
     *  The test method has to be provided by the user. In case the
     *  event fires, one can return data in the event answer object
     *  e.
     *
     * \emp Attention: this function will be called within the same
     *                 context as the EventServer::put()-method.
     *                 Therefore pay attention to blocking calls etc.
     *
     *  @param p activation parameter set to be checked (Communication Object).
     *           Can be modified in the test event handler to store state
     *           information in the parameter object (for example needed to
     *           easily implement an event which only fires with state changes)
     *  @param e event answer object (Communication Object) which returns data
     *           in case of firing to the client which is responsible for this event
     *           activation
     *  @param s current information against which the testEvent() checks
     *           the parameters p
     *
     *  @return status code
     *     - true: fire event (predicate true)
     *     - false: do not fire event (predicate false) */
     virtual bool testEvent(P& p, E& e, const S& s) throw() = 0;
  };


  /** Template for generating a class which handles the event
   *  service on server side.
   *
   *  Template parameters:
   *    - \b P: Activation parameter class (Communication Object)
   *    - \b E: Event answer class (Communication Object)
   *    - \b S: State class (provides current information needed in testEvent() to
   *            check whether an event activation fires or not)
   *
   *  Demonstrated in @ref example5
   *
   */
  template<class P, class E, class S> class EventServer {
  private:
    //
    // element of list of activations
    //
    typedef struct EventServerList {
      EventId                     clientId;
      EventMode                   eventMode;
      SmartEventClientPattern_ptr client;
      P                           p;
      int                         eventCnt;
      EventServerList             *next;
    }EventServerList;

    //
    // element of list of connected clients
    //
    typedef struct EventClientList {
      SmartEventClientPattern_ptr client;
      EventClientList *next;
    }EventClientList;

    EventServerList     *events;
    EventClientList     *clients;
    SmartRecursiveMutex mutexEventList;
    SmartRecursiveMutex mutexClientList;

    /// management class of the component
    SmartComponent *component;

    /// name of service
    std::string service;

    /// handler object that contains the test function
    EventTestHandler<P,E,S> & testHandler;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor and incoming handlers
    int serverReady;

    /// the CORBA server object
    EventServer_impl *corbaServant;

    /// private functions
    static void hndActivate(void*, const SmartEventClientPattern_ptr, const CORBA::Long, const CORBA::Long, const CORBA::Any &);

    /// private functions
    static void hndDeactivate(void*, const SmartEventClientPattern_ptr, const CORBA::Long);

    /// private functions
    static CHS::StatusCode hndCnct(void*, const SmartEventClientPattern_ptr);

    /// private functions
    static void hndDisc(void*, const SmartEventClientPattern_ptr);

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    EventServer() throw(CHS::SmartError);

  public:
    /** Constructor.
     *
     *  @param component management class of the component
     *  @param service name of the service
     *  @param testHandler test handler decide if a event needs to fire
     */
    EventServer(SmartComponent* component,const std::string& service,EventTestHandler<P,E,S>& testHandler) throw();

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all activations are properly deactivated at client side.
     */
    virtual ~EventServer() throw();

    /** Initiate testing the event conditions for the activations.
     *
     *  @param state contains the current information checked in testEvent()
     *         against the individual activation parameters.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *
     */
    StatusCode put(const S& state) throw();

  };
}

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include <smartEvent.th>

#endif    // _SMARTEVENT_HH

