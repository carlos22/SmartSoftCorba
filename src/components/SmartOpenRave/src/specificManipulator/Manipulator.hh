//--------------------------------------------------------------------------
//  Copyright (C) 2010 Jonas Brich
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartOpenRave component".
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
//---------------------------------------------------------------------

#ifndef _MANIPULATOR_HH
#define _MANIPULATOR_HH

#include <vector>

namespace SpecificManipulator {

/**
 * Abstract class which interacts as an interface for all manipulator specific code.
 *
 * Each manipulator specific class must implement this class with all methods.
 */
class Manipulator {

public:

	/**
	 * Converts real joint angles into OpenRave joint angles.
	 *
	 * realAngles:		vector which holds all the real joint angles
	 * openRaveAngles:	vector which holds the converted joint angles
	 *
	 * The size of the vectors must be the same.
	 */
	virtual bool convertRealAnglesToOpenRaveAngles(const std::vector<double>& realAngles, std::vector<double>& openRaveAngles) = 0;

	/**
	 * Converts OpenRave joint angles into real joint angles.
	 *
	 * openRaveAngles:	vector which holds all the OpenRave joint angles
	 * realAngles:		vector which holds the converted joint angles
	 *
	 * The size of the vectors must be the same.
	 */
	virtual bool convertOpenRaveAnglesToRealAngles(const std::vector<double>& openRaveAngles, std::vector<double>& realAngles) = 0;

	/**
	 * Returns the active DoFs of the manipulator.
	 * This method will return the DoFs without the gripper.
	 *
	 * dofs: vector which will include the active dofs
	 */
	virtual void getActivDofsWithoutGripper(std::vector<int>& dofs) = 0;

	/**
	 * Returns the active DoFs of the manipulator.
	 * This method will return the DoFs with the gripper.
	 *
	 * dofs: vector which will include the active dofs
	 */
	virtual void getActivDofsWithGripper(std::vector<int>& dofs) = 0;

	/**
	 * Removes dummy joint angles
	 *
	 * angles: this angles will be modified. After the method has return with true the dummy joint angle is removed.
	 *
	 * return: true if the dummy joint angles could be removed otherwise false.
	 */
	virtual bool removeDummyJointAngles(std::vector<double>& angles) = 0;

	/**
	 * Returns the offset of the real manipulator coordinate system to the OpenRave manipulator coordinate system
	 *
	 * x:	offset x-coordinate
	 * y:	offset y-coordinate
	 * z:	offset z-coordinate
	 */
	virtual void getOffsetRealManipulatortoOpenRaveManipulator(double& x, double& y, double& z) = 0;

	/**
	 * Returns the closed gripper angle of OpenRave.
	 */
	double getClosedGripperAngleOpenRave() {
		return closedAngleOpenRave;
	}

	/**
	 * Returns the open gripper angle of OpenRave.
	 */
	virtual double getOpenGripperAngleOpenRave() {
		return openAngleOpenRave;
	}

	virtual ~Manipulator() {
	}

protected:
	/**
	 * These variables are there to match the OpenRave gripper angles to the real gripper angles.
	 */
	double closedAngleOpenRave;
	double openAngleOpenRave;
	double closedAngleReal;
	double openAngleReal;
};

}

#endif /* _MANIPULATOR_HH */
