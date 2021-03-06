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

#include "SmartXsensIMUMTiClient.hh"

// constructor
SmartXsensIMUMTiClient::SmartXsensIMUMTiClient()
{
	std::cout << "constructor of SmartXsensIMUMTiClient\n";

	ini.component.name = "SmartXsensIMUMTiClient";

	ini.pushTimedDataClient.serverName = "SmartXsensIMUMTiServer";
	ini.pushTimedDataClient.serviceName = "imuData";
	ini.pushTimedDataClient.interval = 5;
	ini.viewer.imusamplerate = 120;
}

void SmartXsensIMUMTiClient::init(int argc, char *argv[])
{
	try
	{
		loadParameter(argc, argv);
		component = new CHS::SmartComponent(ini.component.name, argc, argv);

		std::cout << "Component SmartXsensIMUMTiClient is named "
				<< ini.component.name << "." << std::endl;

		// create ports
		pushTimedDataClient = new CHS::PushTimedClient<
				CommBasicObjects::CommIMUData>(component);
		wiringSlave = new CHS::WiringSlave(component);

		// add client port to wiring slave
		pushTimedDataClient->add(wiringSlave, "pushTimedDataClient");

	} catch (const CORBA::Exception &)
	{
		std::cerr << "Uncaught CORBA exception" << std::endl;
	} catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}

// run the component
void SmartXsensIMUMTiClient::run()
{
	compHandler.onStartup();
	component->run();
	delete component;
}

void SmartXsensIMUMTiClient::loadParameter(int argc, char *argv[])
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
			std::cout << "load SmartXsensIMUMTiClient.ini parameter file\n";
			parameter.addFile("SmartXsensIMUMTiClient.ini");
		}

		// than add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLine("", argc, argv);

		// print all known parameters
		parameter.print(); // TODO remove this


		// load parameter
		parameter.getString("component", "name", ini.component.name);

		parameter.getString("pushTimedDataClient", "serverName",
				ini.pushTimedDataClient.serverName);
		parameter.getString("pushTimedDataClient", "serviceName",
				ini.pushTimedDataClient.serviceName);
		parameter.getInt("pushTimedDataClient", "interval",
				ini.pushTimedDataClient.interval);
		parameter.getInt("viewer", "imusamplerate", ini.viewer.imusamplerate);

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

