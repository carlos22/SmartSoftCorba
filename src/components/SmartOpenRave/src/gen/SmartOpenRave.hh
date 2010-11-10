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
// Please do not modify this file. It will be re-generated
// running the code generator.
//--------------------------------------------------------------------------

#ifndef _SMARTOPENRAVE_HH
#define _SMARTOPENRAVE_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartOpenRaveCore.hh"
// include communication objects
#include <commManipulationPlannerEventParameter.hh>

#include <commManipulationPlannerEventResult.hh>

#include <commManipulationPlannerEventState.hh>

#include <commManipulationPlannerParameter.hh>

#include <commManipulationTaskPlanning.hh>

#include <commManipulatorEventParameter.hh>

#include <commManipulatorEventResult.hh>

#include <commManipulatorTrajectory.hh>

#include <commMobileManipulatorState.hh>

#include <commObjectRecognitionEnvironment.hh>

#include <commObjectRecognitionId.hh>

#include <commObjectRecognitionObjectFeatures.hh>

// include tasks

#include "../DemonstrationTask.hh"
#include "../GUITask.hh"
// include handler
#include "../CompHandler.hh"
#include "../ManipulatorEventClientHandler.hh"
#include "../PathPlanningSendHandler.hh"
#include "../EventServerHandler.hh"
#include "../TaskPlanningSendHandler.hh"
#include "../ParameterServerHandler.hh"
#include "../StateChangeHandler.hh"

#define COMP SmartOpenRave::instance()

class SmartOpenRave: public SmartOpenRaveCore {
private:
	static SmartOpenRave _smartOpenRave;

	// constructor
	SmartOpenRave();

	// copy-constructor
	SmartOpenRave(const SmartOpenRave& cc);

	// destructor
	~SmartOpenRave() {
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	ManipulatorEventClientHandler manipulatorEventClientHandler;
	PathPlanningSendHandler pathPlanningSendHandler;
	EventServerHandler eventServerHandler;
	TaskPlanningSendHandler taskPlanningSendHandler;
	ParameterServerHandler parameterServerHandler;
	StateChangeHandler stateChangeHandler;

	// ThreadQueueHandler

	CHS::ThreadQueueSendHandler<Smart::CommManipulatorTrajectory> *threadPathPlanningSendHandler;

	// Added because Handler has to be active but currently it is not possible in the ToolChain
	CHS::ThreadQueueSendHandler<Smart::CommManipulationPlannerParameter> *threadParameterServerHandler;

public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex ManipulatorModeMutex;
	CHS::SmartMutex ManipulatorFactoryMutex;
	CHS::SmartMutex OpenRaveMutex;

	// create condition mutex


	// instantiate tasks

	DemonstrationTask demonstrationTask;
	GUITask gUITask;

	// ports
	CHS::SendClient<Smart::CommManipulatorTrajectory> *trajectorySendClient;
	CHS::SendServer<Smart::CommManipulationTaskPlanning> *taskPlanningSendServer;
	CHS::SmartStateClient *manipulatorStateClient;
	CHS::PushTimedClient<Smart::CommMobileManipulatorState> *manipulatorStatePushTimedClient;
	CHS::SendServer<Smart::CommManipulatorTrajectory> *pathPlanningSendServer;
	CHS::QueryClient<Smart::CommObjectRecognitionId, Smart::CommObjectRecognitionEnvironment> *environementQueryClient;
	CHS::SendServer<Smart::CommManipulationPlannerParameter> *parameterServer;
	CHS::EventClient<Smart::CommManipulatorEventParameter, Smart::CommManipulatorEventResult> *manipulatorEventClient;
	CHS::QueryClient<Smart::CommObjectRecognitionId, Smart::CommObjectRecognitionObjectFeatures> *objectQueryClient;
	CHS::EventServer<Smart::CommManipulationPlannerEventParameter, Smart::CommManipulationPlannerEventResult,
			Smart::CommManipulationPlannerEventState> *eventServer;
	CHS::SmartStateServer *stateServer;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartOpenRave* instance() {
		return (SmartOpenRave*) &_smartOpenRave;
	}

	// ini parameter
	struct ini_ini {

		struct ini_eventServer {
			std::string serviceName;
		} eventServer;

		struct ini_parameterServer {
			std::string serviceName;
		} parameterServer;

		struct ini_pathPlanningSendServer {
			std::string serviceName;
		} pathPlanningSendServer;

		struct ini_stateServer {
			std::string serviceName;
		} stateServer;

		struct ini_taskPlanningSendServer {
			std::string serviceName;
		} taskPlanningSendServer;

		struct ini_environementQueryClient {
			std::string serverName;
			std::string serviceName;
		} environementQueryClient;

		struct ini_manipulatorEventClient {
			std::string serverName;
			std::string serviceName;
		} manipulatorEventClient;

		struct ini_manipulatorStateClient {
			std::string serverName;
			std::string serviceName;
		} manipulatorStateClient;

		struct ini_manipulatorStatePushTimedClient {
			std::string serverName;
			std::string serviceName;
			long interval;
		} manipulatorStatePushTimedClient;

		struct ini_objectQueryClient {
			std::string serverName;
			std::string serviceName;
		} objectQueryClient;

		struct ini_trajectorySendClient {
			std::string serverName;
			std::string serviceName;
		} trajectorySendClient;

		struct ini_OpenRave {
			bool debugOpenRave;
			bool debugSend;
			std::string defaultEnvironmentPath;
			bool gui_active;
			std::string robotName;
			std::string robotPath;
			bool saveObjectsToFile;
			bool saveTrajectoryToFile;
			bool showCompleteTrajectory;
			std::string storedEnvironmentName;
			std::string storedEnvironmentPath;
		} OpenRave;
		struct ini_PortParameter {
			bool withManipulator;
			bool withObjectRecognition;
		} PortParameter;
	} ini;

};
#endif