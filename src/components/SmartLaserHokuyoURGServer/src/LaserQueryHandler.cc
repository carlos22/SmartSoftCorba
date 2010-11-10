//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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
// If you want the workflow to re-generate this file, please
// delete it before running the workflow.
//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartLaserHokuyoURGServer component".
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
//--------------------------------------------------------------------------

#include "LaserQueryHandler.hh"
#include "gen/SmartLaserHokuyoURGServer.hh"

#include <iostream>

void LaserQueryHandler::handleQuery(CHS::QueryServer<Smart::CommVoid, Smart::CommMobileLaserScan> & server,
		const CHS::QueryId id, const Smart::CommVoid & request) throw () {

	if (COMP->stateServer->tryAcquire("query_only") == CHS::SMART_OK) {
		if (COMP->ini.scanner.verbose)
			std::cout << "read new laser data with query_only\n";

		Smart::CommMobileLaserScan scan;
		COMP->laserTask.accquireNewScan(scan);
		server.answer(id, scan);
		COMP->stateServer->release("query_only");
	} else {
		if (COMP->ini.scanner.verbose)
			std::cout << "read new laser data\n";

		// return scan
		CHS::SmartGuard guard(COMP->ScanLock);
		server.answer(id, COMP->global_scan);
		guard.release();
	}

}