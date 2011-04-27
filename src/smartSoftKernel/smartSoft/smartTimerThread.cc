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


#include "smartTimerThread.hh"

using namespace CHS;

//=============================================================================
// CHS::SyncReleaseTimerHandler
//=============================================================================

TimerHandler::TimerHandler()
{

};

TimerHandler::~TimerHandler()
{

};


//=============================================================================
// CHS::SyncReleaseTimerHandler
//=============================================================================

//=============================================================================
// CHS::TimerThread
//=============================================================================


CHS::TimerThread::TimerThread()
  : ACE_Task <ACE_NULL_SYNCH>(),
    shutting_down_ (0)
{
}

TimerThread::~TimerThread (void)
{
  // shutdown timer thread, if we have one running
  if (thr_count() != 0) {
    // Mark for closing down.
    shutting_down_ = 1;

    // Signal timer event.
    timer_event_.signal ();

    // Wait for the Timer Handler thread to exit.
    thr_mgr()->wait_grp (this->grp_id ());
  }
}

int TimerThread::svc (void)
{
  ACE_Time_Value absolute_time;
  int result = 0;

  while (shutting_down_ == 0)
    {
      // Check whether the timer queue has any items in it.
      if (timer_queue.is_empty () == 0)
        {
          // Get the earliest absolute time.
          absolute_time = timer_queue.earliest_time ();

	  /* ACE 5.2 has not ACE_Event::wait() for relative time.
	   * Since our Timerqueue uses the standart gettimeofday (and
	   * the user cant change this), we are allowed to use the
	   * absolute time from timer_queue.earliest_time() for
	   * ACE_Event::wait().
	   *
	   */
	
          // Block till next timer expires.
          result = timer_event_.wait (&absolute_time);
        }
      else
        // The timer queue has no entries, so wait indefinitely.
        result = timer_event_.wait ();

      // Check for timer expiries.
      if (result == -1)
        {
          switch (errno)
            {
            case ETIME:
              // timeout: expire timers
              timer_queue.expire ();
              break;
            default:
              // Error.
              ACE_ERROR_RETURN ((LM_ERROR,
                                 ACE_LIB_TEXT ("%N:%l:(%P | %t):%p\n"),
                                 ACE_LIB_TEXT ("TimerThread::svc:wait failed")),
                                -1);
            }
        }
    }
  return 0;
}

long
TimerThread::scheduleTimer(TimerHandler &handler,
			   const void *act,
			   const ACE_Time_Value &time,
			   const ACE_Time_Value &interval)
{
  // absolute time.
  ACE_Time_Value absolute_time = timer_queue.gettimeofday () + time;

  // Only one guy goes in here at a time
  ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> guard(timer_queue.mutex());
  if (guard.locked() == 0) {
    return SMART_ERROR;
  }

  // Schedule the timer
  long result = timer_queue.schedule (&handler,
				      act,
				      absolute_time,
				      interval);
  if (result != -1)
  {
    // no failures: check to see if we are the earliest time
    if (timer_queue.earliest_time () == absolute_time)

      // wake up the timer thread
      if (timer_event_.signal () == -1)
      {
	// Cancel timer
	timer_queue.cancel (result);
	result = -1;
      }
  }
  return result;
}

CHS::StatusCode
TimerThread::cancelTimer(long timer_id,
			   const void **arg,
			   bool notifyHandler)
{
  // No need to singal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  int result = timer_queue.cancel (timer_id,
					   arg,
					   !notifyHandler ? 0 : 1);
  if (result == 1) {
    return SMART_OK;
  } else {
    // timer not found
    return SMART_ERROR;
  }
}

int
TimerThread::cancelTimers(TimerHandler &handler,
			  bool notifyHandler)
{
  // No need to signal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  return timer_queue.cancel (&handler,
			     !notifyHandler ? 0 : 1);
}

CHS::StatusCode TimerThread::resetTimerInterval(long timer_id,
						const ACE_Time_Value &interval)
{
  ACE_Time_Value next_wakeup = timer_queue.earliest_time();

  // only one at a time.
  ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> guard(timer_queue.mutex());
  if (guard.locked() == 0) {
    return SMART_ERROR;
  }

  int result = timer_queue.reset_interval(timer_id,
					  interval);
  if (result != 0) {
    // couldn't reset timer interval
    return SMART_ERROR;
  };
  // no failures: check to see if the next wakeup time has changed
  if (timer_queue.earliest_time () == next_wakeup) {
    // wake up the timer thread
    if (timer_event_.signal () == -1)
    {
      // Cancel timer if we couldn't signal our thread
      timer_queue.cancel (result);
      return SMART_ERROR;
    }
  }
  return SMART_OK;
}
