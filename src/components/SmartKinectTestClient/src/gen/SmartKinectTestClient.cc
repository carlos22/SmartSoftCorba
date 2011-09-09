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

#include "SmartKinectTestClient.hh"

// constructor
SmartKinectTestClient::SmartKinectTestClient()
{
	std::cout << "constructor of SmartKinectTestClient\n";

	ini.component.name = "SmartKinectTestClient";

	ini.imagePushNewestClient.serverName = "SmartKinectServer";
	ini.imagePushNewestClient.serviceName = "imageNewest";
	ini.kinectQueryClient.serverName = "SmartKinectServer";
	ini.kinectQueryClient.serviceName = "imageQuery";
	ini.stateClient.serverName = "SmartKinectServer";
	ini.stateClient.serviceName = "state";
	ini.general.filename = "data/kinect_img.xml";
	ini.general.save_to_file = false;
}

void SmartKinectTestClient::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartKinectTestClient is named "
				<< ini.component.name << "." << std::endl;

		// create ports
		imagePushNewestClient = new CHS::PushNewestClient<
				CommVisionObjects::CommKinectImage>(component);
		kinectQueryClient = new CHS::QueryClient<CommBasicObjects::CommVoid,
				CommVisionObjects::CommKinectImage>(component);
		stateClient = new CHS::SmartStateClient(component);
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		imagePushNewestClient->add(wiringSlave, "imagePushNewestClient");
		kinectQueryClient->add(wiringSlave, "kinectQueryClient");
		stateClient->add(wiringSlave, "stateClient");

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartKinectTestClient::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartKinectTestClient::loadParameter(int argc, char *argv[])
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
			std::cout << "load SmartKinectTestClient.ini parameter file\n";
			parameter.addFile("SmartKinectTestClient.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("imagePushNewestClient", "serverName",
				ini.imagePushNewestClient.serverName);
		parameter.getString("imagePushNewestClient", "serviceName",
				ini.imagePushNewestClient.serviceName);
		parameter.getString("kinectQueryClient", "serverName",
				ini.kinectQueryClient.serverName);
		parameter.getString("kinectQueryClient", "serviceName",
				ini.kinectQueryClient.serviceName);
		parameter.getString("stateClient", "serverName",
				ini.stateClient.serverName);
		parameter.getString("stateClient", "serviceName",
				ini.stateClient.serviceName);
		parameter.getString("general", "filename", ini.general.filename);
		parameter.getTruthValue("general", "save_to_file",
				ini.general.save_to_file);

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

