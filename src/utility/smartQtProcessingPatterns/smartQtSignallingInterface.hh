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

#ifndef SMART_QT_SIGNALLING_INTERFACE_HH
#define SMART_QT_SIGNALLING_INTERFACE_HH

#include <qobject.h>
#include <qevent.h>
#include <qthread.h>

namespace Smart {

/**
  Interface for classes that emits a Qt signal whenever new data becomes available.

  Note that for generic client decorator templates it is necessary to separate 
  interface and implementation, since Qt does not support slots or signals in 
  class templates.
  \sa Smart::QtPushTimedClient, Smart::QtPushNewestClient
 */
class QtSignallingInterface: public QObject
{
  Q_OBJECT

public:
  /**
    Event handler of this subclass of QObject.
    If the event is an internal event signalling the availability of new data,
    emit an according Qt signal.
    Otherwise, pass the event to the default handler.
   */
  virtual bool event(QEvent *e)
  {
    if(dynamic_cast<NewDataEvent*>(e))
    {
      emit new_data(_client_id);
      emit new_data();
      return true;
    }
    return QObject::event(e);
  }

  /**
    Set the client id which is reported with emitted signals.
   */
  inline void set_client_id(unsigned int id)
  {
    _client_id = id;
  }

  /**
    Get the current client id which is reported with emitted signals.
   */
  inline unsigned int get_client_id() const
  {
    return _client_id;
  }

signals:
  /**
    The Signal which is emitted whenever new data becomes available.
    The client id is included as an argument of the signal.
   */
  void new_data(unsigned int client_id);

  /**
    The Signal which is emitted whenever new data becomes available.
    Use this signal if there is no need to distinguish between different clients.
   */
  void new_data();
  
protected:

  QtSignallingInterface(unsigned int client_id) 
  : _client_id(client_id)
  {
  }

  /**
    Method to be called by instances of the subclasses (template instances)
    when new data becomes available.
   */
  inline void signal_new_data()
  {
    QThread::postEvent(this, new NewDataEvent);
  }

  /**
    The internal event class.
   */
  class NewDataEvent: public QEvent 
  {
  public:
    NewDataEvent() : QEvent(User) {}
  };

private:
  unsigned int _client_id;
};

} // namespace Smart

#endif
