//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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

#ifndef _EVENTTESTHANDLERCORE_HH
#define _EVENTTESTHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include "externalCommObjectHeaders.hh"
#include <comm3dPointCloud.hh>

#include <commManipulatorEventParameter.hh>

#include <commManipulatorEventResult.hh>

#include <commManipulatorId.hh>

#include <commManipulatorParameter.hh>

#include <commManipulatorRequestScan3d.hh>

#include <commManipulatorTrajectory.hh>

#include <commMobileManipulatorState.hh>

#include <manipulatorEventState.hh>

#include <scanEventState.hh>

class EventTestHandlerCore: public CHS::EventTestHandler<
		Smart::CommManipulatorEventParameter,
		Smart::CommManipulatorEventResult, Smart::ManipulatorEventState>
{
public:
	bool testEvent(Smart::CommManipulatorEventParameter &p,
			Smart::CommManipulatorEventResult &r,
			const Smart::ManipulatorEventState &s) throw()
	{
		return false;
	}
	;
};
#endif