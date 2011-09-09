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

#include "CompHandler.hh"
#include "gen/SmartKatanaTestClient.hh"

#include <iostream>

// include communication objects
#include <CommVisionObjects/comm3dPointCloud.hh>

#include <CommManipulatorObjects/commManipulatorEventParameter.hh>
#include <CommManipulatorObjects/commManipulatorEventResult.hh>
#include <CommManipulatorObjects/commManipulatorId.hh>
#include <CommManipulatorObjects/commManipulatorParameter.hh>
#include <CommManipulatorObjects/commManipulatorRequestScan3d.hh>
#include <CommManipulatorObjects/commManipulatorTrajectory.hh>
#include <CommManipulatorObjects/commMobileManipulatorState.hh>

#include <CommManipulationPlannerObjects/commManipulationPlannerEventParameter.hh>
#include <CommManipulationPlannerObjects/commManipulationPlannerEventResult.hh>
#include <CommManipulationPlannerObjects/commManipulationPlannerParameter.hh>

#include <CommObjectRecognitionObjects/commObjectRecognitionEventResult.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionId.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionObjectProperties.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionParameter.hh>

void CompHandler::onStartup() {
	std::cout << "startup - put your startupCode in CompHandler::onStartup()!!!\n";

	CHS::StatusCode status;

	// connect to all services

	/**
	 * Connect to Laser
	 */
	if (COMP->ini.connect_services.laser_connected) {
		std::cout << "connecting to: " << COMP->ini.laserPushTimedClient.serverName << "; "
				<< COMP->ini.laserPushTimedClient.serviceName << std::endl;
		status = COMP->laserPushTimedClient->connect(COMP->ini.laserPushTimedClient.serverName,
				COMP->ini.laserPushTimedClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->laserPushTimedClient->connect(COMP->ini.laserPushTimedClient.serverName,
					COMP->ini.laserPushTimedClient.serviceName);
		}
		std::cout << "connected.\n";

		COMP->laserPushTimedClient->subscribe(COMP->ini.laserPushTimedClient.interval);

		std::cout << "connecting to: " << COMP->ini.laserStateClient.serverName << "; "
				<< COMP->ini.laserStateClient.serviceName << std::endl;
		status = COMP->laserStateClient->connect(COMP->ini.laserStateClient.serverName,
				COMP->ini.laserStateClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->laserStateClient->connect(COMP->ini.laserStateClient.serverName,
					COMP->ini.laserStateClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.laserQueryClient.serverName << "; "
				<< COMP->ini.laserQueryClient.serviceName << std::endl;
		status = COMP->laserQueryClient->connect(COMP->ini.laserQueryClient.serverName,
				COMP->ini.laserQueryClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->laserQueryClient->connect(COMP->ini.laserQueryClient.serverName,
					COMP->ini.laserQueryClient.serviceName);
		}
		std::cout << "connected.\n";
	}

	/**
	 * Connect to Object Recognition
	 */
	if (COMP->ini.connect_services.object_recognition_connected) {
		std::cout << "connecting to: " << COMP->ini.recognitionStateClient.serverName << "; "
				<< COMP->ini.recognitionStateClient.serviceName << std::endl;
		status = COMP->recognitionStateClient->connect(COMP->ini.recognitionStateClient.serverName,
				COMP->ini.recognitionStateClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->recognitionStateClient->connect(COMP->ini.recognitionStateClient.serverName,
					COMP->ini.recognitionStateClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.recognitionEventClient.serverName << "; "
				<< COMP->ini.recognitionEventClient.serviceName << std::endl;
		status = COMP->recognitionEventClient->connect(COMP->ini.recognitionEventClient.serverName,
				COMP->ini.recognitionEventClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->recognitionEventClient->connect(COMP->ini.recognitionEventClient.serverName,
					COMP->ini.recognitionEventClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.recognitionPropertyQueryClient.serverName << "; "
				<< COMP->ini.recognitionPropertyQueryClient.serviceName << std::endl;
		status = COMP->recognitionPropertyQueryClient->connect(COMP->ini.recognitionPropertyQueryClient.serverName,
				COMP->ini.recognitionPropertyQueryClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->recognitionPropertyQueryClient->connect(COMP->ini.recognitionPropertyQueryClient.serverName,
					COMP->ini.recognitionPropertyQueryClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.recognitionParameterClient.serverName << "; "
				<< COMP->ini.recognitionParameterClient.serviceName << std::endl;
		status = COMP->recognitionParameterClient->connect(COMP->ini.recognitionParameterClient.serverName,
				COMP->ini.recognitionParameterClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->recognitionParameterClient->connect(COMP->ini.recognitionParameterClient.serverName,
					COMP->ini.recognitionParameterClient.serviceName);
		}
		std::cout << "connected.\n";
	}

	/**
	 * Connect to real Manipulator
	 */
	if (COMP->ini.connect_services.manipulator_connected) {
		std::cout << "connecting to: " << COMP->ini.manipulatorRequestScan3dSendClient.serverName << "; "
				<< COMP->ini.manipulatorRequestScan3dSendClient.serviceName << std::endl;
		status = COMP->manipulatorRequestScan3dSendClient->connect(
				COMP->ini.manipulatorRequestScan3dSendClient.serverName,
				COMP->ini.manipulatorRequestScan3dSendClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorRequestScan3dSendClient->connect(
					COMP->ini.manipulatorRequestScan3dSendClient.serverName,
					COMP->ini.manipulatorRequestScan3dSendClient.serviceName);
		}
		std::cout << "connected.\n";


		std::cout << "connecting to: " << COMP->ini.manipulatorScan3dQueryClient.serverName << "; "
				<< COMP->ini.manipulatorScan3dQueryClient.serviceName << std::endl;
		status = COMP->manipulatorScan3dQueryClient->connect(COMP->ini.manipulatorScan3dQueryClient.serverName,
				COMP->ini.manipulatorScan3dQueryClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorScan3dQueryClient->connect(COMP->ini.manipulatorScan3dQueryClient.serverName,
					COMP->ini.manipulatorScan3dQueryClient.serviceName);
		}
		std::cout << "connected.\n";


		std::cout << "connecting to: " << COMP->ini.manipulatorScanEventClient.serverName << "; "
				<< COMP->ini.manipulatorScanEventClient.serviceName << std::endl;
		status = COMP->manipulatorScanEventClient->connect(COMP->ini.manipulatorScanEventClient.serverName,
				COMP->ini.manipulatorScanEventClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorScanEventClient->connect(COMP->ini.manipulatorScanEventClient.serverName,
					COMP->ini.manipulatorScanEventClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.manipulatorStateClient.serverName << "; "
				<< COMP->ini.manipulatorStateClient.serviceName << std::endl;
		status = COMP->manipulatorStateClient->connect(COMP->ini.manipulatorStateClient.serverName,
				COMP->ini.manipulatorStateClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorStateClient->connect(COMP->ini.manipulatorStateClient.serverName,
					COMP->ini.manipulatorStateClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.manipulatorParamClient.serverName << "; "
				<< COMP->ini.manipulatorParamClient.serviceName << std::endl;
		status = COMP->manipulatorParamClient->connect(COMP->ini.manipulatorParamClient.serverName,
				COMP->ini.manipulatorParamClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorParamClient->connect(COMP->ini.manipulatorParamClient.serverName,
					COMP->ini.manipulatorParamClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.manipulatorTrajectorySendClient.serverName << "; "
				<< COMP->ini.manipulatorTrajectorySendClient.serviceName << std::endl;
		status = COMP->manipulatorTrajectorySendClient->connect(COMP->ini.manipulatorTrajectorySendClient.serverName,
				COMP->ini.manipulatorTrajectorySendClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorTrajectorySendClient->connect(
					COMP->ini.manipulatorTrajectorySendClient.serverName,
					COMP->ini.manipulatorTrajectorySendClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.manipulatorPosePushTimedClient.serverName << "; "
				<< COMP->ini.manipulatorPosePushTimedClient.serviceName << std::endl;
		status = COMP->manipulatorPosePushTimedClient->connect(COMP->ini.manipulatorPosePushTimedClient.serverName,
				COMP->ini.manipulatorPosePushTimedClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorPosePushTimedClient->connect(COMP->ini.manipulatorPosePushTimedClient.serverName,
					COMP->ini.manipulatorPosePushTimedClient.serviceName);
		}
		std::cout << "connected.\n";

		COMP->manipulatorPosePushTimedClient->subscribe(COMP->ini.manipulatorPosePushTimedClient.interval);

		std::cout << "connecting to: " << COMP->ini.manipulatorScan3dQueryClient.serverName << "; "
				<< COMP->ini.manipulatorScan3dQueryClient.serviceName << std::endl;
		status = COMP->manipulatorScan3dQueryClient->connect(COMP->ini.manipulatorScan3dQueryClient.serverName,
				COMP->ini.manipulatorScan3dQueryClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorScan3dQueryClient->connect(COMP->ini.manipulatorScan3dQueryClient.serverName,
					COMP->ini.manipulatorScan3dQueryClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.manipulatorEventClient.serverName << "; "
				<< COMP->ini.manipulatorEventClient.serviceName << std::endl;
		status = COMP->manipulatorEventClient->connect(COMP->ini.manipulatorEventClient.serverName,
				COMP->ini.manipulatorEventClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->manipulatorEventClient->connect(COMP->ini.manipulatorEventClient.serverName,
					COMP->ini.manipulatorEventClient.serviceName);
		}
		std::cout << "connected.\n";
	}

	/**
	 * Connect to OpenRave
	 */
	if (COMP->ini.connect_services.openrave_connected) {
		std::cout << "connecting to: " << COMP->ini.openRaveEventClient.serverName << "; "
				<< COMP->ini.openRaveEventClient.serviceName << std::endl;
		status = COMP->openRaveEventClient->connect(COMP->ini.openRaveEventClient.serverName,
				COMP->ini.openRaveEventClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->openRaveEventClient->connect(COMP->ini.openRaveEventClient.serverName,
					COMP->ini.openRaveEventClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.openRaveParameterClient.serverName << "; "
				<< COMP->ini.openRaveParameterClient.serviceName << std::endl;
		status = COMP->openRaveParameterClient->connect(COMP->ini.openRaveParameterClient.serverName,
				COMP->ini.openRaveParameterClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->openRaveParameterClient->connect(COMP->ini.openRaveParameterClient.serverName,
					COMP->ini.openRaveParameterClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.openRaveStateClient.serverName << "; "
				<< COMP->ini.openRaveStateClient.serviceName << std::endl;
		status = COMP->openRaveStateClient->connect(COMP->ini.openRaveStateClient.serverName,
				COMP->ini.openRaveStateClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->openRaveStateClient->connect(COMP->ini.openRaveStateClient.serverName,
					COMP->ini.openRaveStateClient.serviceName);
		}
		std::cout << "connected.\n";

		std::cout << "connecting to: " << COMP->ini.openRaveTrajectorySendClient.serverName << "; "
				<< COMP->ini.openRaveTrajectorySendClient.serviceName << std::endl;
		status = COMP->openRaveTrajectorySendClient->connect(COMP->ini.openRaveTrajectorySendClient.serverName,
				COMP->ini.openRaveTrajectorySendClient.serviceName);
		while (status != CHS::SMART_OK) {
			usleep(500000);
			status = COMP->openRaveTrajectorySendClient->connect(COMP->ini.openRaveTrajectorySendClient.serverName,
					COMP->ini.openRaveTrajectorySendClient.serviceName);
		}
		std::cout << "connected.\n";

	}


	// run all tasks
	COMP->testTask.open();
	//	COMP->eventTask.open();

	if (COMP->ini.connect_services.manipulator_connected) {
		//	COMP->katanaGoalTask.open();
	}

	if (COMP->ini.connect_services.show_opengl_window) {
		COMP->viewerTask.open();
	}

}
