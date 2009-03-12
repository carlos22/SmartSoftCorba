// --------------------------------------------------------------------------
//
//  Copyright (C) 2002 Christian Schlegel
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
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#include "commPlannerEvent.hh"

using namespace Smart;

CommPlannerEventParameter::CommPlannerEventParameter()
{
}

CommPlannerEventParameter::~CommPlannerEventParameter()
{
}

void CommPlannerEventParameter::get(CORBA::Any &a) const
{
  a <<= parameter;
}

void CommPlannerEventParameter::set(const CORBA::Any &a)
{
  SmartIDL::PlannerEventParameter *t;

  a >>= t;

  parameter = *t;
}

void CommPlannerEventParameter::set(int r)
{
  parameter.oldState = r;
}

void CommPlannerEventParameter::get(int& r) const
{
  r = parameter.oldState;
}

void CommPlannerEventParameter::print(std::ostream &os) const
{
  os << "Parameter : " << parameter.oldState << std::endl;
}




CommPlannerEventResult::CommPlannerEventResult()
{
}

CommPlannerEventResult::~CommPlannerEventResult()
{
}

void CommPlannerEventResult::get(CORBA::Any &a) const
{
  a <<= result;
}

void CommPlannerEventResult::set(const CORBA::Any &a)
{
  SmartIDL::PlannerEventResult *t;

  a >>= t;

  result = *t;
}

void CommPlannerEventResult::set(int r)
{
  result.state = r;
}

void CommPlannerEventResult::get(int& r) const
{
  r = result.state;
}

void CommPlannerEventResult::get(char* r)const
{
  switch(result.state) {
    case PLANNER_NO_ERROR:
      strcpy(r,"(ok)");
      break;
    case PLANNER_UNKNOWN_ERROR:
      strcpy(r,"(unknown error)");
      break;
    case PLANNER_NO_GOAL_AVAILABLE:
      strcpy(r,"(no goal available)");
      break;
    case PLANNER_GOAL_OK:
      strcpy(r,"(goal ok)");
      break;
    case PLANNER_GOAL_NOT_MARKED:
      strcpy(r,"(goal not marked)");
      break;
    case PLANNER_START_OCCUPIED_OBSTACLE:
      strcpy(r,"(start occupied by obstacle)");
      break;
    case PLANNER_START_OCCUPIED_GOAL:
      strcpy(r,"(start occupied by goal)");
      break;
    case PLANNER_NO_PATH:
      strcpy(r,"(no path)");
      break;
    case PLANNER_PATH_FOUND:
      strcpy(r,"(path found)");
      break;
    case PLANNER_WRONG_MAPID:
      strcpy(r,"(wrong map id)");
      break;
    default:
      strcpy(r,"(error)");
      break;
  }
}

void CommPlannerEventResult::print(std::ostream &os) const
{
  os << "Result : " << result.state << std::endl;
}



PlannerEventState::PlannerEventState()
{
}

PlannerEventState::~PlannerEventState()
{
}

void PlannerEventState::set(int r)
{
  newState = r;
}

void PlannerEventState::get(int& r) const
{
  r = newState;
}

