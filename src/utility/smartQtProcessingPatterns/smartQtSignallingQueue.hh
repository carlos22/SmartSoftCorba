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

#ifndef SMART_QT_SIGNALLING_QUEUE_HH
#define SMART_QT_SIGNALLING_QUEUE_HH

#include <list>

#include "smartSoft.hh"

#include "smartQtSignallingInterface.hh"

namespace Smart {

/**
  A thread safe queue which emits a Qt signal whenever new data is enqueued.
 */
template<class Data>
class QtSignallingQueue: public QtSignallingInterface
{
public:

  /**
    Constructor.
    The queue ID is passed as argument of each \c new_data signal,
    such that a single slot can distinguish and handle the signals from
    multiple queues.
   */
  QtSignallingQueue(unsigned int queue_id = 0)
  : QtSignallingInterface(queue_id)
  {
  }
  
  /**
    Enqueue new data.
    Acquires the mutex for the queue (implemented by an STL list),
    appends the data, releases the mutex, and signals the parent class
    about new data.
   */
  inline void enqueue(const Data &data)
  {
    _queue_mutex.acquire();
    _queue.push_back(data);
    _queue_mutex.release();
    signal_new_data();
  }

  /**
    Try to dequeue data from the queue.
    If the queue is empty, the method returne \c false without touching the
    \a data argument. Otherwise, the current head of the queue is removed and copied
    to \a data, and the method returns \c true.
   */
  inline bool dequeue(Data &data)
  {
    bool ret = false;
    _queue_mutex.acquire();
    if(!_queue.empty())
    {
      data = _queue.front();
      _queue.pop_front();
      ret = true;
    }
    _queue_mutex.release();
    return ret;
  }

private:
  /**
    Implementation of the queue.
   */
  std::list<Data> _queue;

  /**
    Mutex for serialization of queue accesses.
   */
  CHS::SmartMutex  _queue_mutex;
};

} // namespace Smart

#endif
