// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Matthias Lutz, Andreas Steck
//
//        lutz@hs-ulm.de
//        steck@hs-ulm.de 
//
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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

#ifndef SMART_COMM_TRACKINGGOAL_HH
#define SMART_COMM_TRACKINGGOAL_HH

#include <string>
#include <iostream>

#include "smartTrackingGoalC.hh"

namespace Smart {

class CommTrackingGoal
{
protected:
  SmartIDL::TrackingGoal trackingGoal;

public:

  CommTrackingGoal();
  virtual ~CommTrackingGoal();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommTrackingGoal"; };

  void get( double &_angle, double &_distance, double &_x, double &_y, bool &_valid) const; 
  void set( double _angle, double _distance, double _x, double _y, bool _valid );
};

} // namespace Smart

#endif
