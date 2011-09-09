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

#include "SmartVisualization.hh"

// constructor
SmartVisualization::SmartVisualization()
{
	std::cout << "constructor of SmartVisualization\n";

	ini.component.name = "SmartVisualization";

	ini.baseClient.serverName = "SmartPlayerStageSimulator";
	ini.baseClient.serviceName = "basestate";
	ini.baseClient.interval = 1;
	ini.curPushClient.serverName = "SmartMapperGridMap";
	ini.curPushClient.serviceName = "currentGridMap";
	ini.kinectPushNewestClient.serverName = "SmartKinectServer";
	ini.kinectPushNewestClient.serviceName = "imageNewest";
	ini.laserClient1.serverName = "SmartPlayerStageSimulator";
	ini.laserClient1.serviceName = "scan";
	ini.laserClient2.serverName = "SmartLaserLMS1xxServer";
	ini.laserClient2.serviceName = "scan";
	ini.laserClient3.serverName = "SmartLaserLMS200Server";
	ini.laserClient3.serviceName = "scan";
	ini.lmtQueryClient.serverName = "SmartMapperGridMap";
	ini.lmtQueryClient.serviceName = "ltmGridMapQuery";
	ini.personDetectionEventClient.serverName = "SmartKinectPersonDetection";
	ini.personDetectionEventClient.serviceName = "personEventServer";
	ini.personDetectionQueryClient.serverName = "SmartKinectPersonDetection";
	ini.personDetectionQueryClient.serviceName = "person";
	ini.tofImageClient.serverName = "SmartPMDServer";
	ini.tofImageClient.serviceName = "imageNewest";
	ini.services.show_base = true;
	ini.services.show_cur_map = true;
	ini.services.show_kinect_image = false;
	ini.services.show_laser_1 = true;
	ini.services.show_laser_2 = false;
	ini.services.show_laser_3 = false;
	ini.services.show_ltm = true;
	ini.services.show_person_detection = false;
	ini.services.show_tof_image = false;
}

void SmartVisualization::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartVisualization is named "
				<< ini.component.name << "." << std::endl;

		// create ports
		baseClient = new CHS::PushTimedClient<CommBasicObjects::CommBaseState>(
				component);
		curPushClient = new CHS::PushNewestClient<
				CommNavigationObjects::CommGridMap>(component);
		kinectPushNewestClient = new CHS::PushNewestClient<
				CommVisionObjects::CommKinectImage>(component);
		laserClient1 = new CHS::PushNewestClient<
				CommBasicObjects::CommMobileLaserScan>(component);
		laserClient2 = new CHS::PushNewestClient<
				CommBasicObjects::CommMobileLaserScan>(component);
		laserClient3 = new CHS::PushNewestClient<
				CommBasicObjects::CommMobileLaserScan>(component);
		lmtQueryClient = new CHS::QueryClient<
				CommNavigationObjects::CommGridMapRequest,
				CommNavigationObjects::CommGridMap>(component);

		personDetectionEventClient = new CHS::EventClient<
				CommBasicObjects::CommVoid,
				CommPersonDetectionObjects::CommPersonDetectionEventResult>(
				component);

		personDetectionQueryClient = new CHS::QueryClient<
				CommPersonDetectionObjects::CommPersonId,
				CommPersonDetectionObjects::CommDetectedPerson>(component);
		tofImageClient = new CHS::PushNewestClient<
				CommVisionObjects::CommToFImage>(component);
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		baseClient->add(wiringSlave, "base");
		curPushClient->add(wiringSlave, "currentGridMap");
		kinectPushNewestClient->add(wiringSlave, "kinect");
		laserClient1->add(wiringSlave, "laser1");
		laserClient2->add(wiringSlave, "laser2");
		laserClient3->add(wiringSlave, "laser3");
		lmtQueryClient->add(wiringSlave, "ltmGridMap");
		personDetectionEventClient->add(wiringSlave, "personDetectionEvent");
		personDetectionQueryClient->add(wiringSlave, "personDetectionQuery");
		tofImageClient->add(wiringSlave, "tofImage");

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartVisualization::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartVisualization::loadParameter(int argc, char *argv[])
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
			std::cout << "load SmartVisualization.ini parameter file\n";
			parameter.addFile("SmartVisualization.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("baseClient", "serverName",
				ini.baseClient.serverName);
		parameter.getString("baseClient", "serviceName",
				ini.baseClient.serviceName);
		parameter.getInt("baseClient", "interval", ini.baseClient.interval);
		parameter.getString("curPushClient", "serverName",
				ini.curPushClient.serverName);
		parameter.getString("curPushClient", "serviceName",
				ini.curPushClient.serviceName);
		parameter.getString("kinectPushNewestClient", "serverName",
				ini.kinectPushNewestClient.serverName);
		parameter.getString("kinectPushNewestClient", "serviceName",
				ini.kinectPushNewestClient.serviceName);
		parameter.getString("laserClient1", "serverName",
				ini.laserClient1.serverName);
		parameter.getString("laserClient1", "serviceName",
				ini.laserClient1.serviceName);
		parameter.getString("laserClient2", "serverName",
				ini.laserClient2.serverName);
		parameter.getString("laserClient2", "serviceName",
				ini.laserClient2.serviceName);
		parameter.getString("laserClient3", "serverName",
				ini.laserClient3.serverName);
		parameter.getString("laserClient3", "serviceName",
				ini.laserClient3.serviceName);
		parameter.getString("lmtQueryClient", "serverName",
				ini.lmtQueryClient.serverName);
		parameter.getString("lmtQueryClient", "serviceName",
				ini.lmtQueryClient.serviceName);
		parameter.getString("personDetectionEventClient", "serverName",
				ini.personDetectionEventClient.serverName);
		parameter.getString("personDetectionEventClient", "serviceName",
				ini.personDetectionEventClient.serviceName);
		parameter.getString("personDetectionQueryClient", "serverName",
				ini.personDetectionQueryClient.serverName);
		parameter.getString("personDetectionQueryClient", "serviceName",
				ini.personDetectionQueryClient.serviceName);
		parameter.getString("tofImageClient", "serverName",
				ini.tofImageClient.serverName);
		parameter.getString("tofImageClient", "serviceName",
				ini.tofImageClient.serviceName);
		parameter.getTruthValue("services", "show_base", ini.services.show_base);
		parameter.getTruthValue("services", "show_cur_map",
				ini.services.show_cur_map);
		parameter.getTruthValue("services", "show_kinect_image",
				ini.services.show_kinect_image);
		parameter.getTruthValue("services", "show_laser_1",
				ini.services.show_laser_1);
		parameter.getTruthValue("services", "show_laser_2",
				ini.services.show_laser_2);
		parameter.getTruthValue("services", "show_laser_3",
				ini.services.show_laser_3);
		parameter.getTruthValue("services", "show_ltm", ini.services.show_ltm);
		parameter.getTruthValue("services", "show_person_detection",
				ini.services.show_person_detection);
		parameter.getTruthValue("services", "show_tof_image",
				ini.services.show_tof_image);

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

