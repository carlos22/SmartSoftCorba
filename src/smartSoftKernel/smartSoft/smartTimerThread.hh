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

#ifndef _SMARTTIMERTHREAD_HH
#define _SMARTTIMERTHREAD_HH


class ACE_Handler;

#include <smartErrno.hh>
#include <ace/Task.h>
#include <ace/Synch.h>
#include <ace/Timer_Heap.h>
#include <ace/Time_Value.h>

namespace CHS {

  /** Interface to receive notification from a TimerThread.
   *
   *  handler methods (hooks) are not abstract -> the user is not forced to
   *  provide handler methods he does not need.
   */
  class TimerHandler {
  public:
    TimerHandler();
    virtual ~TimerHandler();

    /** hook called on timer expiration
     */
    virtual void timerExpired(const ACE_Time_Value & absolute_time,
			      const void * arg)
      { };

    /// This method is called when the timer is cancelled
    virtual void timerCanceled() {};

    /// This method is called when the timer queue is destroyed and
    /// the timer is still contained in it
    virtual void timerDeleted(const void * arg) {};
  };

  /** Functor for Timer_Queues.
   * 
   *  @internal
   * 
   * This class implements the functor required by ACE_Timer_Queue
   * to call the Handler method in a TimerHandler Object
   */
  template <class ACE_LOCK>
  class TimerHandlerHandleUpcall
  {
  public:
    typedef ACE_Timer_Queue_T<TimerHandler *,
                              TimerHandlerHandleUpcall<ACE_LOCK>,
                              ACE_LOCK>
    TIMER_QUEUE;

    // = Initialization and termination methods.
    /// Constructor.
    TimerHandlerHandleUpcall () {};

    /// Destructor.
    ~TimerHandlerHandleUpcall () {};

    /// This method is called when the timer expires
    int timeout (TIMER_QUEUE &timer_queue,
		 TimerHandler *handler,
		 const void *arg,
		 int recurring_timer,
		 const ACE_Time_Value &cur_time)
      {
	// Upcall to the <handler>s handle_timeout method.
	handler->timerExpired (cur_time, arg);
	return 0;
      };


    /// This method is called when the timer is cancelled
    int cancel_timer (TIMER_QUEUE &timer_queue,
		      TimerHandler *handler,
                      int dont_call_handle_close,
                      int requires_reference_counting)
      {
	handler->timerCanceled ();
	return 0;
      };

    /// This method is called when the timer queue is destroyed and
    /// the timer is still contained in it
    int deletion (TIMER_QUEUE &timer_queue,
		  TimerHandler *handler,
		  const void *arg)
      {
	handler->timerDeleted(arg);
	return 0;
      }

    /// This method is called when a time is registered
    int registration (TIMER_QUEUE &timer_queue,
                      TimerHandler *handler,
                      const void *arg)
    {
      return 0;
    }

    /// This method is called before the timer expires
    int preinvoke (TIMER_QUEUE &timer_queue,
                   TimerHandler *handler,
                   const void *arg,
                   int reccuring_timer,
                   const ACE_Time_Value &cur_time,
                   const void *upcall_act)
    {
      return 0;
    }

    /// This method is called after the timer expires
    int postinvoke (TIMER_QUEUE &timer_queue,
                    TimerHandler *handler,
                    const void *arg,
                    int recurring_timer,
                    const ACE_Time_Value &cur_time,
                    const void *upcall_act)
    {
      return 0;
    }

    /// This method is called when a handler is canceled
    int cancel_type (TIMER_QUEUE &timer_queue,
                     TimerHandler *handler,
                     int dont_call_handle_close,
                     int &requires_reference_counting)
    {
      return 0;
    }

  private:
    // = Don't allow these operations for now.
    ACE_UNIMPLEMENTED_FUNC (TimerHandlerHandleUpcall (const TimerHandlerHandleUpcall<ACE_LOCK> &));
    ACE_UNIMPLEMENTED_FUNC (void operator= (const TimerHandlerHandleUpcall<ACE_LOCK> &));
  };


  typedef ACE_Timer_Heap_T<TimerHandler *,
                           TimerHandlerHandleUpcall<ACE_SYNCH_RECURSIVE_MUTEX>,
                           ACE_SYNCH_RECURSIVE_MUTEX>
  TimerHeap;

  /** single/continuous timer.
   *
   *  This object has a thread that waits on the earliest time in
   *  a list of timers and an event. When a timer expires, the
   *  apropriate handler is called in the context of this thread.
   *
   *  It is based on ACE_Time_Heap and friends.
   *
   *  Some code taken from ace/Proactor.cpp
   */
  class TimerThread : public ACE_Task<ACE_NULL_SYNCH>
  {

  public:
    /// Constructor.
    TimerThread();

    /// Destructor. Properly shuts down the timer thread
    virtual ~TimerThread ();

    /** Schedule a timer.
     *
     *  @param  handler  The handler that will be called when the timer
     *                   expires.
     *  @param  act      a value that will be passed to the handler.
     *  @param  time     relative time for the first timer expiration
     *  @param  interval Interval for periodic timers. A single shot timer
     *                   is scheduled by default.
     *
     * @return timer_id: -1 on failure. Unique time id else. This id
     *                     can be used to cancel a timer before it
     *                     expires with cancelTimer() and to change
     *                     the the interval of a timer with
     *                     resetTimerInterval().
     */
    long scheduleTimer(TimerHandler &handler,
		       const void *act,
		       const ACE_Time_Value &time,
		       const ACE_Time_Value &interval = ACE_Time_Value::zero);


    /** Cancel a single timer.
     *
     *  @param  timer_id   to cancel
     *  @param  act        pointer to retrive the act that was given on
     *                     scheduleTimer(). Can be used to release ressources
     *                     owned by act. If act == 0, nothing is retrieved.
     *  @param notifyHandler
     *                     true: calls timerCancelled() hook on the associated handler
     *                     false: do not call timerCancelled() hook
     */
    StatusCode cancelTimer(long timer_id,
			   const void **act=0,
			   bool notifyHandler=true);

    /** Cancel all timers associated with a handler
     *
     *  @param handler     cancel timers associated with this handler
     *  @param notifyHandler
     *                     true: calls timerCancelled() hook on the associated handler
     *                     false: do not call timerCancelled() hook
     *
     *  @return number of timers canceled.
     */
    int cancelTimers(TimerHandler &handler,
		     bool notifyHandler=true);


    /** Resets the interval of a timer.
     *
     *  @param timer_id     to change
     *  @param interval     new timer interval (relative to the current time)
     */
    StatusCode resetTimerInterval(long timer_id,
				  const ACE_Time_Value &interval);

  protected:
    /// Run by a daemon thread to handle deferred processing. In other
    /// words, this method will do the waiting on the earliest timer and
    /// event.
    virtual int svc (void);

    TimerHeap timer_queue;

    /// Event to wait on.
    ACE_Auto_Event timer_event_;

    /// Flag used to indicate when we are shutting down.
    int shutting_down_;
  };

}

#endif // _SMARTTIMERTHREAD_HH
