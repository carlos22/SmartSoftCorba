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

#include <math.h>
#include "Katana.hh"
#include "util/ErrorHandler.hh"
#include "gen/SmartOpenRave.hh"

namespace SpecificManipulator {

Katana::Katana() {
	// TODO: possible HACK? WTH is this?
	// this offset is required as we measure the position of katana from the first
	// joint to floor and not from base to floor as it is estimated here. we thus
	// need to substract z later to get this distance.
	this->manipulatorCoord_x = 0;
	this->manipulatorCoord_y = 0;
	//this->manipulatorCoord_z = 0.215;

	// The offset of 21.5cm is the offset between coordinates of robot and open rave.
	// 12mm is to correct a difference between the ZAFH Ulm Manipulator and the model in Open Rave
	this->manipulatorCoord_z = 0.215 - 0.012;

	//	this->closedAngleOpenRave = -0.436;
	this->closedAngleOpenRave = -0.39;
	this->openAngleOpenRave = 0.524;
	this->closedAngleReal = 0.365483;
	this->openAngleReal = 2.11552;

	// This calculations has to been done, to scale the real angle to the OpenRave angle from the gripper.
	// The range and the scale of the real angle of the gripper is bigger than OpenRave.
	this->center = abs(closedAngleReal) - (abs(closedAngleReal) + abs(openAngleReal)) / 2;
	this->offset = abs(closedAngleOpenRave) - (abs(closedAngleOpenRave) + abs(openAngleOpenRave)) / 2;
	this->factor = (-1.0 * closedAngleReal + center) / (closedAngleOpenRave + offset);
}

bool Katana::convertRealAnglesToOpenRaveAngles(const std::vector<double>& realAngles, std::vector<double>& openRaveAngles) {
	if (realAngles.size() != openRaveAngles.size()) {
		ErrorHandler::handleMessage(
				"realAngles vector and openRaveAngles vector is not equal. [convertRealAnglesToOpenRaveAngles in Katana]\n",
				ErrorHandler::WARNING);
		return false;
	}
	// Only joint angles
	if (realAngles.size() >= 5) {
		openRaveAngles[0] = M_PI - realAngles[0];
		openRaveAngles[1] = M_PI_2 - realAngles[1];
		openRaveAngles[2] = M_PI - realAngles[2];
		openRaveAngles[3] = M_PI - realAngles[3];
		openRaveAngles[4] = M_PI - realAngles[4];
	}
	// With gripper angles
	if (realAngles.size() == 6) {
		openRaveAngles[5] = (-1.0 * realAngles[5] + this->center) / this->factor - this->offset;
	}
	return true;
}
bool Katana::convertOpenRaveAnglesToRealAngles(const std::vector<double>& openRaveAngles, std::vector<double>& realAngles) {
	if (realAngles.size() != openRaveAngles.size()) {
		ErrorHandler::handleMessage(
				"realAngles vector and openRaveAngles vector is not equal. [convertOpenRaveAnglesToRealAngles in Katana]\n",
				ErrorHandler::WARNING);
		return false;
	}
	// Only joint angles
	if (openRaveAngles.size() >= 5) {
		realAngles[0] = M_PI - openRaveAngles[0];
		realAngles[1] = M_PI_2 - openRaveAngles[1];
		realAngles[2] = M_PI - openRaveAngles[2];
		realAngles[3] = M_PI - openRaveAngles[3];
		realAngles[4] = M_PI - openRaveAngles[4];
	}
	// With gripper angles
	if (openRaveAngles.size() == 6) {
		realAngles[5] = -1.0 * (openRaveAngles[5] + this->offset) * this->factor + this->center;
	}
	return true;
}

void Katana::getActivDofsWithoutGripper(std::vector<int>& dofs) {
	// Without the 3 Dof because it is a dummy Dof
	dofs.push_back(0);
	dofs.push_back(1);
	dofs.push_back(2);
	dofs.push_back(4);
	dofs.push_back(5);
}
void Katana::getActivDofsWithGripper(std::vector<int>& dofs) {
	// Without the 6 Dof because it is no Dof at all
	this->getActivDofsWithoutGripper(dofs);
	dofs.push_back(7);
}

bool Katana::removeDummyJointAngles(std::vector<double>& angles) {
	if (angles.size() > 6) {
		ErrorHandler::handleMessage("angle size is bigger than 6. [removeDummyJointAngles in Katana]\n", ErrorHandler::WARNING);
		return false;
	}
	angles[3] = angles[4];
	angles[4] = angles[5];
	angles.pop_back();
	return true;
}

void Katana::getOffsetRealManipulatortoOpenRaveManipulator(double& x, double& y, double& z) {
	// this offset is required as we measure the position of katana from the first
	// joint to floor and not from base to floor as it is estimated here. we thus
	// need to substract z later to get this distance.

	x = this->manipulatorCoord_x;
	y = this->manipulatorCoord_y;
	z = this->manipulatorCoord_z;
}

Katana::~Katana() {
}

}
