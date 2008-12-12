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

#ifndef _SMARTPUSHTIMED_HH
#define _SMARTPUSHTIMED_HH

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include "smartComponent.hh"
#include "smartPushTimedInternal.hh"
#include "smartPushTimedServerPattern.hh"
#include "smartPushTimedClientPattern.hh"

//
// needed for the add() / remove() interface for wiring
//
#include "smartCommWiring.hh"

namespace CHS {
  class WiringSlave;
}

/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////


//
// client push timed template
//
namespace CHS {
  /** Client part of the pushTimed pattern to provide a push timed
   *  service. Clients can subscribe to regularly get every n-th
   *  update.
   *
   *  Template parameters
   *    - \b T: Pushed value class (Communication Object)
   *
   *  Demonstrated in @ref example8
   */
  template<class T> class PushTimedClient {
  private:
    T                    data;
    SmartCVwithoutMemory cond;

    /// 0/1 to indicate whether server is stopped / started
    int statusServer;

    /// 0/1 to indicate whether client is not subscribed / is subscribed to server
    int statusSubscribed;

    /// 0/1 to indicate whether data is not available / is available
    int dataAvailable;

    /// mutexConnection protects critical sections from being executed in
    ///                 parallel. These are all related to changing connections
    ///                 while client/server interactions are active. The mutex
    ///                 avoids racing conditions when for example a subscribe
    ///                 and a disconnect are called in parallel.
    SmartRecursiveMutex mutexConnection;

    /// management class of the component
    SmartComponent *component;

    /// the CORBA client object
    PushTimedClient_impl *corbaClient;

    /// the CORBA client reference transferred to server
    SmartPushTimedClientPattern_ptr h;

    /// the CORBA server connection
    SmartPushTimedServerPattern_var corbaServant;

    /// 0/1 to indicate whether client is not connected / is connected to server
    int statusConnected;

    /// 0/1 to indicate whether port is not contained / is contained in the set
    /// of externally accessible client wiring
    int statusManagedPort;

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// stores pointer to wiring slave when managed port
    WiringSlave *wiringslave;

    /// private static functions to be accessed from the CORBA layer
    static void hndUpdate(void*, const CORBA::Any &);

    /// private static functions to be accessed from the CORBA layer
    static void hndServDisc(void*);

    /// private static functions to be accessed from the CORBA layer
    static void hndServerStateChange(void*,int &);

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    PushTimedClient() throw(CHS::SmartError);

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
    /** Constructor (exposed as port and wireable from outside by other components).
     *  add()/remove() and connect()/disconnect() can always be used to change the
     *  status of the instance. Instance is not connected to a service provider.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use, instance not created
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param server     name of the server
     * @param service    name of the service
     */
    PushTimedClient(SmartComponent* component, const std::string& port, WiringSlave* slave) throw(CHS::SmartError);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    PushTimedClient(SmartComponent* component) throw(CHS::SmartError);

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
    PushTimedClient(SmartComponent* component, const std::string& server, const std::string& service) throw(CHS::SmartError);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly cleans up
     *  every pending data reception and removes the instance from the set of wireable ports.
     */
    virtual ~PushTimedClient() throw();

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
    StatusCode connect(const std::string& server, const std::string& service) throw();

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

    /** Subscribe at the server to periodically get every n-th update. A
     *  newly subscribed client gets the next available new data and is
     *  then updated with regard to its individual update cycle.
     *
     *  @param interval  whole-numbered value to set the update rate to
     *                   every n-th value
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is subscribed
     *    - SMART_DISCONNECTED        : client is not connected to a server and can therefore
     *                                  not subscribe for updates, not subscribed
     *    - SMART_ERROR_COMMUNICATION : communication problems, not subscribed
     *    - SMART_ERROR               : something went wrong, not subscribed
     */
    StatusCode subscribe(const int interval) throw();

    /** Unsubscribe to get no more updates. All blocking calls are aborted with the appropriate
     *  status and yet received and still buffered data is deleted to avoid returning old data.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is now unsubscribed or
     *                                  has already been unsubscribed
     *    - SMART_ERROR_COMMUNICATION : communication problems, not unsubscribed
     *    - SMART_ERROR               : something went wrong, not unsubscribed
     *
     * (can not return SMART_DISCONNECTED since then client is for sure also unsubscribed
     *  which results in SMART_OK)
     */
    StatusCode unsubscribe() throw();

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

    /** Non-blocking call to immediately return the latest available
     *  data buffered at the client side from the most recent update.
     *
     *  No data is returned as long as no update is received since
     *  subscription. To avoid returning old data, no data is
     *  returned after the client is unsubscribed or when the
     *  server is not active.
     *
     * @param d is set to the newest currently available data
     *
     * @return status code
     *   - SMART_OK                  : everything ok and latest data since client got subscribed 
     *                                 is returned.
     *   - SMART_NODATA              : client has not yet received an update since subscription and
     *                                 therefore no data is available and no data is returned.
     *   - SMART_NOTACTIVATED        : the server is currently not active and does therefore not 
     *                                 provide updates at the expected rate. No valid data returned.
     *   - SMART_UNSUBSCRIBED        : no data available since client is not subscribed and can
     *                                 therefore not receive updates. Method does not return old data from
     *                                 last subscription since these might be based on too old parameter 
     *                                 settings. To get data one has to be subscribed.
     *   - SMART_DISCONNECTED        : no data returned since client is even not connected to a server.
     *   - SMART_ERROR               : something went wrong
     */
    StatusCode getUpdate(T& d) throw();

    /** Blocking call which waits until the next update is received.
     *
     *  Blocking is aborted with the appropriate status if either the 
     *  server gets deactivated, the client gets unsubscribed or disconnected
     *  or if blocking is not allowed any more at the client.
     *
     *  @param d is set to the newest currently available data
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok and just received data is returned.
     *   - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore. Waiting for the
     *                                 next update is aborted and no valid data is returned.
     *   - SMART_NOTACTIVATED        : the server is currently not active and does therefore not provide updates
     *                                 at the expected rate. No valid data returned.
     *   - SMART_UNSUBSCRIBED        : returns immediately without data if the client is not subscribed.
     *   - SMART_DISCONNECTED        : returns immediately without data since client is even not connected
     *                                 to a server.
     *   - SMART_ERROR               : something went completely wrong and no valid data returned.
     */
    StatusCode getUpdateWait(T& d) throw();

    /** Get cycle time and server state.
     *
     *  Returns cycle time of server in [seconds] and server state indicating
     *  whether server is activated or not. The client can decide on the cycle
     *  time on its individual update rate at subscription.
     *
     * @param t is set to the server cycle time [seconds]
     * @param r indicates whether server is started [true, false]
     *
     * @return status code
     *   - SMART_OK                  : everything is ok and returned values are valid.
     *   - SMART_DISCONNECTED        : client is not connected to a server and
     *                                 therefore not get any valid server info.
     *   - SMART_ERROR_COMMUNICATION : communication problems and returned server info is not valid.
     *   - SMART_ERROR               : something went completely wrong and no valid server info returned.
     */
    StatusCode getServerInfo(double& t,bool& r) throw();
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// server push timed template
//
namespace CHS {
  
  // forward declaration
  template<class T> class PushTimedServer;
  
  /** Handler class for Push Timed Server to handle timer events.
   *
   *  Used by the PushTimedServer to handle timer events
   *  to initiate the next data acquisition for the push
   *  timed service.
   *
   *  The user should provide the handlePushTimer() method by
   *  subclassing and register an instance of this handler
   *  class with the PushTimedServer.
   */
  template<class T>
  class PushTimedHandler
  {
  public:
    /** Empty virtual destructor
     */
    virtual ~PushTimedHandler() {}
  
    /** Handler method for timer events of the push timed server
     *
     *  This handler is triggered by the internal timer to regularly
     *  initiate data acquisition for the pushTimed service when the
     *  service is activated.
     *
     *  It should use PushTimedServer::put() to send the acquired data
     *  to the Clients.
     *
     *  Since the method is executed by the components timer thread,
     *  which is reponsible for timely triggering several activities
     *  in parallel, it must be very fast and non-blocking. If data
     *  acquisition takes too much time, the handler should only be
     *  used to trigger data acquisition, which is then performed by
     *  another thread.  
     *
     *  The timer is only used to regularly trigger the data acquisition.
     *  See the put() method for further hints.
     */
    virtual void handlePushTimer(PushTimedServer<T> & server) throw() = 0;
  };
  

  /** Server part of the pushTimed pattern to provide a push timed
   *  service. A user provided handler method is regularly triggered
   *  to initiate data acquisition. Newly acquired data is provided
   *  to the server by the put-method. It immediately distributes
   *  the new data to subscribed clients taking into account their
   *  individual update cycles.
   *
   *  Template parameters
   *    - \b T: Pushed value class (Communication Object)
   *
   *  Demonstrated in @ref example8
   */
  template<class T> class PushTimedServer {
  private:
    typedef struct PushTimedClientList {
      SmartPushTimedClientPattern_ptr  client;
      int                              ctrReset;
      int                              ctrCurrent;
      int                              subscribed;
      PushTimedClientList              *next;
    };

    SmartRecursiveMutex mutexClientList;

    PushTimedClientList *subscriptions;

    /// management class of the component
    SmartComponent *component;

    /// name of server
    std::string service;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor ...
    int serverReady;
 
    /// cycleTime of the service in seconds
    double cycleTime;

    /// flag to indicate whether timedServer is currently running (1) or not (0)
    int running;

    /// number of the timer returned with activation from TimerQueue
    int timer;

    /// handler class to be registered with the timer
    CHS::SmartPushHandler *pushHandler;

    /// user handler that provides the push data
    PushTimedHandler<T> &userHandler;

    /// the CORBA server object
    PushTimedServer_impl *corbaServant;

    /// private functions
    static CHS::StatusCode hndCnct(void*, const SmartPushTimedClientPattern_ptr);

    /// private functions
    static void hndDisc(void*, const SmartPushTimedClientPattern_ptr);

    /// private functions
    static void hndSubscribe(void*, const SmartPushTimedClientPattern_ptr,int,int&);

    /// private functions
    static void hndUnsubscribe(void*, const SmartPushTimedClientPattern_ptr);

    /// private functions
    static void hndGetServerInfo(void*, double&, int&);

    /// static callback method for timer expired which then calls user implemented
    /// non-static timerHandler()
    static void hndTimerCallback(void*);

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    PushTimedServer() throw(CHS::SmartError);

  public:
    /** Constructor.
     *
     * @param component  management class of the component
     * @param service    name of the service
     * @param handler    is called every \e cycle seconds to push the information
     * @param cycle      cycle time of the service in [seconds]
     *
     * (Currently exception not thrown)
     */
    PushTimedServer(SmartComponent* component, const std::string& service, PushTimedHandler<T>& handler, const double cycle) throw(CHS::SmartError);

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all connected and subscribed clients are unsubscribed and
     *  disconnected properly.
     */
    virtual ~PushTimedServer() throw();

    /** Activate the server. The timer which triggers the data
     *  acquisition for client updates is enabled. The activation
     *  resets all internal counters such that all subscribed clients 
     *  get the first update before individual update cycles are considered.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok
     *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least one client
     *                                  which is then not informed on the server state change.
     *                                  The other clients are informed correctly.
     *    - SMART_ERROR               : something went completely wrong with at least one client
     *                                  which is then not informed on the server state change.
     *                                  The other clients are informed correctly.
     */
    StatusCode start() throw();

    /** Deactivate the server. The timer which triggers the data
     *  acquisition for client updates is disabled.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok
     *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least one client
     *                                  which is then not informed on the server state change.
     *                                  The other clients are informed correctly.
     *    - SMART_ERROR               : something went completely wrong with at least one client
     *                                  which is then not informed on the server state change.
     *                                  The other clients are informed correctly.
     */
    StatusCode stop() throw();

    /** Provide new data which is sent to all subscribed clients
     *  taking into account their individual update cycles. Update
     *  cycles are always whole-numbered multiples of the server
     *  update cycle.
     *  
     *  (Individual update interval counters are incremented each
     *   time this member function is called irrespectively of the
     *   elapsed time. One should use the time triggered handler to
     *   call the put() member function with the appropriate timing.)
     *
     *  PushTimedHandler::handlePushRequest() directly or indirectly
     *  use this method to provide new data.
     *
     *  @param d contains the newly acquired data to be sent as
     *           update.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok
     *    - SMART_NOTACTIVATED        : server is stopped and does therefore not
     *                                  distribute any data to clients. In that
     *                                  case update interval counters are not
     *                                  touched.
     *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least
     *                                  one client. The other clients are updated
     *                                  correctly.
     *    - SMART_ERROR               : something went completely wrong with at least one
     *                                  client. Some clients might still been
     *                                  updated correctly.  
     */
    StatusCode put(const T& d) throw();
  };
}

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include <smartPushTimed.th>

#endif    // _SMARTPUSHTIMED_HH

