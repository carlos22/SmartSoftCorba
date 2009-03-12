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

#ifndef _COMM_CDL_GOAL_EVENT_HH
#define _COMM_CDL_GOAL_EVENT_HH

#include <iostream>
#include <string>
#include "smartCdlGoalEventC.hh"

// ----------------------------------------------------------------------
// Values to define the Cdl Goal State
// ----------------------------------------------------------------------
#define CDL_GOAL_UNKNOWN       0
#define CDL_GOAL_REACHED       1
#define CDL_GOAL_NOT_REACHED   2

namespace Smart {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommCdlGoalEventParameter
{
protected:
  SmartIDL::CdlGoalEventParameter parameter;

public:
  CommCdlGoalEventParameter();
  virtual ~CommCdlGoalEventParameter();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::cdlGoalEventParameter";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommCdlGoalEventParameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommCdlGoalEventResult
{
protected:
  SmartIDL::CdlGoalEventResult result;

public:
  CommCdlGoalEventResult();
  virtual ~CommCdlGoalEventResult();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::cdlGoalEventResult";
  };

  void set(int);
  void get(int&) const;
  void get(char* state) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommCdlGoalEventResult &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event state part
//
// --------------------------------------------------------------------------
class CdlGoalEventState
{
protected:
  int newState;

public:
  CdlGoalEventState();
  virtual ~CdlGoalEventState();

  void set(int);
  void get(int&) const;
  void print(void);
};

}

#endif

