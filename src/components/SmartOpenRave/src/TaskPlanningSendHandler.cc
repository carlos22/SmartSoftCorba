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
//------------------------------------------------------------------------

#include "TaskPlanningSendHandler.hh"
#include "gen/SmartOpenRave.hh"

#include <iostream>
#include "OpenRave.hh"
#include "ManipulatorFactory.hh"

void TaskPlanningSendHandler::handleSend(const Smart::CommManipulationTaskPlanning &r) throw() {

	// ------------------------------------------------
	// check if activated. If not, do nothing with the
	// trajectory otherwise perform it.
	// ------------------------------------------------
	if (COMP->stateServer->tryAcquire("trajectory") == CHS::SMART_OK) {
		// release trajectory state so a state change can happen
		COMP->stateServer->release("trajectory");

		CHS::StatusCode status;
		CHS::EventId eventId;
		Smart::CommManipulatorEventParameter katanaEventParameter;

		SpecificManipulator::Manipulator* specificManipulator = NULL;
		// Create specific Manipulator
		try {
			specificManipulator = MANIPULATORFACTORY->createManipulatorClass(COMP->ini.OpenRave.robotName);
		} catch (std::exception& e) {
			return;
		}

		if (COMP->ini.PortParameter.withManipulator) {
			// Set event to "Goal_not_reached" of the Manipulator so that every other event is recognized
			katanaEventParameter.set_event(Smart::GOAL_NOT_REACHED);
			status = COMP->manipulatorEventClient->activate(CHS::continuous, katanaEventParameter, eventId);
			if (status != CHS::SMART_OK) {
				ErrorHandler::handleMessage(
						"Could not activate EventClient of Manipulator. [handleSend in PathPlanningSendHandler]",
						ErrorHandler::ERROR);
				return;
			}
		} else {
			COMP->ManipulatorModeMutex.acquire();
			COMP->manipulatorMode = COMP->SEND_TRAJECTORY;
			COMP->ManipulatorModeMutex.release();
		}

		try {
			// Sync OpenRave with the Real Manipulator
			OPENRAVE->syncManipulator();
		} catch (openrave_exception& e) {
			ErrorHandler::handleMessage(e.message(), ErrorHandler::ERROR);
			OPENRAVE->unlockEnvironment();
			return;
		}

		// Set component into PLANNING_PATH mode
		ErrorHandler::handleMessage("Planning Path. [handleSend in PathPlanningSendHandler]", Smart::PLANNING_PATH,
				ErrorHandler::INFO, COMP->ini.OpenRave.debugSend);

		/**
		 * TEST !!!
		 */
		try {
			std::cout << "Starting GraspPlanning ....\n";
			OPENRAVE->testPython();
			std::cout << "GraspPlanning successful.\n";
			return;
		} catch (openrave_exception& e) {
			ErrorHandler::handleMessage(e.message(), ErrorHandler::ERROR);
			OPENRAVE->unlockEnvironment();
			return;
		} catch (...) {
			std::cout << "Catched something in Pathplanningsendhandler.\n";
			return;
		}

		/**
		 * TEST END !!
		 */

	} else {
		ErrorHandler::handleMessage("Component not in \"trajectory\" state.\n", ErrorHandler::WARNING);
	}
}