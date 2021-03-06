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

#ifndef _SMARTPTUTESTCLIENT_HH
#define _SMARTPTUTESTCLIENT_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartPTUTestClientCore.hh"
// include communication objects
#include <CommBasicObjects/commDevicePoseState.hh>
#include <CommPTUObjects/commPTUGoalEventParameter.hh>
#include <CommPTUObjects/commPTUGoalEventResult.hh>
#include <CommPTUObjects/commPTUMoveRequest.hh>
#include <CommPTUObjects/commPTUMoveResponse.hh>
#include <CommPTUObjects/commPTUParameter.hh>
// include tasks	

#include "../DevicePoseStateTask.hh"
#include "../TestTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../GoalEventHandler.hh"

#define COMP SmartPTUTestClient::instance()

class SmartPTUTestClient: public SmartPTUTestClientCore
{
private:
	static SmartPTUTestClient _smartPTUTestClient;

	// constructor
	SmartPTUTestClient();

	// copy-constructor
	SmartPTUTestClient(const SmartPTUTestClient& cc);

	// destructor
	~SmartPTUTestClient()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	GoalEventHandler goalEventHandler;

	// ThreadQueueHandler 

public:
	// component
	CHS::SmartComponent *component;

	// create mutex


	// create condition mutex


	// instantiate tasks

	DevicePoseStateTask devicePoseStateTask;
	TestTask testTask;

	// ports
	CHS::PushNewestClient<CommBasicObjects::CommDevicePoseState>
			*devicePoseStateClient;
	CHS::EventClient<CommPTUObjects::CommPTUGoalEventParameter,
			CommPTUObjects::CommPTUGoalEventResult> *goalEventClient;
	CHS::QueryClient<CommPTUObjects::CommPTUMoveRequest,
			CommPTUObjects::CommPTUMoveResponse> *moveQueryClient;
	CHS::SendClient<CommPTUObjects::CommPTUMoveRequest> *moveSendClient;
	CHS::SendClient<CommPTUObjects::CommPTUParameter> *paramClient;
	CHS::SmartStateClient *stateClient;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartPTUTestClient* instance()
	{
		return (SmartPTUTestClient*) &_smartPTUTestClient;
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

		struct ini_devicePoseStateClient
		{
			std::string serverName;
			std::string serviceName;
		} devicePoseStateClient;

		struct ini_goalEventClient
		{
			std::string serverName;
			std::string serviceName;
		} goalEventClient;

		struct ini_moveQueryClient
		{
			std::string serverName;
			std::string serviceName;
		} moveQueryClient;

		struct ini_moveSendClient
		{
			std::string serverName;
			std::string serviceName;
		} moveSendClient;

		struct ini_paramClient
		{
			std::string serverName;
			std::string serviceName;
		} paramClient;

		struct ini_stateClient
		{
			std::string serverName;
			std::string serviceName;
		} stateClient;

	} ini;

};
#endif
