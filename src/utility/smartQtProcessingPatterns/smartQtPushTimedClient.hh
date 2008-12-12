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

#ifndef SMART_QT_PUSH_NEWEST_CLIENT_HH
#define SMART_QT_PUSH_NEWEST_CLIENT_HH

#include "smartSoft.hh"
#include "smartQtSignallingInterface.hh"

namespace Smart {

/**
  A decorator class for the CHS::PushTimedClient
  which emits a Qt signal whenever new data is available,
  and rejects calls to the blocking getUpdateWait method.
  This class is especially usefull when building graphical user interfaces
  for SmartSoft components with the Qt toolkit.
 */
template<class Data>
class QtPushTimedClient: 
  public CHS::SmartTask, 
  public CHS::PushTimedClient<Data>,
  public QtSignallingInterface
{
public:
  QtPushTimedClient(CHS::SmartComponent *component, const std::string &port, CHS::WiringSlave* slave, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::PushTimedClient<Data>(component, port, slave),
    QtSignallingInterface(client_id),
    _state_mutex(),
    _state_condition(_state_mutex)
  {
    this->open(); // start this->svc() in a separate thread
  }

  QtPushTimedClient(CHS::SmartComponent *component, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::PushTimedClient<Data>(component),
    QtSignallingInterface(client_id),
    _state_mutex(),
    _state_condition(_state_mutex)
  {
    this->open(); // start this->svc() in a separate thread
  }

  QtPushTimedClient(CHS::SmartComponent *component, const std::string &server, const std::string &service, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::PushTimedClient<Data>(component,server,service),
    QtSignallingInterface(client_id),
    _state_mutex(),
    _state_condition(_state_mutex)
  {
    this->open(); // start this->svc() in a separate thread
  }

  ~QtPushTimedClient() throw()
  {
    cancel();
  }
  
  int svc()
  {
    CHS::StatusCode status;
    Data dummy;
    int ret = 0;
    _state_mutex.acquire();
    while(!_state.cancel)
    {
      if(_state.subscribed)
      {
        _state_mutex.release();

        status = CHS::PushTimedClient<Data>::getUpdateWait(dummy);
        if(status==CHS::SMART_OK) signal_new_data();

        _state_mutex.acquire();

        if((status==CHS::SMART_UNSUBSCRIBED) || (status==CHS::SMART_DISCONNECTED) || (status==CHS::SMART_CANCELLED))
        {
          _state.subscribed = false;
        }
        else if(status!=CHS::SMART_OK) // i.e. SMART_ERROR
        {
          _state.cancel = true;
          ret = -1;
        }
      }
      else
      {
        _state_condition.wait();
      }
    }
    _state_mutex.release();
    return ret;
  }

  /**
    The subscribe method now additionally unblocks a thread 
    which waits for new data and causes signals to be emitted.
    \sa Smart::QtSignallingInterface
   */
  inline CHS::StatusCode subscribe(const int interval) throw()
  {
    CHS::StatusCode status = CHS::PushTimedClient<Data>::subscribe(interval);
    if(status==CHS::SMART_OK)
    {
      _state_mutex.acquire();
      _state.subscribed = true;
      _state_condition.signal();
      _state_mutex.release();
    }
    return status;
  }

  /**
    Intercept calls to blocking method.
    Sorry: if you really wanted to use this method, you did not understand the
    concept of this class.
   */
  inline CHS::StatusCode getUpdateWait(Data &d) throw()
  {
    std::cerr << "Warning: Smart::QtPushTimedClient::getUpdateWait() called." << std::endl;
    return CHS::SMART_ERROR;
  }

  /**
    Cancel waiting for new data and force the thread to exit.
   */
  inline void cancel() throw()
  {
    _state_mutex.acquire();
    _state.cancel = true;
    _state_condition.signal();
    _state_mutex.release();
  }
  
private:
  struct State
  {
    inline State() : subscribed(false), cancel(false) {}
    bool subscribed;
    bool cancel;
  };
  State _state;
  CHS::SmartMutex _state_mutex;
  CHS::SmartConditionMutex _state_condition;
};

} // namespace Smart

#endif
