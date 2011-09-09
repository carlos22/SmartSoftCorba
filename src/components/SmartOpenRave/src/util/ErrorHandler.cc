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
//--------------------------------------------------------------------------

#include "ErrorHandler.hh"
#include "gen/SmartOpenRave.hh"

void ErrorHandler::handleMessage(const std::string& message, ErrorHandler::ErrorTypes type, bool sendMessage) {
	if (sendMessage) {
		if (type == ErrorHandler::INFO) {
			std::cout << "INFO: " << message << "\n";
		} else if (type == ErrorHandler::WARNING) {
			std::cout << "WARNING: " << message << "\n";
		} else if (type == ErrorHandler::ERROR) {
			std::cerr << "ERROR: " << message << "\n";
		}
	}
}

void ErrorHandler::handleMessage(const std::string& message, CommManipulationPlannerObjects::ManipulationPlannerEvent event,
		ErrorHandler::ErrorTypes type, bool sendMessage) {
	ErrorHandler::handleMessage(message, type, sendMessage);
	// send event to other components
	CommManipulationPlannerObjects::CommManipulationPlannerEventState state;
	state.set_event(event);
	COMP->eventServer->put(state);
}

void ErrorHandler::handleMessage(const std::string& message, CommManipulationPlannerObjects::ManipulationPlannerEvent event, CommBasicObjects::CommPose3d& pose,
			ErrorHandler::ErrorTypes type,   bool sendMessage){
	ErrorHandler::handleMessage(message, type, sendMessage);
	// send event to other components
	CommManipulationPlannerObjects::CommManipulationPlannerEventState state;
	state.set_event(event);
	state.set_pose(pose);
	std::cout<<" INFO: "<< "Pose past Iteration: "<<pose<<std::endl;
	COMP->eventServer->put(state);
}
