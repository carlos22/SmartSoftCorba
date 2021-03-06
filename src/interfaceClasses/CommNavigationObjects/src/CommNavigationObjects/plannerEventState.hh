//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
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
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

#ifndef _COMM_COMMNAVIGATIONOBJECTS_PLANNEREVENTSTATE_HH
#define _COMM_COMMNAVIGATIONOBJECTS_PLANNEREVENTSTATE_HH

#include "CommNavigationObjects/gen/enumPlannerEventType.hh"

#include "CommNavigationObjects/plannerEventState.hh"

namespace CommNavigationObjects
{

class PlannerEventState
{

protected:
	CommNavigationObjects::PlannerEventType newState;

public:
	PlannerEventState()
	{
	}
	virtual ~PlannerEventState()
	{
	}

	//
	// user interfaces
	void set(PlannerEventType);
	void get(PlannerEventType&) const;

};

}
#endif
