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

#include "SmartPMDServer.hh"

// constructor
SmartPMDServer::SmartPMDServer() :
	ImageTaskCondMutex(ImageTaskMutex)
{
	std::cout << "constructor of SmartPMDServer\n";

	ini.component.name = "SmartPMDServer";

	ini.imagePushNewestServer.serviceName = "imageNewest";
	ini.imageQueryServer.serviceName = "imageQuery";
	ini.paramServer.serviceName = "param";
	ini.stateServer.serviceName = "state";
	ini.basePushTimedClient.serverName = "SmartPioneerBaseServer";
	ini.basePushTimedClient.serviceName = "basestate";
	ini.basePushTimedClient.interval = 1;
	ini.ptuDeviceStateServer.serverName = "SmartPTUServer";
	ini.ptuDeviceStateServer.serviceName = "devicePoseState";
	ini.base.base_a = 0;
	ini.base.on_base = true;
	ini.base.on_ptu = false;
	ini.base.steer_a = 0;
	ini.base.turret_a = 0;
	ini.base.x = 0;
	ini.base.y = 0;
	ini.base.z = 0;
	ini.general.debug_info = false;
	ini.general.initialize_camera = true;
	ini.general.processing_parameter = "";
	ini.general.processing_plugin
			= "src/utility/pmd-lib/plugins/camcubeproc.L32.ppp";
	ini.general.source_parameter = "";
	ini.general.source_plugin = "src/utility/pmd-lib/plugins/camcube.L32.pap";
	ini.general.valid_image_time = 1.0;
	ini.hardware_properties.activate_lense_calibration = true;
	ini.hardware_properties.activate_suppress_motion_blur = false;
	ini.hardware_properties.height = 204;
	ini.hardware_properties.integration_time = 2500;
	ini.hardware_properties.max_distance = 7500;
	ini.hardware_properties.min_distance = 300;
	ini.hardware_properties.modulation_frequency = 18;
	ini.hardware_properties.opening_angle_x_axis = 0.6981317;
	ini.hardware_properties.opening_angle_y_axis = 0.6981317;
	ini.hardware_properties.width = 204;
	ini.sensor_pose.azimuth = 0;
	ini.sensor_pose.elevation = 0;
	ini.sensor_pose.roll = 0;
	ini.sensor_pose.x = 0;
	ini.sensor_pose.y = 0;
	ini.sensor_pose.z = 0;
}

