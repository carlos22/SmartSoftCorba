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

#include "SmartOpenRave.hh"

// constructor
SmartOpenRave::SmartOpenRave()
{
	std::cout << "constructor of SmartOpenRave\n";

	ini.component.name = "SmartOpenRave";

	ini.eventServer.serviceName = "eventServer";
	ini.objectQueryServer.serviceName = "objectFeatures";
	ini.parameterServer.serviceName = "param";
	ini.pathPlanningSendServer.serviceName = "pathPlanning";
	ini.stateServer.serviceName = "state";
	ini.taskPlanningSendServer.serviceName = "taskPlanning";
	ini.environmentQueryClient.serverName = "SmartObjectRecognition";
	ini.environmentQueryClient.serviceName = "environment";
	ini.manipulatorEventClient.serverName = "SmartKatanaServer";
	ini.manipulatorEventClient.serviceName = "eventServer";
	ini.manipulatorStatePushTimedClient.serverName = "SmartKatanaServer";
	ini.manipulatorStatePushTimedClient.serviceName = "currentPose";
	ini.manipulatorStatePushTimedClient.interval = 1;
	ini.objectQueryClient.serverName = "SmartObjectRecognition";
	ini.objectQueryClient.serviceName = "objectFeatures";
	ini.trajectorySendClient.serverName = "SmartKatanaServer";
	ini.trajectorySendClient.serviceName = "performTrajectory";
	ini.OpenRave.debugOpenRave = true;
	ini.OpenRave.debugSend = false;
	ini.OpenRave.defaultEnvironmentPath = "data/defaultOpenRave.env.xml";
	ini.OpenRave.gui_active = true;
	ini.OpenRave.robotName = "Katana";
	ini.OpenRave.robotPath = "robots/katana.robot.xml";
	ini.OpenRave.saveObjectsToFile = false;
	ini.OpenRave.saveTrajectoryToFile = false;
	ini.OpenRave.showCompleteTrajectory = false;
	ini.OpenRave.storedEnvironmentName = "storedEnvironment";
	ini.OpenRave.storedEnvironmentPath = "data/";
	ini.PortParameter.withManipulator = true;
	ini.PortParameter.withObjectRecognition = true;
}

