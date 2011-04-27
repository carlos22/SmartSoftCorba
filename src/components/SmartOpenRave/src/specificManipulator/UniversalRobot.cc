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
#include "UniversalRobot.hh"
#include "util/ErrorHandler.hh"
#include "gen/SmartOpenRave.hh"

namespace SpecificManipulator {

UniversalRobot::UniversalRobot() {
	//TODO:
}

bool UniversalRobot::convertRealAnglesToOpenRaveAngles(const std::vector<double>& realAngles, std::vector<double>& openRaveAngles) {
	if (realAngles.size() != openRaveAngles.size()) {
		ErrorHandler::handleMessage(
				"realAngles vector and openRaveAngles vector is not equal. [convertRealAnglesToOpenRaveAngles in UniversalRobot]\n",
				ErrorHandler::WARNING);
		return false;
	}

	return true;
}
bool UniversalRobot::convertOpenRaveAnglesToRealAngles(const std::vector<double>& openRaveAngles, std::vector<double>& realAngles) {
	if (realAngles.size() != openRaveAngles.size()) {
		ErrorHandler::handleMessage(
				"realAngles vector and openRaveAngles vector is not equal. [convertOpenRaveAnglesToRealAngles in UniversalRobot]\n",
				ErrorHandler::WARNING);
		return false;
	}

	return true;
}

void UniversalRobot::getActivDofsWithoutGripper(std::vector<int>& dofs) {
	dofs.push_back(0);
	dofs.push_back(1);
	dofs.push_back(2);
	dofs.push_back(3);
	dofs.push_back(4);
	dofs.push_back(5);
}
void UniversalRobot::getActivDofsWithGripper(std::vector<int>& dofs) {
	this->getActivDofsWithoutGripper(dofs);
}

bool UniversalRobot::removeDummyJointAngles(std::vector<double>& angles) {
	return true;
}

void UniversalRobot::getOffsetRealManipulatortoOpenRaveManipulator(double& x, double& y, double& z) {
	x = this->manipulatorCoord_x;
	y = this->manipulatorCoord_y;
	z = this->manipulatorCoord_z;
}

UniversalRobot::~UniversalRobot() {
}

}
