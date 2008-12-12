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
// --------------------------------------------------------------------------

/** @file smartQtSendServerHandler.hh

  @brief A Send<C> server handler mapping calls to the handler to Qt signals.

  @author Boris Kluge <kluge@faw.uni-ulm.de>

  Using this handler, you can process reception of sent data from within
  your single threaded Qt GUI application.

  For example, you may have a GUI thread
  @code
  class CommData { ... };
  
  class GuiThread: public QObject, public CHS::SmartTask
  {
    Q_OBJECT
  public:
    GuiThread(Smart::QtSendServerHandler<CommData> &send_handler)
    : _send_handler(send_handler)
    {
    }

    int svc();
    //...

  public slots:
    void handle_send();

  private:
    Smart::QtSendServerHandler<CommData> &_send_handler;
  };
  @endcode

  In your \c main() function you create handlers, client objects and threads as usually
  @code
  int main(int argc, char *argv[])
  {
    std::string server_name = "...";
    std::string send_service_name = "...";

    // ...

    CHS::SmartComponent component(server_name,argc,argv);

    //...

    Smart::QtSendServerHandler<CommData> send_handler;

    GuiThread gui_thread(send_handler);

    CHS::SendServer<CommData> send_server(&component,send_service_name,send_handler);

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
    QObject::connect(&_send_handler, SIGNAL(new_send()), this, SLOT(handle_send()));
    // ...
    return qapp.exec();
  }
  @endcode

  The code which actually handles the callbacks goes into the slot method:
  @code
  void GuiThread::handle_send()
  {
    CommData data;
    while(_send_handler.pop(data))
    {
      // do something interesting with the received data
    }
  }
  @endcode
*/

#ifndef SMART_QT_SEND_SERVER_HANDLER_HH
#define SMART_QT_SEND_SERVER_HANDLER_HH

#include "smartSoft.hh"

#include <qobject.h>
#include <qevent.h>
#include <qthread.h>

namespace Smart {

/**
  The common base class for Qt signalling Send server handlers.
  There are several reasons for having this base class. Firstly, 
  Qt slots/signals and templates hardly mix, and secondly, we can factor
  out common code in order to decrease template bloating.
 */
class QtSendServerHandlerInterface: public QObject
{
  Q_OBJECT

public:
  /**
    Internal event method.
    This method is called from within the Qt thread.
    Overwritten from QObject, checks if the event is actually an internal 
    NewSendEvent, and, if so, emits a signal.
   */
  virtual bool event(QEvent *e)
  {
    if(dynamic_cast<NewSendEvent*>(e))
    {
      emit new_send();
      return true;
    }
    return QObject::event(e);
  }

signals:

  /**
    The signal which is emitted as response to calls to the server handler.
   */
  void new_send();
  
protected:

  /**
    Protected constructor. Users must create instances of derived classes only.
   */
  QtSendServerHandlerInterface() {}

  /**
    Internal method which is called from the SmartSoft thread
    from within the server handler.
    The method posts a Qt event to this method's object, which is (in contrast
    to signals/slots) thread-safe.
   */
  inline void signal_new_send()
  {
    QThread::postEvent(this, new NewSendEvent);
  }

  /**
    The internal event class.
   */
  class NewSendEvent: public QEvent 
  {
  public:
    NewSendEvent() : QEvent(User) {}
  };
};

/**
  Class template for Qt signalling Send handlers.
 */
template<class C>
class QtSendServerHandler: 
  public CHS::SendServerHandler<C>,
  public QtSendServerHandlerInterface
{
public:

  /**
    Default constuctor.
    No arguments are needed since anything interesting happens
    where the signals of this handler object are received.
   */
  QtSendServerHandler() {}
  
  /**
    The overwritten handler method.
    It pushed the reference to the calling server and the sent data onto a queue,
    and sends the Qt thread an event by calling \c signal_new_push_timed().
   */  
  void handleSend(const C &cmd) throw()
  {
    _queue_mutex.acquire();
    _queue.push_back(SendQueueEntry(cmd));
    _queue_mutex.release();
    signal_new_send();
  }

  /**
    Get the data which the server received.
    This method can be used in slot methods which handle the Qt signal
    in order to recover the data which the server received.
    The method returns the data through the reference argument.
    The method returns \c true, if the handler was called.
   */
  inline bool pop(C &cmd)
  {
    bool ret = false;
    _queue_mutex.acquire();
    if(!_queue.empty())
    {
      cmd = _queue.front().command;
      _queue.pop_front();
      ret = true;
    }
    _queue_mutex.release();
    return ret;
  }

private:
  /**
    Class of the queue entries.
   */
  struct SendQueueEntry
  {
    SendQueueEntry() : command() {}
    SendQueueEntry(const C &c) : command(c) {}
    C command;
  };
  std::list<SendQueueEntry> _queue;
  CHS::SmartMutex _queue_mutex;
};

} // namespace Smart

#endif
