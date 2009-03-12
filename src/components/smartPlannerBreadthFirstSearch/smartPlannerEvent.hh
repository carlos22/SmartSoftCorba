// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Christian Schlegel, Matthias Lutz, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//        lutz@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft smartPlannerBreadthFirstSearch component".
//  It provides planning services based on grid maps.
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

#ifndef _SMART_PLANNER_EVENT_HH
#define _SMART_PLANNER_EVENT_HH

// ----------------------------------------------------------------------
// constants                                                           
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

#endif

