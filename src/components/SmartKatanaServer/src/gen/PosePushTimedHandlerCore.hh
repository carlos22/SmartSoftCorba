//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.2
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

#ifndef _POSEPUSHTIMEDHANDLERCORE_HH
#define _POSEPUSHTIMEDHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <CommBasicObjects/commBaseState.hh>

#include <CommBasicObjects/commMobileLaserScan.hh>

#include <CommBasicObjects/commVoid.hh>

#include <CommManipulatorObjects/commManipulatorEventParameter.hh>

#include <CommManipulatorObjects/commManipulatorEventResult.hh>

#include <CommManipulatorObjects/commManipulatorId.hh>

#include <CommManipulatorObjects/commManipulatorParameter.hh>

#include <CommManipulatorObjects/commManipulatorRequestScan3d.hh>

#include <CommManipulatorObjects/commManipulatorTrajectory.hh>

#include <CommManipulatorObjects/commMobileManipulatorState.hh>

#include <CommManipulatorObjects/manipulatorEventState.hh>

#include <CommManipulatorObjects/scanEventState.hh>

#include <CommVisionObjects/comm3dPointCloud.hh>

class PosePushTimedHandlerCore: public CHS::PushTimedHandler<
		CommManipulatorObjects::CommMobileManipulatorState>
{
public:
	void handlePushTimer(CHS::PushTimedServer<
			CommManipulatorObjects::CommMobileManipulatorState> & server) throw()
	{
	}
	;
};
#endif