// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
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

/** @file smartQtQueryServerHandler.hh

  @brief A Query<R,A> server handler mapping calls to the handler to Qt signals.

  @author Boris Kluge <kluge@faw.uni-ulm.de>

  Using this handler, you can process server queries from within
  your single threaded Qt GUI application.

  For example, you may have a GUI thread
  @code
  class CommRequest { ... };
  class CommAnswer { ... };
  
  class GuiThread: public QObject, public CHS::SmartTask
  {
    Q_OBJECT
  public:
    GuiThread(Smart::QtQueryServerHandler<CommRequest,CommAnswer> &query_handler)
    : _query_handler(query_handler)
    {
    }

    int svc();
    //...

  public slots:
    void handle_query();

  private:
    Smart::QtQueryServerHandler<CommRequest,CommAnswer> &_query_handler;
  };
  @endcode

  In your \c main() function you create handlers, client objects and threads
  @code
  int main(int argc, char *argv[])
  {
    std::string server_name = "...";
    std::string query_service_name = "...";

    // ...

    CHS::SmartComponent component(server_name,argc,argv);

    //...

    Smart::QtQueryServerHandler<CommRequest,CommAnswer> query_handler;

    GuiThread gui_thread(query_handler);

    CHS::QueryServer<CommRequest,CommAnswer> query_server(&component,query_service_name,query_handler);

    gui_thread.open();

    component.run();
    // ...
  }
  @endcode

  In order to be notified about handler call backs, we connect a slot to the signal
  of the handler object in the \c svc() method of the GUI thread:
  @code
  void GuiThread::svc()
  {
    QApplication qapp;
    // ...
    QObject::connect(&_query_handler, SIGNAL(new_query()), this, SLOT(handle_query()));
    // ...
    return qapp.exec();
  }
  @endcode

  The code which actually handles the callbacks goes into the slot method:
  @code
  void GuiThread::handle_query()
  {
    CommRequest request;
    CHS::QueryId id;
    CHS::QueryServer<CommRequest,CommAnswer> *query_server;

    CommAnswer answer;

    while(_query_handler.pop(&query_server,id,request))
    {
      CommAnswer answer = some_function(request);
      query_server->answer(id, answer);
    }
  }
  @endcode
*/

#ifndef SMART_QT_QUERY_SERVER_HANDLER_HH
#define SMART_QT_QUERY_SERVER_HANDLER_HH

#include "smartSoft.hh"

#include <qobject.h>
#include <qevent.h>
#include <qthread.h>

namespace Smart {

/**
  The common base class for Qt signalling Query server handlers.
  There are several reasons for having this base class. Firstly, 
  Qt slots/signals and templates hardly mix, and secondly, we can factor
  out common code in order to decrease template bloating.
 */
class QtQueryServerHandlerInterface: public QObject
{
  Q_OBJECT

public:
  /**
    Internal event method.
    This method is called from within the Qt thread.
    Overwritten from QObject, checks if the event is actually an internal 
    NewQueryEvent, and, if so, emits a signal.
   */
  virtual bool event(QEvent *e)
  {
    if(dynamic_cast<NewQueryEvent*>(e))
    {
      emit new_query();
      return true;
    }
    return QObject::event(e);
  }

signals:

  /**
    The signal which is emitted as response to calls to the server handler.
   */
  void new_query();
  
protected:

  /**
    Protected constructor. Users must create instances of derived classes only.
   */
  QtQueryServerHandlerInterface() {}

  /**
    Internal method which is called from the SmartSoft thread
    from within the server handler.
    The method posts a Qt event to this method's object, which is (in contrast
    to signals/slots) thread-safe.
   */
  inline void signal_new_query()
  {
    QThread::postEvent(this, new NewQueryEvent);
  }

  /**
    The internal event class.
   */
  class NewQueryEvent: public QEvent 
  {
  public:
    NewQueryEvent() : QEvent(User) {}
  };
};

/**
  Class template for Qt signalling Query handlers.
 */
template<class R, class A>
class QtQueryServerHandler: 
  public CHS::QueryServerHandler<R,A>,
  public QtQueryServerHandlerInterface
{
public:

  /**
    Default constuctor.
    No arguments are needed since anything interesting happens
    where the signals of this handler object are received.
   */
  QtQueryServerHandler() {}
  
  /**
    The overwritten handler method.
    It pushed the reference to the calling server, the query ID and the request onto a queue,
    and sends the Qt thread an event by calling \c signal_new_push_timed().
   */  
  void handleQuery(CHS::QueryServer<R,A> &server, const CHS::QueryId id, const R &request) throw()
  {
    _queue_mutex.acquire();
    _queue.push_back(RequestQueueEntry(server,id,request));
    _queue_mutex.release();
    signal_new_query();
  }

  /**
    Get the server which called the handler.
    This method can be used in slot methods which handle the Qt signal
    in order to recover the reference to the server, the query ID and the request.
    The method expect the address of a pointer to a server as its first argument,
    which is used to return the reference to the server.
    The query ID and the request are returned through the following reference arguments.
    The method returns \c true, if the handler was called.
   */
  inline bool pop(CHS::QueryServer<R,A> **server, CHS::QueryId &id, R &request)
  {
    bool ret = false;
    _queue_mutex.acquire();
    if(!_queue.empty())
    {
      *server = _queue.front().server;
      id = _queue.front().id;
      request = _queue.front().request;
      _queue.pop_front();
      ret = true;
    }
    _queue_mutex.release();
    return ret;
  }

private:
  /**
    The class of the queue entries.
   */
  struct RequestQueueEntry
  {
    RequestQueueEntry() : server(0), id(0), request() {}
    RequestQueueEntry(CHS::QueryServer<R, A> &server, CHS::QueryId id, const R &r) : server(&server), id(id), request(r) {}
    CHS::QueryServer<R, A> *server;
    CHS::QueryId id;
    R request;
  };
  std::list<RequestQueueEntry> _queue;
  CHS::SmartMutex _queue_mutex;
};

} // namespace Smart

#endif
