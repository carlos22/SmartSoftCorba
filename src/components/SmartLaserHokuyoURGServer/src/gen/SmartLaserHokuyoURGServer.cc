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

#include "SmartLaserHokuyoURGServer.hh"

// constructor
SmartLaserHokuyoURGServer::SmartLaserHokuyoURGServer()
{
	std::cout << "constructor of SmartLaserHokuyoURGServer\n";

	ini.component.name = "SmartLaserHokuyoURGServer";

	ini.laserPushNewestServer.serviceName = "scan";
	ini.laserPushTimedServer.serviceName = "scan";
	ini.laserPushTimedServer.cycle = 0.1;
	ini.laserQueryServer.serviceName = "scan";
	ini.stateServer.serviceName = "state";
	ini.baseTimedClient.serverName = "SmartPioneerBaseServer";
	ini.baseTimedClient.serviceName = "basestate";
	ini.baseTimedClient.interval = 1;
	ini.manipulatorTimedClient.serverName = "SmartKatanaServer";
	ini.manipulatorTimedClient.serviceName = "currentPose";
	ini.manipulatorTimedClient.interval = 1;
	ini.base_manipulator.base_a = 0;
	ini.base_manipulator.on_base = true;
	ini.base_manipulator.on_manipulator = false;
	ini.base_manipulator.on_turret = false;
	ini.base_manipulator.steer_a = 0;
	ini.base_manipulator.turret_a = 0;
	ini.base_manipulator.x = 0;
	ini.base_manipulator.y = 0;
	ini.base_manipulator.z = 0;
	ini.scanner.azimuth = 0;
	ini.scanner.device = "/dev/ttyACM0";
	ini.scanner.elevation = 0;
	ini.scanner.max_range = 5600;
	ini.scanner.min_range = 20;
	ini.scanner.opening_angle = 24000;
	ini.scanner.roll = 0;
	ini.scanner.speed = 115200;
	ini.scanner.type = "URG-04LX";
	ini.scanner.verbose = false;
	ini.scanner.x = 0;
	ini.scanner.y = 0;
	ini.scanner.z = 0;
	ini.services.activate_push_newest = true;
	ini.services.activate_push_timed = true;
}

void SmartLaserHokuyoURGServer::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartLaserHokuyoURGServer is named "
				<< ini.component.name << "." << std::endl;

		// create ports
		baseTimedClient = new CHS::PushTimedClient<
				CommBasicObjects::CommBaseState>(component);
		laserPushNewestServer = new CHS::PushNewestServer<
				CommBasicObjects::CommMobileLaserScan>(component,
				ini.laserPushNewestServer.serviceName);
		laserPushTimedServer = new CHS::PushTimedServer<
				CommBasicObjects::CommMobileLaserScan>(component,
				ini.laserPushTimedServer.serviceName, pushTimedHandler,
				ini.laserPushTimedServer.cycle);

		laserQueryServer = new CHS::QueryServer<CommBasicObjects::CommVoid,
				CommBasicObjects::CommMobileLaserScan>(component,
				ini.laserQueryServer.serviceName, laserQueryHandler);

		manipulatorTimedClient = new CHS::PushTimedClient<
				CommManipulatorObjects::CommMobileManipulatorState>(component);
		stateServer = new CHS::SmartStateServer(component,
				ini.stateServer.serviceName, stateChangeHandler);
		// define states for stateServer (CHS::SmartStateServer) 
		if (stateServer->defineStates("active", "active") != CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		if (stateServer->defineStates("query_only", "query_only")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		baseTimedClient->add(wiringSlave, "baseClient");
		manipulatorTimedClient->add(wiringSlave, "manipulatorClient");

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartLaserHokuyoURGServer::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartLaserHokuyoURGServer::loadParameter(int argc, char *argv[])
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
			std::cout << "load SmartLaserHokuyoURGServer.ini parameter file\n";
			parameter.addFile("SmartLaserHokuyoURGServer.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("laserPushNewestServer", "serviceName",
				ini.laserPushNewestServer.serviceName);
		parameter.getString("laserPushTimedServer", "serviceName",
				ini.laserPushTimedServer.serviceName);
		parameter.getDouble("laserPushTimedServer", "cycle",
				ini.laserPushTimedServer.cycle);
		parameter.getString("laserQueryServer", "serviceName",
				ini.laserQueryServer.serviceName);
		parameter.getString("stateServer", "serviceName",
				ini.stateServer.serviceName);
		parameter.getString("baseTimedClient", "serverName",
				ini.baseTimedClient.serverName);
		parameter.getString("baseTimedClient", "serviceName",
				ini.baseTimedClient.serviceName);
		parameter.getInt("baseTimedClient", "interval",
				ini.baseTimedClient.interval);
		parameter.getString("manipulatorTimedClient", "serverName",
				ini.manipulatorTimedClient.serverName);
		parameter.getString("manipulatorTimedClient", "serviceName",
				ini.manipulatorTimedClient.serviceName);
		parameter.getInt("manipulatorTimedClient", "interval",
				ini.manipulatorTimedClient.interval);
		parameter.getDouble("base_manipulator", "base_a",
				ini.base_manipulator.base_a);
		parameter.getTruthValue("base_manipulator", "on_base",
				ini.base_manipulator.on_base);
		parameter.getTruthValue("base_manipulator", "on_manipulator",
				ini.base_manipulator.on_manipulator);
		parameter.getTruthValue("base_manipulator", "on_turret",
				ini.base_manipulator.on_turret);
		parameter.getDouble("base_manipulator", "steer_a",
				ini.base_manipulator.steer_a);
		parameter.getDouble("base_manipulator", "turret_a",
				ini.base_manipulator.turret_a);
		parameter.getInt("base_manipulator", "x", ini.base_manipulator.x);
		parameter.getInt("base_manipulator", "y", ini.base_manipulator.y);
		parameter.getInt("base_manipulator", "z", ini.base_manipulator.z);
		parameter.getDouble("scanner", "azimuth", ini.scanner.azimuth);
		parameter.getString("scanner", "device", ini.scanner.device);
		parameter.getDouble("scanner", "elevation", ini.scanner.elevation);
		parameter.getInt("scanner", "max_range", ini.scanner.max_range);
		parameter.getInt("scanner", "min_range", ini.scanner.min_range);
		parameter.getInt("scanner", "opening_angle", ini.scanner.opening_angle);
		parameter.getDouble("scanner", "roll", ini.scanner.roll);
		parameter.getInt("scanner", "speed", ini.scanner.speed);
		parameter.getString("scanner", "type", ini.scanner.type);
		parameter.getTruthValue("scanner", "verbose", ini.scanner.verbose);
		parameter.getInt("scanner", "x", ini.scanner.x);
		parameter.getInt("scanner", "y", ini.scanner.y);
		parameter.getInt("scanner", "z", ini.scanner.z);
		parameter.getTruthValue("services", "activate_push_newest",
				ini.services.activate_push_newest);
		parameter.getTruthValue("services", "activate_push_timed",
				ini.services.activate_push_timed);

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

