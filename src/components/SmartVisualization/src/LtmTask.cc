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
//  Copyright (C) 2011 Manuel Wopfner
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
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

#include "LtmTask.hh"
#include "gen/SmartVisualization.hh"

#include <iostream>

LtmTask::LtmTask() :
	gridMap(0) {
}

void LtmTask::init() {
	gridMap = new GridMapVisualization(COMP->getWindow3d(), "GridMap", true, false);
}

int LtmTask::svc() {
	std::cout << "connecting to: " << COMP->ini.lmtQueryClient.serverName << "; " << COMP->ini.lmtQueryClient.serviceName << std::endl;
	CHS::StatusCode status = COMP->lmtQueryClient->connect(COMP->ini.lmtQueryClient.serverName, COMP->ini.lmtQueryClient.serviceName);
	while (status != CHS::SMART_OK) {
		usleep(500000);
		status = COMP->lmtQueryClient->connect(COMP->ini.lmtQueryClient.serverName, COMP->ini.lmtQueryClient.serviceName);
	}
	std::cout << COMP->ini.lmtQueryClient.serverName << "; " << COMP->ini.lmtQueryClient.serviceName << " connected.\n";

	CommNavigationObjects::CommGridMapRequest mapRequest;
	CommNavigationObjects::CommGridMap map;

	if (gridMap != 0) {
		while (1) {
			mapRequest.set_parameter(0, 0, 0, 10000, 10000, true, 30);
			CHS::StatusCode status = COMP->lmtQueryClient->query(mapRequest, map);
			if (status == CHS::SMART_OK) {
				gridMap->displayGridMap(map);
			} else {
				gridMap->clear();
			}

			usleep(30000000);
		}

		delete gridMap;
	} else {
		std::cerr << "No grid map visualization available\n";
	}
	return 0;
}