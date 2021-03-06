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

#ifndef _SMARTXSENSIMUMTISERVER_HH
#define _SMARTXSENSIMUMTISERVER_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartXsensIMUMTiServerCore.hh"
// include communication objects
#include <CommBasicObjects/commIMUData.hh>
// include tasks	

#include "../WorkerTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../PushTimedDataHandler.hh"

#define COMP SmartXsensIMUMTiServer::instance()

class SmartXsensIMUMTiServer: public SmartXsensIMUMTiServerCore
{
private:
	static SmartXsensIMUMTiServer _smartXsensIMUMTiServer;

	// constructor
	SmartXsensIMUMTiServer();

	// copy-constructor
	SmartXsensIMUMTiServer(const SmartXsensIMUMTiServer& cc);

	// destructor
	~SmartXsensIMUMTiServer()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	PushTimedDataHandler pushTimedDataHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex


	// create condition mutex


	// instantiate tasks

	WorkerTask workerTask;

	// ports
	CHS::PushTimedServer<CommBasicObjects::CommIMUData> *pushTimedDataServer;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartXsensIMUMTiServer* instance()
	{
		return (SmartXsensIMUMTiServer*) &_smartXsensIMUMTiServer;
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

		struct ini_pushTimedDataServer
		{
			std::string serviceName;
			double cycle;
		} pushTimedDataServer;

		struct ini_xsens
		{

			std::string port;

			bool verbose;
		} xsens;
	} ini;

};
#endif
