// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Matthias Lutz, Andreas Steck
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

#include "commTrackingGoal.hh"

using namespace Smart;

CommTrackingGoal::CommTrackingGoal()
{
  trackingGoal.angle = 0;
  trackingGoal.distance = 0;
  trackingGoal.x = 0;
  trackingGoal.y = 0;
  trackingGoal.valid = false;
}

CommTrackingGoal::~CommTrackingGoal()
{
}

void CommTrackingGoal::get(CORBA::Any &a) const
{
  a <<= trackingGoal;
}

void CommTrackingGoal::set(const CORBA::Any &a)
{
  SmartIDL::TrackingGoal *tmp;
  if(a >>= tmp)
  {
    trackingGoal = *tmp;
  }
}

void CommTrackingGoal::get(double &_angle, double &_distance, double &_x, double &_y, bool &_valid) const
{
  _angle = trackingGoal.angle;
  _distance = trackingGoal.distance;
  _x = trackingGoal.x;
  _y = trackingGoal.y;
  _valid = trackingGoal.valid;
}

   
void CommTrackingGoal::set( double _angle, double _distance, double _x, double _y, bool _valid )
{
  trackingGoal.angle = _angle;
  trackingGoal.distance = _distance;
  trackingGoal.x = _x;
  trackingGoal.y = _y;
  trackingGoal.valid = _valid;
}

