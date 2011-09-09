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

#ifndef _SMARTPLANNERBREADTHFIRSTSEARCH_HH
#define _SMARTPLANNERBREADTHFIRSTSEARCH_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartPlannerBreadthFirstSearchCore.hh"
// include communication objects
#include <CommBasicObjects/commBaseState.hh>
#include <CommNavigationObjects/commGridMap.hh>
#include <CommNavigationObjects/commPlannerEventParameter.hh>
#include <CommNavigationObjects/commPlannerEventResult.hh>
#include <CommNavigationObjects/commPlannerGoal.hh>
#include <CommNavigationObjects/commPlannerParameter.hh>
#include <CommNavigationObjects/plannerEventState.hh>
// include tasks	

#include "../PlannerTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../ParameterHandler.hh"
#include "../PlannerTestHandler.hh"
#include "../PlannerTriggerTimer.hh"
#include "../StateChangeHandler.hh"

#define COMP SmartPlannerBreadthFirstSearch::instance()

class SmartPlannerBreadthFirstSearch: public SmartPlannerBreadthFirstSearchCore
{
private:
	static SmartPlannerBreadthFirstSearch _smartPlannerBreadthFirstSearch;

	// constructor
	SmartPlannerBreadthFirstSearch();

	// copy-constructor
	SmartPlannerBreadthFirstSearch(const SmartPlannerBreadthFirstSearch& cc);

	// destructor
	~SmartPlannerBreadthFirstSearch()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	ParameterHandler parameterHandler;
	PlannerTestHandler plannerTestHandler;
	PlannerTriggerTimer plannerTriggerTimer;
	StateChangeHandler stateChangeHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex PlannerGlobalLock;
	CHS::SmartMutex PlannerGoalLock;
	CHS::SmartMutex PlannerTriggerLock;

	// create condition mutex
	CHS::SmartConditionMutex PlannerTrigger;//(PlannerTriggerLock);


	// instantiate tasks

	PlannerTask plannerTask;

	// ports
	CHS::PushTimedClient<CommBasicObjects::CommBaseState> *baseClient;
	CHS::PushNewestClient<CommNavigationObjects::CommGridMap> *curMapClient;
	CHS::SendServer<CommNavigationObjects::CommPlannerParameter> *paramServer;
	CHS::EventServer<CommNavigationObjects::CommPlannerEventParameter,
			CommNavigationObjects::CommPlannerEventResult,
			CommNavigationObjects::PlannerEventState> *plannerEventServer;
	CHS::PushNewestServer<CommNavigationObjects::CommPlannerGoal>
			*plannerGoalServer;
	CHS::SmartStateServer *stateServer;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartPlannerBreadthFirstSearch* instance()
	{
		return (SmartPlannerBreadthFirstSearch*) &_smartPlannerBreadthFirstSearch;
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

		struct ini_paramServer
		{
			std::string serviceName;
		} paramServer;

		struct ini_plannerEventServer
		{
			std::string serviceName;
		} plannerEventServer;

		struct ini_plannerGoalServer
		{
			std::string serviceName;
		} plannerGoalServer;

		struct ini_stateServer
		{
			std::string serviceName;
		} stateServer;

		struct ini_baseClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} baseClient;

		struct ini_curMapClient
		{
			std::string serverName;
			std::string serviceName;
		} curMapClient;

		struct ini_Settings
		{

			double no_path_event_timeout;
		} Settings;
	} ini;

};
#endif
