//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.3
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
//        steck@hs-ulm.de
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

#include "PersonDetectionTask.hh"
#include "gen/SmartVisualization.hh"

#include <CommBasicObjects/commVoid.hh>
#include <CommPersonDetectionObjects/commPersonDetectionEventResult.hh>
#include <CommPersonDetectionObjects/commDetectedPerson.hh>

#include <iostream>

PersonDetectionTask::PersonDetectionTask() :
	personDetection(0)
{
}

void PersonDetectionTask::init()
{
	personDetection = new PersonDetectionVisualization(COMP->getWindow3d(), "PersonDetection");
}

int PersonDetectionTask::svc()
{
	CHS::StatusCode status;

	std::cout << "connecting to: " << COMP->ini.personDetectionQueryClient.serverName << "; "
			<< COMP->ini.personDetectionQueryClient.serviceName << std::endl;
	status = COMP->personDetectionQueryClient->connect(COMP->ini.personDetectionQueryClient.serverName,
			COMP->ini.personDetectionQueryClient.serviceName);
	while (status != CHS::SMART_OK)
	{
		usleep(500000);
		status = COMP->personDetectionQueryClient->connect(COMP->ini.personDetectionQueryClient.serverName,
				COMP->ini.personDetectionQueryClient.serviceName);
	}
	std::cout << "connected.\n";

	std::cout << "connecting to: " << COMP->ini.personDetectionEventClient.serverName << "; "
			<< COMP->ini.personDetectionEventClient.serviceName << std::endl;
	 status = COMP->personDetectionEventClient->connect(COMP->ini.personDetectionEventClient.serverName,
			COMP->ini.personDetectionEventClient.serviceName);
	while (status != CHS::SMART_OK)
	{
		usleep(500000);
		status = COMP->personDetectionEventClient->connect(COMP->ini.personDetectionEventClient.serverName,
				COMP->ini.personDetectionEventClient.serviceName);
	}
	std::cout << "connected.\n";

	// Register to Event
	CHS::EventId id;
	CommBasicObjects::CommVoid eventParameter;
	CommPersonDetectionObjects::CommPersonDetectionEventResult eventResult;

	COMP->personDetectionEventClient->activate(CHS::continuous, eventParameter, id);
	std::cout << "Registered to PD Server Event!" << std::endl;

	if (personDetection != 0)
	{
		while (1)
		{
			COMP->personDetectionEventClient->getNextEvent(id, eventResult);

			std::vector<CommPersonDetectionObjects::CommDetectedPerson> persons;
			CommPersonDetectionObjects::CommPersonId queryRequest;
			CommPersonDetectionObjects::CommDetectedPerson queryResult;

			std::cout << "[PersonDetection] env_id: " << eventResult.get_environment_id() << "\n";

			for (size_t i =0; i < eventResult.get_person_id_size(); ++i){
				queryRequest.set_person_id(eventResult.get_person_id(i));
				COMP->personDetectionQueryClient->query(queryRequest, queryResult);
				if (queryResult.is_valid()){
					persons.push_back(queryResult);
				}
			}

			personDetection->clear();
			personDetection->displayPersons(persons);
		}

		delete personDetection;
	} else
	{
		std::cerr << "No person detection visualization available\n";
	}

	return 0;
}