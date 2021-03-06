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
// If you want the workflow to re-generate this file, please
// delete it before running the workflow.
//--------------------------------------------------------------------------
//
//  Copyright (C) 2010 Jonas Brich
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "Unicap Video Server component".
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

#include "CompHandler.hh"
#include "gen/SmartUnicapImageServer.hh"

#include <iostream>

#include "Unicap.hh"

// include communication objects
#include <CommBasicObjects/commBaseState.hh>

#include <CommBasicObjects/commDevicePoseState.hh>

#include <CommBasicObjects/commVoid.hh>

#include <CommVisionObjects/commVideoImage.hh>

void CompHandler::onStartup()
{

	CHS::StatusCode status;

	// connect to all services
	if (COMP->ini.base.on_base) {
		std::cout << "connecting to: " << COMP->ini.basePushTimedClient.serverName
				<< "; " << COMP->ini.basePushTimedClient.serviceName << std::endl;
		status = COMP->basePushTimedClient->connect(
				COMP->ini.basePushTimedClient.serverName,
				COMP->ini.basePushTimedClient.serviceName);
		while (status != CHS::SMART_OK)
		{
			usleep(500000);
			status = COMP->basePushTimedClient->connect(
					COMP->ini.basePushTimedClient.serverName,
					COMP->ini.basePushTimedClient.serviceName);
		}
		std::cout << "connected.\n";

		COMP->basePushTimedClient->subscribe(COMP->ini.basePushTimedClient.interval);
	}

	if (COMP->ini.base.on_ptu) {
		std::cout << "connecting to: " << COMP->ini.ptuPushTimedClient.serverName
				<< "; " << COMP->ini.ptuPushTimedClient.serviceName << std::endl;
		status = COMP->ptuPushTimedClient->connect(
				COMP->ini.ptuPushTimedClient.serverName,
				COMP->ini.ptuPushTimedClient.serviceName);
		while (status != CHS::SMART_OK)
		{
			usleep(500000);
			status = COMP->ptuPushTimedClient->connect(
					COMP->ini.ptuPushTimedClient.serverName,
					COMP->ini.ptuPushTimedClient.serviceName);
		}
		std::cout << "connected.\n";

		COMP->ptuPushTimedClient->subscribe();
	}

	try{
		UNICAPINZ->init();
		std::cout << "Unicap successful init!\n";
	} catch(Unicap::UnicapException& e){
		std::cerr << "Unicap init failed!" << std::endl;
		std::cerr << "Error: " << e.what() << " [CompHandler].\n";
	} catch(...){
		std::cerr << "Unicap init failed!" << std::endl;
		std::cerr << "Error: Undefined Error. [CompHandler]\n";
	}

	// activate state server
	if (COMP->stateServer->activate() != CHS::SMART_OK)
		std::cerr << "ERROR: activate state" << std::endl;

	// run all tasks
	COMP->imageTask.open();

	// start push timed server
	//COMP->imagePushTimedServer->start();


}