void SmartPMDServer::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartPMDServer is named " << ini.component.name
				<< "." << std::endl;

		// create ports
		basePushTimedClient = new CHS::PushTimedClient<
				CommBasicObjects::CommBaseState>(component);
		imagePushNewestServer = new CHS::PushNewestServer<
				CommVisionObjects::CommToFImage>(component,
				ini.imagePushNewestServer.serviceName);
		imageQueryServer = new CHS::QueryServer<CommBasicObjects::CommVoid,
				CommVisionObjects::CommToFImage>(component,
				ini.imageQueryServer.serviceName, imageQueryHandler);

		paramServer = new CHS::SendServer<CommVisionObjects::CommToFParameter>(
				component, ini.paramServer.serviceName, parameterHandler);

		ptuDeviceStateServer = new CHS::PushNewestClient<
				CommBasicObjects::CommDevicePoseState>(component);
		stateServer = new CHS::SmartStateServer(component,
				ini.stateServer.serviceName, stateChangeHandler);
		// define states for stateServer (CHS::SmartStateServer) 
		if (stateServer->defineStates("pushimage", "queryonly")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		if (stateServer->defineStates("pushimage", "pushimage")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		if (stateServer->defineStates("queryonly", "queryonly")
				!= CHS::SMART_OK)
			std::cerr << "ERROR: define state" << std::endl;
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		basePushTimedClient->add(wiringSlave, "basePushTimedClient");
		ptuDeviceStateServer->add(wiringSlave, "ptuDeviceStateServer");

		// create timer
		double fractpart, intpart;
		// setup imageTaskTimer
		fractpart = modf(0.04, &intpart);
		component->getTimerThread().scheduleTimer(imageTaskTimer, (void*) 0,
				ACE_Time_Value(0, 0), ACE_Time_Value(intpart, (int) (fractpart
						* 1000* 1000 )));

	}
	catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	}
	catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartPMDServer::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartPMDServer::loadParameter(int argc, char *argv[])
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
			std::cout << "load SmartPMDServer.ini parameter file\n";
			parameter.addFile("SmartPMDServer.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("imagePushNewestServer", "serviceName",
				ini.imagePushNewestServer.serviceName);
		parameter.getString("imageQueryServer", "serviceName",
				ini.imageQueryServer.serviceName);
		parameter.getString("paramServer", "serviceName",
				ini.paramServer.serviceName);
		parameter.getString("stateServer", "serviceName",
				ini.stateServer.serviceName);
		parameter.getString("basePushTimedClient", "serverName",
				ini.basePushTimedClient.serverName);
		parameter.getString("basePushTimedClient", "serviceName",
				ini.basePushTimedClient.serviceName);
		parameter.getInt("basePushTimedClient", "interval",
				ini.basePushTimedClient.interval);
		parameter.getString("ptuDeviceStateServer", "serverName",
				ini.ptuDeviceStateServer.serverName);
		parameter.getString("ptuDeviceStateServer", "serviceName",
				ini.ptuDeviceStateServer.serviceName);
		parameter.getDouble("base", "base_a", ini.base.base_a);
		parameter.getTruthValue("base", "on_base", ini.base.on_base);
		parameter.getTruthValue("base", "on_ptu", ini.base.on_ptu);
		parameter.getDouble("base", "steer_a", ini.base.steer_a);
		parameter.getDouble("base", "turret_a", ini.base.turret_a);
		parameter.getInt("base", "x", ini.base.x);
		parameter.getInt("base", "y", ini.base.y);
		parameter.getInt("base", "z", ini.base.z);
		parameter.getTruthValue("general", "debug_info", ini.general.debug_info);
		parameter.getTruthValue("general", "initialize_camera",
				ini.general.initialize_camera);
		parameter.getString("general", "processing_parameter",
				ini.general.processing_parameter);
		parameter.getString("general", "processing_plugin",
				ini.general.processing_plugin);
		parameter.getString("general", "source_parameter",
				ini.general.source_parameter);
		parameter.getString("general", "source_plugin",
				ini.general.source_plugin);
		parameter.getDouble("general", "valid_image_time",
				ini.general.valid_image_time);
		parameter.getTruthValue("hardware_properties",
				"activate_lense_calibration",
				ini.hardware_properties.activate_lense_calibration);
		parameter.getTruthValue("hardware_properties",
				"activate_suppress_motion_blur",
				ini.hardware_properties.activate_suppress_motion_blur);
		parameter.getInt("hardware_properties", "height",
				ini.hardware_properties.height);
		parameter.getInt("hardware_properties", "integration_time",
				ini.hardware_properties.integration_time);
		parameter.getDouble("hardware_properties", "max_distance",
				ini.hardware_properties.max_distance);
		parameter.getDouble("hardware_properties", "min_distance",
				ini.hardware_properties.min_distance);
		parameter.getInt("hardware_properties", "modulation_frequency",
				ini.hardware_properties.modulation_frequency);
		parameter.getDouble("hardware_properties", "opening_angle_x_axis",
				ini.hardware_properties.opening_angle_x_axis);
		parameter.getDouble("hardware_properties", "opening_angle_y_axis",
				ini.hardware_properties.opening_angle_y_axis);
		parameter.getInt("hardware_properties", "width",
				ini.hardware_properties.width);
		parameter.getDouble("sensor_pose", "azimuth", ini.sensor_pose.azimuth);
		parameter.getDouble("sensor_pose", "elevation",
				ini.sensor_pose.elevation);
		parameter.getDouble("sensor_pose", "roll", ini.sensor_pose.roll);
		parameter.getInt("sensor_pose", "x", ini.sensor_pose.x);
		parameter.getInt("sensor_pose", "y", ini.sensor_pose.y);
		parameter.getInt("sensor_pose", "z", ini.sensor_pose.z);

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

