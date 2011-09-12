//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
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

#ifndef _COMM_COMMTRACKINGOBJECTS_COMMTRACKINGGOAL_HH
#define _COMM_COMMTRACKINGOBJECTS_COMMTRACKINGGOAL_HH

#include <string>
#include <iostream>


// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommTrackingObjects/gen/smartCommTrackingGoalC.hh"

namespace CommTrackingObjects
{

class CommTrackingGoal
{
protected:
	CommTrackingObjectsIDL::CommTrackingGoal idl_CommTrackingGoal;

public:
	CommTrackingGoal()
	{
		idl_CommTrackingGoal.angle = 0;
		idl_CommTrackingGoal.distance = 0;
		idl_CommTrackingGoal.x = 0;
		idl_CommTrackingGoal.y = 0;
		idl_CommTrackingGoal.valid = false;
	}

	CommTrackingGoal(const CommTrackingObjectsIDL::CommTrackingGoal &obj) :
		idl_CommTrackingGoal(obj)
	{
	}

	operator CommTrackingObjectsIDL::CommTrackingGoal() const
	{
		return idl_CommTrackingGoal;
	}

	virtual ~CommTrackingGoal()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommTrackingObjects::CommTrackingGoal";
	}

	//
	// user interfaces

	void get( double &_angle, double &_distance, double &_x, double &_y, bool &_valid) const;
	void set( double _angle, double _distance, double _x, double _y, bool _valid );
};

}
#endif