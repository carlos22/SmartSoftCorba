//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.8.0
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
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
//
//  Copyright (C) 2009 Andreas Steck, Dennis Stampfer
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//        stampfer@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This file is part of the SmartSoft MDSD Toolchain demonstration workspace.
//  It is based on the collection of smartExampleComponents included in the
//  SmartSoft distribution.
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
//--------------------------------------------------------------------------

#include "TimeTask.hh"
#include "gen/SamplePushNewestClient.hh"

#include <iostream>

TimeTask::TimeTask()
{
	std::cout << "constructor TimeTask\n";
}

int TimeTask::svc()
{
	CommSampleObjects::CommSampleTime a;
	CHS::StatusCode status;
	int h, m, s;

	COMP->timeClient->subscribe();

	while(1)
	{
		status = COMP->timeClient->getUpdateWait(a);
		if (status == CHS::SMART_OK)
		{
			a.get(h, m, s);
			std::cout << "hour: " << h << " minute: " << m << " second: " << s << std::endl;
		}
	} // while
	return 0;
}
