//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.4
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
#include <CommManipulationPlannerObjects/commManipulationPlannerEventParameter.hh>
#include <CommManipulationPlannerObjects/commManipulationPlannerEventResult.hh>
#include <CommManipulationPlannerObjects/commManipulationPlannerEventState.hh>
#include <CommManipulationPlannerObjects/commManipulationPlannerParameter.hh>
#include <CommManipulationPlannerObjects/commManipulationTaskPlanning.hh>
#include <CommManipulatorObjects/commManipulatorEventParameter.hh>
#include <CommManipulatorObjects/commManipulatorEventResult.hh>
#include <CommManipulatorObjects/commManipulatorTrajectory.hh>
#include <CommManipulatorObjects/commMobileManipulatorState.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionEnvironment.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionId.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionObjectProperties.hh>
// include tasks	

#include "../DemonstrationTask.hh"
#include "../GUITask.hh"
#include "../ParameterTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../EventServerHandler.hh"
#include "../ManipulatorEventClientHandler.hh"
#include "../ObjectQueryHandler.hh"
#include "../ParameterServerHandler.hh"
#include "../PathPlanningSendHandler.hh"
#include "../StateChangeHandler.hh"
#include "../TaskPlanningSendHandler.hh"

#define COMP SmartOpenRave::instance()

class SmartOpenRave: public SmartOpenRaveCore
{
private:
	static SmartOpenRave _smartOpenRave;

	// constructor
	SmartOpenRave();

	// copy-constructor
	SmartOpenRave(const SmartOpenRave& cc);

	// destructor
	~SmartOpenRave()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	EventServerHandler eventServerHandler;
	ManipulatorEventClientHandler manipulatorEventClientHandler;
	ObjectQueryHandler objectQueryHandler;
	ParameterServerHandler parameterServerHandler;
	PathPlanningSendHandler pathPlanningSendHandler;
	StateChangeHandler stateChangeHandler;
	TaskPlanningSendHandler taskPlanningSendHandler;

	// ThreadQueueHandler 


	CHS::ThreadQueueSendHandler<
			CommManipulatorObjects::CommManipulatorTrajectory>
			*threadPathPlanningSendHandler;

public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex ManipulatorFactoryMutex;
	CHS::SmartMutex ManipulatorModeMutex;
	CHS::SmartMutex OpenRaveMutex;

	// create condition mutex


	// instantiate tasks

	DemonstrationTask demonstrationTask;
	GUITask gUITask;
	ParameterTask parameterTask;

	// ports
	CHS::QueryClient<CommObjectRecognitionObjects::CommObjectRecognitionId,
			CommObjectRecognitionObjects::CommObjectRecognitionEnvironment>
			*environmentQueryClient;
	CHS::EventServer<
			CommManipulationPlannerObjects::CommManipulationPlannerEventParameter,
			CommManipulationPlannerObjects::CommManipulationPlannerEventResult,
			CommManipulationPlannerObjects::CommManipulationPlannerEventState>
			*eventServer;
	CHS::EventClient<CommManipulatorObjects::CommManipulatorEventParameter,
			CommManipulatorObjects::CommManipulatorEventResult>
			*manipulatorEventClient;
	CHS::PushTimedClient<CommManipulatorObjects::CommMobileManipulatorState>
			*manipulatorStatePushTimedClient;
	CHS::QueryClient<CommObjectRecognitionObjects::CommObjectRecognitionId,
			CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties>
			*objectQueryClient;
	CHS::QueryServer<CommObjectRecognitionObjects::CommObjectRecognitionId,
			CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties>
			*objectQueryServer;
	CHS::SendServer<
			CommManipulationPlannerObjects::CommManipulationPlannerParameter>
			*parameterServer;
	CHS::SendServer<CommManipulatorObjects::CommManipulatorTrajectory>
			*pathPlanningSendServer;
	CHS::SmartStateServer *stateServer;
	CHS::SendServer<
			CommManipulationPlannerObjects::CommManipulationTaskPlanning>
			*taskPlanningSendServer;
	CHS::SendClient<CommManipulatorObjects::CommManipulatorTrajectory>
			*trajectorySendClient;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartOpenRave* instance()
	{
		return (SmartOpenRave*) &_smartOpenRave;
	}

	// ini parameter
	struct ini_ini
	{

		// component struct
		struct ini_component
		{
			// the name of the component
			std::string name;
		} component;

		struct ini_eventServer
		{
			std::string serviceName;
		} eventServer;

		struct ini_objectQueryServer
		{
			std::string serviceName;
		} objectQueryServer;

		struct ini_parameterServer
		{
			std::string serviceName;
		} parameterServer;

		struct ini_pathPlanningSendServer
		{
			std::string serviceName;
		} pathPlanningSendServer;

		struct ini_stateServer
		{
			std::string serviceName;
		} stateServer;

		struct ini_taskPlanningSendServer
		{
			std::string serviceName;
		} taskPlanningSendServer;

		struct ini_environmentQueryClient
		{
			std::string serverName;
			std::string serviceName;
		} environmentQueryClient;

		struct ini_manipulatorEventClient
		{
			std::string serverName;
			std::string serviceName;
		} manipulatorEventClient;

		struct ini_manipulatorStatePushTimedClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} manipulatorStatePushTimedClient;

		struct ini_objectQueryClient
		{
			std::string serverName;
			std::string serviceName;
		} objectQueryClient;

		struct ini_trajectorySendClient
		{
			std::string serverName;
			std::string serviceName;
		} trajectorySendClient;

		struct ini_OpenRave
		{

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

		struct ini_PortParameter
		{

			bool withManipulator;

			bool withObjectRecognition;
		} PortParameter;
	} ini;

};
#endif
