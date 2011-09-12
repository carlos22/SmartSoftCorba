//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.8.0
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

#ifndef _SAMPLEPUSHNEWESTCLIENT_HH
#define _SAMPLEPUSHNEWESTCLIENT_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SamplePushNewestClientCore.hh"
// include communication objects
#include <CommSampleObjects/commSampleTime.hh>

// include tasks	

#include "../TimeTask.hh"
// include handler
#include "../CompHandler.hh"

#define COMP SamplePushNewestClient::instance()

class SamplePushNewestClient: public SamplePushNewestClientCore
{
private:
	static SamplePushNewestClient _samplePushNewestClient;

	// constructor
	SamplePushNewestClient();

	// copy-constructor
	SamplePushNewestClient(const SamplePushNewestClient& cc);

	// destructor
	~SamplePushNewestClient()
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

	TimeTask timeTask;

	// ports
	CHS::PushNewestClient<CommSampleObjects::CommSampleTime> *timeClient;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SamplePushNewestClient* instance()
	{
		return (SamplePushNewestClient*) &_samplePushNewestClient;
	}

	// ini parameter
	struct ini_ini
	{

		struct ini_timeClient
		{
			std::string serverName;
			std::string serviceName;
		} timeClient;

	} ini;

};
#endif