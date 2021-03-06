//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.2
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Manuel Wopfner
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Mapper/GridMapper component".
//  It provides mapping services based on grid maps. The current map
//  represents the latest snapshot of the local surrounding based on
//  laserscans. The current map can be preoccupied by the longterm map.
//  The longterm map is a simple occupancy grid map.
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


#include "GoalEventTestHandler.hh"
#include "gen/SmartPTUServer.hh"

#include <iostream>

bool GoalEventTestHandler::testEvent(CommPTUObjects::CommPTUGoalEventParameter &p,
		CommPTUObjects::CommPTUGoalEventResult &r, const CommPTUObjects::PTUGoalEventState &s) throw()
{
	bool result = false;

	CommPTUObjects::PTUMoveStatus oldState, newState;

	oldState = p.get_state();
	newState = s.get_state();

	if (oldState != newState)
	{
		p.set_state(newState);
		r.set_state(newState);
		std::cout << "[GoalEventTestHandler] true!" << std::endl;
		result = true;
	} else
	{
		//std::cout << "[GoalEventTestHandler] false!" << std::endl;
		result = false;
	}

	return result;
}
