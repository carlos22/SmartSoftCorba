// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Christian Schlegel, Andreas Steck, Matthias Lutz
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
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

#ifndef SMART_COMM_PLANNER_GOAL_HH
#define SMART_COMM_PLANNER_GOAL_HH

#include <string>
#include <iostream>
#include <math.h>

#include "smartPlannerGoalC.hh"

namespace Smart {

class CommPlannerGoal
{
protected:
  SmartIDL::PlannerGoal goal;

public:

  CommPlannerGoal();
  virtual ~CommPlannerGoal();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommPlannerGoal"; };

  void get_goal(double &xW,double &yW,double &hW,double &xG,double &yG,double &hG,long &id,long &flag); 
  void set_goal(double xW,double yW,double hW,double xG,double yG,double hG,long id,long flag);
  void print(void);
};

} // namespace Smart

#endif
