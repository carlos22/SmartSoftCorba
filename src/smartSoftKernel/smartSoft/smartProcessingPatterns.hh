// --------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
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
// --------------------------------------------------------------------------


#ifndef _SMARTPROCESSINGPATTERNS_HH
#define _SMARTPROCESSINGPATTERNS_HH


namespace CHS {

  /** Decorator for QueryServerHandler to defer handling to another
   *  thread.
   *
   *  This Decorator (see Design Patterns by Gamma et. al) can be used
   *  if a QueryServerHandler is slow and/or blocking and would
   *  therefore block the framework. This handler is an active object
   *  and has a single thread, in which the Queries are handled one after
   *  another.
   *
   *  example usage:
   *  \code
   *
   *  MySlowQueryHandler: public QueryServerHandler<R,A>
   *  {
   *    ...
   *  };
   *
   *  ...
   *
   *  MySlowQueryHandler slowHandler;
   *  ThreadQueueQueryHandler<R,A> threadedHandler(slowHandler);
   *  QueryServer queryService<R,A>(component,"heavy_computation", threadedHandler);
   *  \endcode
   */
  template <class R, class A>
  class ThreadQueueQueryHandler : public CHS::SmartTask,
				  public CHS::QueryServerHandler<R,A>
  {
  protected:
    /** needed to store a complete query request.  */
    struct MsgQueueItem{
      MsgQueueItem() : server(0), id(0), request() {};
      MsgQueueItem(CHS::QueryServer<R, A> & server, CHS::QueryId id, const R& r) : server(&server), id(id), request(r) {};
      CHS::QueryServer<R, A> * server;
      CHS::QueryId   id;
      R request;
    };

    /** original handler.
     */
    QueryServerHandler<R,A> & handler;

    /** message queue to our thread.
     */
    CHS::MessageQueue<MsgQueueItem> msgQueue;

  public:

    /** Create a new threaded QueryServerHandler Decorator.
     *
     *  the handling thread can be started automatically, or with a
     *  separate open();
     *
     *  @param handler which will be called in a separate thread.
     *  @param start   start the handler thread (default)
     */
    ThreadQueueQueryHandler(QueryServerHandler<R,A> & handler, bool start=true)
      : handler(handler)
      {
	open();
      };

    /** queues request for execution in the other thread.
     *
     *  @internal
     */
    void handleQuery(CHS::QueryServer<R,A>& server,
		       const QueryId id,
		       const R& request) throw()
      {
	std::cerr << "enqueing query" << std::endl;
	// defer query to thread.
	msgQueue.enqueue(MsgQueueItem(server, id, request));
      };

    /** method that runs in a separate thread.
     *
     *  The default svc() just sits in a loop waiting for incoming
     *  queries, and calling the handler registered on construction. If
     *  another behaviour is needed it can be added by subclassing.
     *
     *  The loop can be cooperatively cancelled with SmartThreadManager.
     */
    virtual int svc() {
      // sit here and wait for requests
      MsgQueueItem item;
      while(! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self ())) {
// <asteck date="20.11.2008">
//std::cout << "EINSPRUNG svc() vor dequeue" << std::endl;
	item = msgQueue.dequeue();
//std::cout << "EINSPRUNG svc() nach dequeue" << std::endl;
	std::cerr << "thread dequeued query" << std::endl;
	handler.handleQuery(*(item.server), item.id, item.request);
// </asteck>
      }
      return 0;
    }
  };

  
  /** Decorator for SendServerHandler to defer handling to another
   *  thread.
   *
   *  This decorator (see Design Patterns by Gamma et. al) can be used
   *  if a SendServerHandler is slow and/or blocking and would
   *  therefore block the framework. This handler is an active object
   *  and has a single thread, in which the Sends are handled one after
   *  another.
   *
   *  example usage:
   *  \code
   *
   *  MySlowSendHandler: public SendServerHandler<...>
   *  {
   *    ...
   *  };
   *
   *  ...
   *
   *  MySlowSendHandler slowHandler;
   *  ThreadQueueSendHandler threadedHandler(slowHandler);
   *  SendServer sendService<R>(component,"longcmd", threadedHandler);
   *  \endcode
   */
  template <class C>
  class ThreadQueueSendHandler : public SmartTask,
				 public SendServerHandler<C>
  {
  protected:
    /** original handler.
     */
    SendServerHandler<C> & handler;

    /** queue entry for a command */
    struct MsgQueueItem{
      MsgQueueItem() : cmd() {};
      MsgQueueItem(const C& cmd) : cmd(cmd) {};
      C cmd;
    };

    /** message queue to our thread.
     */
    MessageQueue<MsgQueueItem> msgQueue;

  public:

    /** Create a new threaded SendServerHandler Decorator.
     *
     *  the handling thread can be started automatically, or with a
     *  separate open();
     *
     *  @param handler which will be called in a separate thread.
     *  @param start   start the handler thread (default)
     *
     */
    ThreadQueueSendHandler(SendServerHandler<C> & handler, bool start=true)
      : handler(handler)
      {
	if (start) {
	  open();
	}
      };

    /** @internal method called by the pattern. */
    void handleSend( const C& cmd) throw()
      {
	// defer query to thread.
	msgQueue.enqueue(MsgQueueItem(cmd));
      };

    /** method that runs in the active objects thread.
     *
     *  The default svc() just sits in an loop waiting for incoming
     *  sends, and calling the handler registred on construction. If
     *  another behaviour is needed it can be added by subclassing.
     *
     *  The loop can be cooperatively cancelled with SmartThreadManager.
     */
    virtual int svc() {
      // sit here and wait for requests
      MsgQueueItem item;
      while(! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self ())) {
	item = msgQueue.dequeue();
	handler.handleSend(item.cmd);
      }
      return 0;
    }
  };

  /** Decorator for PushTimedServerHandler to defer handling to another
   *  thread.
   *
   *  This decorator (see Design Patterns by Gamma et. al) can be used
   *  if a PushTimedServerHandler is slow and/or blocking and would
   *  therefore block the framework. This handler is an active object
   *  and has a single thread, in which the Pushes are handled one after
   *  another.
   *
   *  example usage:
   *  \code
   *
   *  MySlowPushTimedHandler: public PushTimedHandler<...>
   *  {
   *    ...
   *  };
   *
   *  ...
   *
   *  MySlowPushTimedHandler slowHandler;
   *  ThreadQueuePushTimedHandler threadedHandler(slowHandler);
   *  PushTimedServer pushService<R>(component,"longcmd", threadedHandler);
   *  \endcode
   */
  template <class C>
  class ThreadQueuePushTimedHandler : public SmartTask,
				 public PushTimedHandler<C>
  {
  protected:
    /** original handler.
     */
    PushTimedHandler<C> & handler;

    /** queue entry for a command */
    struct MsgQueueItem{
      MsgQueueItem() : server(0) {};
      MsgQueueItem(PushTimedServer<C> & server) : server(&server) {};
      CHS::PushTimedServer<C> * server;
    };

    /** message queue to our thread.
     */
    MessageQueue<MsgQueueItem> msgQueue;

  public:

    /** Create a new threaded PushTimedHandler Decorator.
     *
     *  the handling thread can be started automatically, or with a
     *  separate open();
     *
     *  @param handler which will be called in a separate thread.
     *  @param start   start the handler thread (default)
     *
     */
    ThreadQueuePushTimedHandler(PushTimedHandler<C> & handler, bool start=true)
      : handler(handler)
      {
	if (start) {
	  open();
	}
      };

    /** @internal method called by the pattern. */
    void handlePushTimer(CHS::PushTimedServer<C> & server) throw()
      {
	// defer query to thread.
	msgQueue.enqueue(MsgQueueItem(server));
      };

    /** method that runs in the active objects thread.
     *
     *  The default svc() just sits in an loop waiting for incoming
     *  timeouts, and calling the handler registred on construction. If
     *  another behaviour is needed it can be added by subclassing.
     *
     *  The loop can be cooperatively cancelled with SmartThreadManager.
     */
    virtual int svc() {
      // sit here and wait for requests
      MsgQueueItem item;
      while(! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self ())) {
	item = msgQueue.dequeue();
	handler.handlePushTimer(*(item.server));
      }
      return 0;
    }
  };


};
  
  

#endif // _SMARTPROCESSINGPATTERNS_HH
