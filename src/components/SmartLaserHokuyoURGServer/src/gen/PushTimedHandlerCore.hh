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

#ifndef _PUSHTIMEDHANDLERCORE_HH
#define _PUSHTIMEDHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include "externalCommObjectHeaders.hh"
#include <CommManipulatorObjects/commMobileManipulatorState.hh>

class PushTimedHandlerCore: public CHS::PushTimedHandler<
		CommExternalObjects::CommMobileLaserScan>
{
public:
	void handlePushTimer(CHS::PushTimedServer<
			CommExternalObjects::CommMobileLaserScan> & server) throw()
	{
	}
	;
};
#endif