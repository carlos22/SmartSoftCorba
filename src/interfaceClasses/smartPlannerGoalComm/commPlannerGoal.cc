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

#include "commPlannerGoal.hh"

using namespace Smart;

CommPlannerGoal::CommPlannerGoal()
{
  goal.id  = 0;
  goal.flag = 1;
  goal.xWayPoint = 0.0;
  goal.yWayPoint = 0.0;
  goal.hWayPoint = 0.0;
  goal.xGoalPoint = 0.0;
  goal.yGoalPoint = 0.0;
  goal.hGoalPoint = 0.0;
}

CommPlannerGoal::~CommPlannerGoal()
{
}

void CommPlannerGoal::get(CORBA::Any &a) const
{
  a <<= goal;
}

void CommPlannerGoal::set(const CORBA::Any &a)
{
  SmartIDL::PlannerGoal *tmp_goal;
  if(a >>= tmp_goal)
  {
    goal = *tmp_goal;
  }
}

void CommPlannerGoal::get_goal(double &xW,double &yW,double &hW,double &xG,double &yG,double &hG,long &id,long &flag) 
{
  xW   = goal.xWayPoint;
  yW   = goal.yWayPoint;
  hW   = goal.hWayPoint;
  xG   = goal.xGoalPoint;
  yG   = goal.yGoalPoint;
  hG   = goal.hGoalPoint;
  id   = goal.id;
  flag = goal.flag;
  //std::cout << "GET Planner Goal from Comm object: "<<std::endl;
  //print();
}
void CommPlannerGoal::set_goal(double xW,double yW,double hW,double xG,double yG,double hG,long id,long flag)
{
  goal.xWayPoint  = xW;
  goal.yWayPoint  = yW;
  goal.hWayPoint  = hW;
  goal.xGoalPoint = xG;
  goal.yGoalPoint = yG;
  goal.hGoalPoint = hG;
  goal.id         = id;
  goal.flag       = flag;
}

void CommPlannerGoal::print(void) {
    std::cout << "Planner Way Point  x y h [mm,mm,deg] " << goal.xWayPoint << " " << goal.yWayPoint << " " << goal.hWayPoint*180.0/M_PI << "\n";
    std::cout << "Planner Goal Point x y h [mm,mm,deg] "<< goal.xGoalPoint << " " << goal.yGoalPoint << " " << goal.hGoalPoint*180.0/M_PI << "\n";
    std::cout << "Planner id flag " << goal.id << " " << goal.flag << "\n";
  }

