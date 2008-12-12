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

/** @file smartQtPushTimedServerHandler.hh

  @brief A PushTimed server handler mapping calls to the handler to Qt signals.

  @author Boris Kluge <kluge@faw.uni-ulm.de>

  Using this handler, you can process PushTimed handler calls from within
  your single threaded Qt GUI application.

  For example, you may have a GUI thread

  @code
  class CommData { ... };
  
  class GuiThread: public QObject, public CHS::SmartTask
  {
    Q_OBJECT
  public:
    GuiThread(Smart::QtPushTimedServerHandler<CommData> &push_timed_handler)
    : _push_timed_handler(push_timed_handler)
    {
    }

    int svc();
    //...

  public slots:
    void handle_push_timed();

  private:
    Smart::QtPushTimedServerHandler<CommData> &_push_timed_handler;
  };
  @endcode

  In your \c main() function you create handlers, client objects and threads
  @code
  int main(int argc, char *argv[])
  {
    std::string server_name = "...";
    std::string push_timed_service_name = "...";

    // ...

    CHS::SmartComponent component(server_name,argc,argv);

    //...

    Smart::QtPushTimedServerHandler<CommData> push_timed_handler;

    GuiThread gui_thread(push_timed_handler);

    CHS::PushTimedServer<CommData> push_timed_server(&component,push_timed_service_name,push_timed_handler);

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
    QObject::connect(&_push_timed_handler, SIGNAL(new_push_timed()), this, SLOT(handle_push_timed()));
    // ...
    return qapp.exec();
  }
  @endcode

  The code which actually handles the call backs goes into the slot method:
  @code
  void GuiThread::handle_push_timed()
  {
    CommData data;
    data = ... // fill with current data
    CHS::PushTimedServer<CommData> *push_timed_server;
    while(_push_timed_handler.pop(&push_timed_server))
    {
      push_timed_server->put(data);
    }
  }
  @endcode
*/

#ifndef SMART_QT_PUSH_TIMED_SERVER_HANDLER_HH
#define SMART_QT_PUSH_TIMED_SERVER_HANDLER_HH

#include "smartSoft.hh"

#include <qobject.h>
#include <qevent.h>
#include <qthread.h>

namespace Smart {

/**
  The common base class for Qt signalling PushTimed server handlers.
  There are several reasons for having this base class. Firstly, 
  Qt slots/signals and templates hardly mix, and secondly, we can factor
  out common code in order to decrease template bloating.
 */
class QtPushTimedServerHandlerInterface: public QObject
{
  Q_OBJECT

public:
  /**
    Internal event method.
    This method is called from within the Qt thread.
    Overwritten from QObject, checks if the event is actually an internal 
    NewPushTimedEvent, and, if so, emits a signal.
   */
  virtual bool event(QEvent *e)
  {
    if(dynamic_cast<NewPushTimedEvent*>(e))
    {
      emit new_push_timed();
      return true;
    }
    return QObject::event(e);
  }

signals:

  /**
    The signal which is emitted as response to calls to the server handler.
   */
  void new_push_timed();
  
protected:

  /**
    Protected constructor. Users must create instances of derived classes only.
   */
  QtPushTimedServerHandlerInterface() {}

  /**
    Internal method which is called from the SmartSoft thread
    from within the server handler.
    The method posts a Qt event to this method's object, which is (in contrast
    to signals/slots) thread-safe.
   */
  inline void signal_new_push_timed()
  {
    QThread::postEvent(this, new NewPushTimedEvent);
  }

  /**
    The internal event class.
   */
  class NewPushTimedEvent: public QEvent 
  {
  public:
    NewPushTimedEvent() : QEvent(User) {}
  };
};

/**
  Class template for Qt signalling PushTimed handlers.
 */
template<class C>
class QtPushTimedServerHandler: 
  public CHS::PushTimedHandler<C>,
  public QtPushTimedServerHandlerInterface
{
public:

  /**
    Default constuctor.
    No arguments are needed since anything interesting happens
    where the signals of this handler object are received.
   */
  QtPushTimedServerHandler() {}

  /**
    The overwritten handler method.
    It pushed the reference to the calling server onto a queue
    and sends the Qt thread an event by calling \c signal_new_push_timed().
   */  
  void handlePushTimer(CHS::PushTimedServer<C> &server) throw()
  {
    _queue_mutex.acquire();
    _queue.push_back(&server);
    _queue_mutex.release();
    signal_new_push_timed();
  }

  /**
    Get the server which called the handler.
    This method can be used in slot methods which handle the Qt signal
    in order to recover the reference to the server which wants to push
    updated data to its clients.
    The method expect the address of a pointer to a server as its argument.
    If the argument is non-null, the pointer to the server is returned
    in the argument.
    The method returns \c true, if the handler was called.
   */
  inline bool pop(CHS::PushTimedServer<C> **server = 0)
  {
    bool ret = false;
    _queue_mutex.acquire();
    if(!_queue.empty())
    {
      if(server) 
      {
        *server = _queue.front();
      }
      _queue.pop_front();
      ret = true;
    }
    _queue_mutex.release();
    return ret;
  }

private:
  std::list<CHS::PushTimedServer<C>*> _queue;
  CHS::SmartMutex _queue_mutex;
};

} // namespace Smart

#endif
