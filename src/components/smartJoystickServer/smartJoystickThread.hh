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

#ifndef SMART_JOYSTICK_THREAD_HH
#define SMART_JOYSTICK_THREAD_HH

#include <list>
#include <vector>

#include "smartSoft.hh"
#include "commJoystick.hh"

namespace Smart {

class JoystickThread: public CHS::SmartTask
{
public:

  JoystickThread
  (
    CHS::SmartComponent &component, 
    CHS::SmartParameter &parameters
  );
    
  int svc();

private:

  CHS::SmartComponent &_component;
  std::string _push_service_name;
  double _min_interval;
  double _max_interval;
  std::string _joystick_device;
  bool _invert_y_axis;

  bool _verbose;
};

} // namespace Smart

#endif