void SmartOpenRave::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartOpenRave is named " << ini.component.name
				<< "." << std::endl;

		// create ports
		environmentQueryClient
				= new CHS::QueryClient<
						CommObjectRecognitionObjects::CommObjectRecognitionId,
						CommObjectRecognitionObjects::CommObjectRecognitionEnvironment>(
						component);
		eventServer
				= new CHS::EventServer<
						CommManipulationPlannerObjects::CommManipulationPlannerEventParameter,
						CommManipulationPlannerObjects::CommManipulationPlannerEventResult,
						CommManipulationPlannerObjects::CommManipulationPlannerEventState>(
						component, ini.eventServer.serviceName,
						eventServerHandler);

		manipulatorEventClient = new CHS::EventClient<
				CommManipulatorObjects::CommManipulatorEventParameter,
				CommManipulatorObjects::CommManipulatorEventResult>(component,
				manipulatorEventClientHandler);

		manipulatorStatePushTimedClient = new CHS::PushTimedClient<
				CommManipulatorObjects::CommMobileManipulatorState>(component);
		objectQueryClient
				= new CHS::QueryClient<
						CommObjectRecognitionObjects::CommObjectRecognitionId,
						CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties>(
						component);
		objectQueryServer
				= new CHS::QueryServer<
						CommObjectRecognitionObjects::CommObjectRecognitionId,
						CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties>(
						component, ini.objectQueryServer.serviceName,
						objectQueryHandler);

		parameterServer
				= new CHS::SendServer<
						CommManipulationPlannerObjects::CommManipulationPlannerParameter>(
						component, ini.parameterServer.serviceName,
						parameterServerHandler);

		threadPathPlanningSendHandler = new CHS::ThreadQueueSendHandler<
				CommManipulatorObjects::CommManipulatorTrajectory>(
				pathPlanningSendHandler);
		pathPlanningSendServer = new CHS::SendServer<
				CommManipulatorObjects::CommManipulatorTrajectory>(component,
				ini.pathPlanningSendServer.serviceName,
				*threadPathPlanningSendHandler);

		stateServer = new CHS::SmartStateServer(component,
				ini.stateServer.serviceName, stateChangeHandler);
		// define states for stateServer (CHS::SmartStateServer) 
		if (stateServer->defineStates("demonstration", "demonstration")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		if (stateServer->defineStates("simulation", "simulation")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		if (stateServer->defineStates("trajectory", "trajectory")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;

		taskPlanningSendServer = new CHS::SendServer<
				CommManipulationPlannerObjects::CommManipulationTaskPlanning>(
				component, ini.taskPlanningSendServer.serviceName,
				taskPlanningSendHandler);

		trajectorySendClient = new CHS::SendClient<
				CommManipulatorObjects::CommManipulatorTrajectory>(component);
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		environmentQueryClient->add(wiringSlave, "environmentQueryClient");
		manipulatorEventClient->add(wiringSlave, "manipulatorEventClient");
		manipulatorStatePushTimedClient->add(wiringSlave,
				"manipulatorStatePushTimedClient");
		objectQueryClient->add(wiringSlave, "objectQueryClient");
		trajectorySendClient->add(wiringSlave, "trajectorySendClient");

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartOpenRave::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartOpenRave::loadParameter(int argc, char *argv[])
{
	/* 
	 Parameters can be specified via command line -filename=<filename>
	 
	 With this parameter present:
	 - The component will look for the file in the current working directory, 
	 a path relative to the current directory or any absolute path
	 - The component will use the default values if the file cannot be found
	 
	 With this parameter absent:
	 - <Name of Component>.ini will be read from current working directory, if found there
	 - $SMART_ROOT/etc/<Name of Component>.ini will be read otherwise
	 - Default values will be used if neither found in working directory or /etc   
	 */
	CHS::SmartParameter parameter;

	// load parameters
	try
	{
		// check if paramfile is given as argument
		bool paramFile = false;
		std::string str;
		for (int i = 0; i < argc; i++)
		{
			str = argv[i];
			if (str.find("filename") != std::string::npos)
				paramFile = true;
		}

		// if paramfile is given as argument
		if (paramFile == true)
		{
			std::cout << "load parameter file from argv \n";
			parameter.addFile(argc, argv, "filename", false);
		}
		// else load standard paramfile
		else
		{
			std::cout << "load SmartOpenRave.ini parameter file\n";
			parameter.addFile("SmartOpenRave.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("eventServer", "serviceName",
				ini.eventServer.serviceName);
		parameter.getString("objectQueryServer", "serviceName",
				ini.objectQueryServer.serviceName);
		parameter.getString("parameterServer", "serviceName",
				ini.parameterServer.serviceName);
		parameter.getString("pathPlanningSendServer", "serviceName",
				ini.pathPlanningSendServer.serviceName);
		parameter.getString("stateServer", "serviceName",
				ini.stateServer.serviceName);
		parameter.getString("taskPlanningSendServer", "serviceName",
				ini.taskPlanningSendServer.serviceName);
		parameter.getString("environmentQueryClient", "serverName",
				ini.environmentQueryClient.serverName);
		parameter.getString("environmentQueryClient", "serviceName",
				ini.environmentQueryClient.serviceName);
		parameter.getString("manipulatorEventClient", "serverName",
				ini.manipulatorEventClient.serverName);
		parameter.getString("manipulatorEventClient", "serviceName",
				ini.manipulatorEventClient.serviceName);
		parameter.getString("manipulatorStatePushTimedClient", "serverName",
				ini.manipulatorStatePushTimedClient.serverName);
		parameter.getString("manipulatorStatePushTimedClient", "serviceName",
				ini.manipulatorStatePushTimedClient.serviceName);
		parameter.getInt("manipulatorStatePushTimedClient", "interval",
				ini.manipulatorStatePushTimedClient.interval);
		parameter.getString("objectQueryClient", "serverName",
				ini.objectQueryClient.serverName);
		parameter.getString("objectQueryClient", "serviceName",
				ini.objectQueryClient.serviceName);
		parameter.getString("trajectorySendClient", "serverName",
				ini.trajectorySendClient.serverName);
		parameter.getString("trajectorySendClient", "serviceName",
				ini.trajectorySendClient.serviceName);
		parameter.getTruthValue("OpenRave", "debugOpenRave",
				ini.OpenRave.debugOpenRave);
		parameter.getTruthValue("OpenRave", "debugSend", ini.OpenRave.debugSend);
		parameter.getString("OpenRave", "defaultEnvironmentPath",
				ini.OpenRave.defaultEnvironmentPath);
		parameter.getTruthValue("OpenRave", "gui_active",
				ini.OpenRave.gui_active);
		parameter.getString("OpenRave", "robotName", ini.OpenRave.robotName);
		parameter.getString("OpenRave", "robotPath", ini.OpenRave.robotPath);
		parameter.getTruthValue("OpenRave", "saveObjectsToFile",
				ini.OpenRave.saveObjectsToFile);
		parameter.getTruthValue("OpenRave", "saveTrajectoryToFile",
				ini.OpenRave.saveTrajectoryToFile);
		parameter.getTruthValue("OpenRave", "showCompleteTrajectory",
				ini.OpenRave.showCompleteTrajectory);
		parameter.getString("OpenRave", "storedEnvironmentName",
				ini.OpenRave.storedEnvironmentName);
		parameter.getString("OpenRave", "storedEnvironmentPath",
				ini.OpenRave.storedEnvironmentPath);
		parameter.getTruthValue("PortParameter", "withManipulator",
				ini.PortParameter.withManipulator);
		parameter.getTruthValue("PortParameter", "withObjectRecognition",
				ini.PortParameter.withObjectRecognition);

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (const CHS::ParameterError & e)
	{
		std::cerr << "Exception from parameter handling: " << e << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

