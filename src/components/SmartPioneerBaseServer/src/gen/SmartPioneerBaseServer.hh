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

#ifndef _SMARTPIONEERBASESERVER_HH
#define _SMARTPIONEERBASESERVER_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartPioneerBaseServerCore.hh"
// include communication objects
#include <CommBasicObjects/commBaseParameter.hh>
#include <CommBasicObjects/commBasePositionUpdate.hh>
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commBatteryEvent.hh>
#include <CommBasicObjects/commBatteryParameter.hh>
#include <CommBasicObjects/commBatteryState.hh>
#include <CommBasicObjects/commNavigationVelocity.hh>
#include <CommBasicObjects/commVoid.hh>
// include tasks	

// include handler
#include "../CompHandler.hh"
#include "../BasePositionUpdateHandler.hh"
#include "../BaseStateHandler.hh"
#include "../BaseStateQueryHandler.hh"
#include "../BatteryEventHandler.hh"
#include "../NavigationVelocityHandler.hh"
#include "../ParameterHandler.hh"

#define COMP SmartPioneerBaseServer::instance()

class SmartPioneerBaseServer: public SmartPioneerBaseServerCore
{
private:
	static SmartPioneerBaseServer _smartPioneerBaseServer;

	// constructor
	SmartPioneerBaseServer();

	// copy-constructor
	SmartPioneerBaseServer(const SmartPioneerBaseServer& cc);

	// destructor
	~SmartPioneerBaseServer()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	BasePositionUpdateHandler basePositionUpdateHandler;
	BaseStateHandler baseStateHandler;
	BaseStateQueryHandler baseStateQueryHandler;
	BatteryEventHandler batteryEventHandler;
	NavigationVelocityHandler navigationVelocityHandler;
	ParameterHandler parameterHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex


	// create condition mutex


	// instantiate tasks


	// ports
	CHS::PushTimedServer<CommBasicObjects::CommBaseState> *basePositionServer;
	CHS::SendServer<CommBasicObjects::CommBasePositionUpdate>
			*basePositionUpdateServer;
	CHS::QueryServer<CommBasicObjects::CommVoid,
			CommBasicObjects::CommBaseState> *baseQueryServer;
	CHS::EventServer<CommBasicObjects::CommBatteryParameter,
			CommBasicObjects::CommBatteryEvent,
			CommBasicObjects::CommBatteryState> *batteryEventServer;
	CHS::SendServer<CommBasicObjects::CommNavigationVelocity>
			*navigationVelocityServer;
	CHS::SendServer<CommBasicObjects::CommBaseParameter> *paramServer;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartPioneerBaseServer* instance()
	{
		return (SmartPioneerBaseServer*) &_smartPioneerBaseServer;
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

		struct ini_basePositionServer
		{
			std::string serviceName;
			double cycle;
		} basePositionServer;

		struct ini_basePositionUpdateServer
		{
			std::string serviceName;
		} basePositionUpdateServer;

		struct ini_baseQueryServer
		{
			std::string serviceName;
		} baseQueryServer;

		struct ini_batteryEventServer
		{
			std::string serviceName;
		} batteryEventServer;

		struct ini_navigationVelocityServer
		{
			std::string serviceName;
		} navigationVelocityServer;

		struct ini_paramServer
		{
			std::string serviceName;
		} paramServer;

		struct ini_robot
		{

			bool enable_motors;

			bool enable_sonar;

			long maxRotVel;

			long maxRotVelAcc;

			long maxRotVelDecel;

			long maxVel;

			long maxVelAcc;

			long maxVelDecel;

			std::string robotType;

			std::string serialport;
		} robot;
	} ini;

};
#endif
