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

#ifndef _GOALEVENTTESTHANDLERCORE_HH
#define _GOALEVENTTESTHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <CommBasicObjects/commBaseState.hh>

#include <CommBasicObjects/commDevicePoseState.hh>

#include <CommBasicObjects/commVoid.hh>

#include <CommPTUObjects/commPTUGoalEventParameter.hh>

#include <CommPTUObjects/commPTUGoalEventResult.hh>

#include <CommPTUObjects/commPTUMoveRequest.hh>

#include <CommPTUObjects/commPTUMoveResponse.hh>

#include <CommPTUObjects/commPTUParameter.hh>

#include <CommPTUObjects/pTUGoalEventState.hh>

class GoalEventTestHandlerCore: public CHS::EventTestHandler<
		CommPTUObjects::CommPTUGoalEventParameter,
		CommPTUObjects::CommPTUGoalEventResult,
		CommPTUObjects::PTUGoalEventState>
{
public:
	bool testEvent(CommPTUObjects::CommPTUGoalEventParameter &p,
			CommPTUObjects::CommPTUGoalEventResult &r,
			const CommPTUObjects::PTUGoalEventState &s) throw()
	{
		return false;
	}
	;
};
#endif
