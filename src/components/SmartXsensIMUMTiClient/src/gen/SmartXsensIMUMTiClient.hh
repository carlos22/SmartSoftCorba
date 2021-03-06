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

#ifndef _SMARTXSENSIMUMTICLIENT_HH
#define _SMARTXSENSIMUMTICLIENT_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartXsensIMUMTiClientCore.hh"
// include communication objects
#include <CommBasicObjects/commIMUData.hh>
// include tasks	

#include "../GuiTask.hh"
// include handler
#include "../CompHandler.hh"

#define COMP SmartXsensIMUMTiClient::instance()

class SmartXsensIMUMTiClient: public SmartXsensIMUMTiClientCore
{
private:
	static SmartXsensIMUMTiClient _smartXsensIMUMTiClient;

	// constructor
	SmartXsensIMUMTiClient();

	// copy-constructor
	SmartXsensIMUMTiClient(const SmartXsensIMUMTiClient& cc);

	// destructor
	~SmartXsensIMUMTiClient()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;

	// ThreadQueueHandler 

public:
	// component
	CHS::SmartComponent *component;

	// create mutex


	// create condition mutex


	// instantiate tasks

	GuiTask guiTask;

	// ports
	CHS::PushTimedClient<CommBasicObjects::CommIMUData> *pushTimedDataClient;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartXsensIMUMTiClient* instance()
	{
		return (SmartXsensIMUMTiClient*) &_smartXsensIMUMTiClient;
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

		struct ini_pushTimedDataClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} pushTimedDataClient;

		struct ini_viewer
		{

			long imusamplerate;
		} viewer;
	} ini;

};
#endif
