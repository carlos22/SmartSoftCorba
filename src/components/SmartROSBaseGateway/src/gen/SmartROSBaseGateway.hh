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

#ifndef _SMARTROSBASEGATEWAY_HH
#define _SMARTROSBASEGATEWAY_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartROSBaseGatewayCore.hh"
// include communication objects
#include <CommBasicObjects/commBasePositionUpdate.hh>
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commNavigationVelocity.hh>
#include <CommBasicObjects/commVoid.hh>
// include tasks	

#include "../ROS_SpinTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../BasePositionUpdateSendHandler.hh"
#include "../BaseStatePushTimedHandler.hh"
#include "../BaseStateQueryHandler.hh"
#include "../NavigationVelocitySendServerHandler.hh"

#define COMP SmartROSBaseGateway::instance()

class SmartROSBaseGateway: public SmartROSBaseGatewayCore
{
private:
	static SmartROSBaseGateway _smartROSBaseGateway;

	// constructor
	SmartROSBaseGateway();

	// copy-constructor
	SmartROSBaseGateway(const SmartROSBaseGateway& cc);

	// destructor
	~SmartROSBaseGateway()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	BasePositionUpdateSendHandler basePositionUpdateSendHandler;
	BaseStatePushTimedHandler baseStatePushTimedHandler;
	BaseStateQueryHandler baseStateQueryHandler;
	NavigationVelocitySendServerHandler navigationVelocitySendServerHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex GlobalLock;

	// create condition mutex


	// instantiate tasks

	ROS_SpinTask rOS_SpinTask;

	// ports
	CHS::PushTimedServer<CommBasicObjects::CommBaseState>
			*basePositionPushTimedServer;
	CHS::SendServer<CommBasicObjects::CommBasePositionUpdate>
			*basePositionUpdateSendServer;
	CHS::QueryServer<CommBasicObjects::CommVoid,
			CommBasicObjects::CommBaseState> *baseStateQueryServer;
	CHS::SendServer<CommBasicObjects::CommNavigationVelocity>
			*navigationVelocitySendServer;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartROSBaseGateway* instance()
	{
		return (SmartROSBaseGateway*) &_smartROSBaseGateway;
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

		struct ini_basePositionPushTimedServer
		{
			std::string serviceName;
			double cycle;
		} basePositionPushTimedServer;

		struct ini_basePositionUpdateSendServer
		{
			std::string serviceName;
		} basePositionUpdateSendServer;

		struct ini_baseStateQueryServer
		{
			std::string serviceName;
		} baseStateQueryServer;

		struct ini_navigationVelocitySendServer
		{
			std::string serviceName;
		} navigationVelocitySendServer;

		struct ini_ROS
		{

			std::string raw_odom_subscr_name;

			std::string vel_publ_name;
		} ROS;

		struct ini_general
		{

			bool start_push_timed;

			bool verbose;
		} general;
	} ini;

};
#endif