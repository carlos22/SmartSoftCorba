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
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

#include <cstdio>
#include <cstring>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "linux/joystick.h"

#include "smartJoystickThread.hh"

#include "commJoystick.hh"
#include "commTimeStamp.hh"


using namespace Smart;

JoystickThread::JoystickThread
(
    CHS::SmartComponent &component, 
    CHS::SmartParameter &parameters
)
: _component(component),
  _push_service_name("joystick"),
  _min_interval(0.040),
  _max_interval(0.500),
  _joystick_device("/dev/input/js0"),
  _invert_y_axis(true),
  _verbose(true)
{
  parameters.getString("service","name",_push_service_name);
  parameters.getTruthValue("service","verbose",_verbose);

  parameters.getDouble("interval","min",_min_interval);
  parameters.getDouble("interval","max",_max_interval);

  parameters.getString("hardware","device",_joystick_device);

  parameters.getTruthValue("hardware","invert_y_axis",_invert_y_axis);
}

int JoystickThread::svc()
{
  CHS::StatusCode status;
  CommJoystick js;
  int js_fd;

  static const unsigned int DATA_SIZE = 8;
  js_event js_data[DATA_SIZE];
  CommTimeStamp last_push_time, now;

  static const unsigned int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];

  // try to open joystick device
  js_fd = ::open(_joystick_device.c_str(), O_RDONLY);
  if(js_fd<0)
  {
    std::cerr << "ERROR: failed to open \"" << _joystick_device << "\" as joystick device." << std::endl;
    std::cerr << strerror(errno) << std::endl;
    return -1;
  }
  else if(_verbose)
  {
    if (ioctl(js_fd, JSIOCGNAME(BUFFER_SIZE), buffer) < 0)
    {
      strncpy(buffer, "<unknown>", BUFFER_SIZE);
    }
    std::cout << "Connected to \"" << buffer << "\"." << std::endl;
  }

  // create server object for newest scan push
  CHS::PushNewestServer<Smart::CommJoystick> push_server(&_component, _push_service_name);

  bool data_changed = true;
  while (! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self()))
  {
    fd_set js_fd_set;
    timeval timeout;
    int select_status;

    FD_ZERO(&js_fd_set);
    FD_SET(js_fd, &js_fd_set);
    
    if(data_changed)
    {
      timeout.tv_sec  =  int(::floor(_min_interval));
      timeout.tv_usec = (int(::floor(_min_interval * 1000000)) % 1000000);
    }
    else
    {
      timeout.tv_sec  =  int(::floor(_max_interval));
      timeout.tv_usec = (int(::floor(_max_interval * 1000000)) % 1000000);
    }

    select_status = select(js_fd + 1, &js_fd_set, 0, 0, &timeout);
    
    if(select_status>0)
    {
      int bytes_read = ::read(js_fd, js_data, DATA_SIZE * sizeof(js_event));
      if(bytes_read>0)
      {
        unsigned int num_events_read = bytes_read / sizeof(js_event);
        for(unsigned int i=0; i<num_events_read; ++i)
        {
          if(js_data[i].type & JS_EVENT_BUTTON)
          {
            if(js_data[i].number < 8)
            {
              js.set_button(js_data[i].number, bool(js_data[i].value));
            }
          }
          else if(js_data[i].type & JS_EVENT_AXIS)
          {
            if(js_data[i].number==0)
            {
              js.set_x_int(js_data[i].value);
            }
            else if(js_data[i].number==1)
            {
              if(_invert_y_axis)
              {
                js.set_y_int(-js_data[i].value);
              }
              else
              {
                js.set_y_int(js_data[i].value);
              }
            }
          }
        }
        data_changed = true;
      }
    }
    else
    {
      if(select_status<0)
      {
        std::cerr << "WARNING: select failed." << std::cerr;
      }
    }

    now.set_now();
    if(now.seconds_after(last_push_time)>=_min_interval)
    {
      last_push_time = now;
      data_changed = false;
      status = push_server.put(js);
      if(status!=CHS::SMART_OK)
      {
        std::cerr << "WARNING: error on push (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
      }
      else 
      {
        data_changed = false;
        if(_verbose) std::cout << js << " sent." << std::endl;
      }
    }
  }

  return 0;
}
