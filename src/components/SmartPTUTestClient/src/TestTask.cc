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
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------
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
#include "TestTask.hh"
#include "gen/SmartPTUTestClient.hh"

#include <iostream>

TestTask::TestTask() {
	std::cout << "constructor TestTask\n";
}

int TestTask::svc() {

	std::string paramValue;
	CommPTUObjects::CommPTUParameter param;

	COMP->stateClient->setWaitState("active");

	// activate event
	CommPTUObjects::CommPTUGoalEventParameter parameter;
	parameter.set_state(CommPTUObjects::PTUMoveStatus::GOAL_NOT_REACHED);
	CHS::EventId id;

	COMP->goalEventClient->activate(CHS::continuous, parameter, id);

	while (1) {
		CommPTUObjects::CommPTUMoveRequest request;
		CommPTUObjects::CommPTUMoveResponse response;

		// request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		//		request.set_pan(-M_PI_2);
		//		request.set_tilt(0);
		//		COMP->moveQueryClient->query(request, response);
		//		checkResponse(response);
		//		//		sleep(1);
		//
		//		request.set_pan(M_PI_2);
		//		request.set_tilt(M_PI_4 / 2);
		//		COMP->moveQueryClient->query(request, response);
		//		checkResponse(response);
		//		//		sleep(1);


		// straight on
		std::cout << "## going straight on" << std::endl;
		request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		request.set_pan(0);
		request.set_tilt(0);
		COMP->moveSendClient->send(request);
		sleep(2);

		// circa table
		std::cout << "## going CIRCA TABLE" << std::endl;
		request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		request.set_pan(0);
		request.set_tilt(-M_PI_4);
		COMP->moveSendClient->send(request);
		sleep(10);

		// left 
		std::cout << "## going LEFT" << std::endl;
		request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		request.set_pan(M_PI_2);
		request.set_tilt(0);
		COMP->moveSendClient->send(request);
		sleep(10);

		// right
		std::cout << "## going RIGHT" << std::endl;
		request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		request.set_pan(-M_PI_2);
		request.set_tilt(0);
		COMP->moveSendClient->send(request);
		sleep(10);



		//request.set_move_mode(CommPTUObjects::PTUMoveFlag::TILT_ABSOLUTE);
		//request.set_tilt(-M_PI_4 / 2);
		//COMP->moveQueryClient->query(request, response);
		//std::cout << response.get_status() << std::endl;
		//sleep(1);

		//request.set_move_mode(CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE);
		//request.set_pan(0);
		//request.set_tilt(0);
		//COMP->moveSendClient->send(request);
		//sleep(5);

		//		paramValue = "SENSOROFFSET(0)(800)(200)(0)(0)(0)";
		//		param.set(paramValue);
		//		COMP->paramClient->send(param);
		//
		//		paramValue = "PANACC(2)";
		//		param.set(paramValue);
		//		COMP->paramClient->send(param);

		//request.set_move_mode(CommPTUObjects::PTUMoveFlag::POSITION_WORLD);
		//request.set_position(2000, 1000, 0);
		//COMP->moveQueryClient->query(request, response);
		//checkResponse(response);
		//		sleep(1);

		//		paramValue ="SENSOROFFSET(0)(-400)(-20)(0)(0)(0)";
		//		param.set(paramValue);
		//		COMP->paramClient->send(param);
		//
		//		request.set_move_mode(CommPTUObjects::PTUMoveFlag::POSITION_ROBOT);
		//		request.set_position(2000, 0, 0);
		//		COMP->moveQueryClient->query(request, response);
		//		checkResponse(response);
	}
	return 0;
}