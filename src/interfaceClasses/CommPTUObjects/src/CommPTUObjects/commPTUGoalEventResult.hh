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
//  Copyright (C) 2010 Manuel Wopfner
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
// --------------------------------------------------------------------------

#ifndef _COMM_COMMPTUOBJECTS_COMMPTUGOALEVENTRESULT_HH
#define _COMM_COMMPTUOBJECTS_COMMPTUGOALEVENTRESULT_HH

#include <string>

// include files of classes

// include header-files of included communication objects

// include enums
#include <CommPTUObjects/gen/enumPTUMoveStatus.hh>

// include client-side file generated by IDL compiler
#include "CommPTUObjects/gen/smartCommPTUGoalEventResultC.hh"

namespace CommPTUObjects {

class CommPTUGoalEventResult {
protected:
	CommPTUObjectsIDL::CommPTUGoalEventResult idl_CommPTUGoalEventResult;

public:
	CommPTUGoalEventResult() {
	}

	CommPTUGoalEventResult(const CommPTUObjectsIDL::CommPTUGoalEventResult &obj) :
		idl_CommPTUGoalEventResult(obj) {
	}

	virtual ~CommPTUGoalEventResult() {
	}

	operator CommPTUObjectsIDL::CommPTUGoalEventResult() const {
		return idl_CommPTUGoalEventResult;
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void) {
		return "CommPTUObjects::CommPTUGoalEventResult";
	}

	/**
	 * Set the status of the PTU action. (Success, Failure, etc.)
	 */
	inline void set_state(CommPTUObjects::PTUMoveStatus state) {
		idl_CommPTUGoalEventResult.state = state.get_value_idl();
	}

	/**
	 * Get the status of the PTU action. (Success, Failure, etc.)
	 */
	inline PTUMoveStatus get_state() const {
		return idl_CommPTUGoalEventResult.state;
	}


	/**
	 * Get the status of the PTU action. (Success, Failure, etc.)
	 */
	inline void get(char* r) const
	{
		switch (idl_CommPTUGoalEventResult.state)
		{
		case CommPTUObjectsIDL::LITERAL_FAILURE:
			strcpy(r, "(FAILURE)");
			break;

		case CommPTUObjectsIDL::LITERAL_GOAL_NOT_REACHED:
			strcpy(r, "(GOALNOTREACHED)");
			break;

		case CommPTUObjectsIDL::LITERAL_GOAL_REACHED:
			strcpy(r, "(GOALREACHED)");
			break;

		case CommPTUObjectsIDL::LITERAL_HALTED:
			strcpy(r, "(HALTED)");
			break;

		case CommPTUObjectsIDL::LITERAL_PAN_TILT_OUT_OF_RANGE:
		case CommPTUObjectsIDL::LITERAL_TILT_OUT_OF_RANGE:
		case CommPTUObjectsIDL::LITERAL_PAN_OUT_OF_RANGE:
			strcpy(r, "(OUTOFRANGE)");
			break;

		default:
			strcpy(r, "(UNKNOWN)");
			break;
		}
	}

};

}
#endif
