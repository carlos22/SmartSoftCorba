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

#ifndef _SMARTPUSHNEWEST_HH
#define _SMARTPUSHNEWEST_HH

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include "smartComponent.hh"
#include "smartPushNewestServerPattern.hh"
#include "smartPushNewestClientPattern.hh"

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

namespace CHS {
  /** Client part of the pushNewest pattern to provide a push newest
   *  service. Clients can subscribe to get updates each time new
   *  data is available at the server.
   *
   *  Template parameters
   *    - \b T: Pushed value class (Communication Object)
   *
   *  Demonstrated in @ref example2
   */
  template<class T> class PushNewestClient {
  private:
    T                    data;
    SmartCVwithoutMemory cond;

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
    PushNewestClient_impl *corbaClient;

    /// the CORBA client reference transferred to server
    SmartPushNewestClientPattern_ptr h;

    /// the CORBA server connection
    SmartPushNewestServerPattern_var corbaServant;

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

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    PushNewestClient() throw(CHS::SmartError);

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
    PushNewestClient(SmartComponent* component, const std::string& port, WiringSlave* slave) throw(CHS::SmartError);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    PushNewestClient(SmartComponent* component) throw(CHS::SmartError);

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
    PushNewestClient(SmartComponent* component, const std::string& server, const std::string& service) throw(CHS::SmartError);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly cleans up
     *  every pending data reception and removes the instance from the set of wireable ports.
     */
    virtual ~PushNewestClient() throw();

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

    /** Subscribe at the server to get updates as soon as new data is available
     *  at the server.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is subscribed
     *    - SMART_DISCONNECTED        : client is not connected to a server and can therefore
     *                                  not subscribe for updates, not subscribed
     *    - SMART_ERROR_COMMUNICATION : communication problems, not subscribed
     *    - SMART_ERROR               : something went wrong, not subscribed
     */
    StatusCode subscribe() throw();

    /** Unsubscribe to get no more updates. 
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is now unsubscribed or
     *                                  has already been unsubscribed. All blocking calls are
     *                                  aborted with the appropriate status and yet received
     *                                  and still buffered data is deleted to avoid returning
     *                                  old data.
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
     *  returned after the client got unsubscribed.
     *
     * @param d is set to the latest available data
     *
     * @return status code
     *   - SMART_OK                  : everything is ok and latest data returned.
     *   - SMART_NODATA              : client has not yet received an update since subscription and
     *                                 therefore no data is available and no data is returned.
     *   - SMART_UNSUBSCRIBED        : no data available since client is not subscribed and can therefore
     *                                 not receive updates. Method does not return old data from last 
     *                                 subscription since these might be based on too old parameter settings. 
     *                                 To get data one has to be subscribed.
     *   - SMART_DISCONNECTED        : no data available since client is even not connected to a server.
     *   - SMART_ERROR               : something went wrong and no data returned.
     */
    StatusCode getUpdate(T& d) throw();

    /** Blocking call which waits until the next update is received.
     *
     *  Blocking is aborted with the appropriate status if either the
     *  client gets unsubscribed or disconnected or if blocking is not 
     *  allowed anymore at the client.
     *
     *  @param d is set to the latest available data
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok and just received data is returned.
     *   - SMART_CANCELLED           : blocking not allowed or not allowed anymore. Waiting for the next 
     *                                 update is aborted and no valid data is returned.
     *   - SMART_UNSUBSCRIBED        : client is unsubscribed or got unsubscribed and member function
     *                                 returns without valid data.
     *   - SMART_DISCONNECTED        : client is not connected or got disconnected and member function
     *                                 returns without valid data.
     *   - SMART_ERROR               : something went wrong and no data returned.
     */
    StatusCode getUpdateWait(T& d) throw();
  };
}



/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// server push newest template
//
namespace CHS {
  /** Server part of the pushNewest pattern to provide a push newest
   *  service. New data provided to that pattern by the server is
   *  immediately sent to every subscribed client.
   *
   *  Template parameters
   *    - \b T: Pushed value class (Communication Object)
   *
   *  Demonstrated in @ref example2
   */
  template<class T> class PushNewestServer {
  private:
    typedef struct PushNewestClientList {
      SmartPushNewestClientPattern_ptr client;
      PushNewestClientList             *next;
      int                              subscribed;
    };

    SmartRecursiveMutex mutexClientList;

    PushNewestClientList *subscriptions;

    /// management class of the component
    SmartComponent *component;

    /// name of server
    std::string service;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor ...
    int serverReady; 

    /// the CORBA server object
    PushNewestServer_impl *corbaServant;

    /// private functions
    static CHS::StatusCode hndCnct(void*, const SmartPushNewestClientPattern_ptr);

    /// private functions
    static void hndDisc(void*, const SmartPushNewestClientPattern_ptr);

    /// private functions
    static void hndSubscribe(void*, const SmartPushNewestClientPattern_ptr);

    /// private functions
    static void hndUnsubscribe(void*, const SmartPushNewestClientPattern_ptr);

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    PushNewestServer() throw(CHS::SmartError);

  public:
    /** Constructor.
     *
     * @param component  management class of the component
     * @param service    name of the service
     *
     * (Currently exception not thrown)
     */
    PushNewestServer(SmartComponent* component, const std::string& service) throw(CHS::SmartError);

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all connected and subscribed clients are unsubscribed and
     *  disconnected properly.
     */
    virtual ~PushNewestServer() throw();

    /** Send updated data to all subscribed clients
     *
     *  @param d contains the value object to be sent to every
     *           subscribed client
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok
     *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least
     *                                  one client. The other clients are updated
     *                                  correctly.
     *    - SMART_ERROR               : something went wrong with at least one
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
#include <smartPushNewest.th>

#endif    // _SMARTPUSHNEWEST_HH

