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

#include "commCdlGoalEvent.hh"

using namespace Smart;

CommCdlGoalEventParameter::CommCdlGoalEventParameter()
{
}

CommCdlGoalEventParameter::~CommCdlGoalEventParameter()
{
}

void CommCdlGoalEventParameter::get(CORBA::Any &a) const
{
  a <<= parameter;
}

void CommCdlGoalEventParameter::set(const CORBA::Any &a)
{
  SmartIDL::CdlGoalEventParameter *t;

  a >>= t;

  parameter = *t;
}

void CommCdlGoalEventParameter::set(int r)
{
  parameter.oldState = r;
}

void CommCdlGoalEventParameter::get(int& r) const
{
  r = parameter.oldState;
}

void CommCdlGoalEventParameter::print(std::ostream &os) const
{
  os << "Parameter : " << parameter.oldState << std::endl;
}




CommCdlGoalEventResult::CommCdlGoalEventResult()
{
}

CommCdlGoalEventResult::~CommCdlGoalEventResult()
{
}

void CommCdlGoalEventResult::get(CORBA::Any &a) const
{
  a <<= result;
}

void CommCdlGoalEventResult::set(const CORBA::Any &a)
{
  SmartIDL::CdlGoalEventResult *t;

  a >>= t;

  result = *t;
}

void CommCdlGoalEventResult::set(int r)
{
  result.state = r;
}

void CommCdlGoalEventResult::get(int& r) const
{
  r = result.state;
}

void CommCdlGoalEventResult::get(char* r)const
{
  switch(result.state) {
    case CDL_GOAL_UNKNOWN:
      strcpy(r,"(unknown)");
      break;
    case CDL_GOAL_REACHED:
      strcpy(r,"(reached)");
      break;
    case CDL_GOAL_NOT_REACHED:
      strcpy(r,"(notreached)");
      break;
    default:
      strcpy(r,"(error)");
      break;
  }
}

void CommCdlGoalEventResult::print(std::ostream &os) const
{
  os << "Result : " << result.state << std::endl;
}



CdlGoalEventState::CdlGoalEventState()
{
}

CdlGoalEventState::~CdlGoalEventState()
{
}

void CdlGoalEventState::set(int r)
{
  newState = r;
}

void CdlGoalEventState::get(int& r) const
{
  r = newState;
}

void CdlGoalEventState::print(void)
{
 std::cout<< "CdlGoalEventState - state: " << newState << "\n"; 
}
