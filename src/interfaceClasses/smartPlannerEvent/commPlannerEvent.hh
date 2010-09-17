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

#ifndef _COMM__PLANNER_EVENT_HH
#define _COMM__PLANNER_EVENT_HH

#include <iostream>
#include <string>
#include "smartPlannerEventC.hh"

// ----------------------------------------------------------------------
// Values to define the planner State
// ----------------------------------------------------------------------
#define PLANNER_NO_ERROR                0   // everything ok
#define PLANNER_UNKNOWN_ERROR           1   // so far unknown problem
#define PLANNER_NO_GOAL_AVAILABLE       2   // no goal lines available
#define PLANNER_GOAL_OK                 3   // at least one goal cell labeled
#define PLANNER_GOAL_NOT_MARKED         4   // every goal cell already occupied
#define PLANNER_START_OCCUPIED_OBSTACLE 5   // start cell occupied by obstacle
#define PLANNER_START_OCCUPIED_GOAL     6   // start cell occupied by goal cell
#define PLANNER_NO_PATH                 7   // no path found
#define PLANNER_PATH_FOUND              8   // valid path found
#define PLANNER_WRONG_MAPID             9   // map id does not match
#define PLANNER_UNKNOWN                10   // unknown


namespace Smart {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommPlannerEventParameter
{
protected:
  SmartIDL::PlannerEventParameter parameter;

public:
  CommPlannerEventParameter();
  virtual ~CommPlannerEventParameter();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::plannerEventParameter";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommPlannerEventParameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommPlannerEventResult
{
protected:
  SmartIDL::PlannerEventResult result;

public:
  CommPlannerEventResult();
  virtual ~CommPlannerEventResult();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::plannerEventResult";
  };

  void set(int);
  void get(int&) const;
  void get(char* state) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommPlannerEventResult &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event state part
//
// --------------------------------------------------------------------------
class PlannerEventState
{
protected:
  int newState;

public:
  PlannerEventState();
  virtual ~PlannerEventState();

  void set(int);
  void get(int&) const;
};

}

#endif

