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

#ifndef _NAVIGATIONVELOCITYHANDLERCORE_HH
#define _NAVIGATIONVELOCITYHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <CommBasicObjects/commBaseParameter.hh>

#include <CommBasicObjects/commBasePositionUpdate.hh>

#include <CommBasicObjects/commBaseState.hh>

#include <CommBasicObjects/commBatteryEvent.hh>

#include <CommBasicObjects/commBatteryParameter.hh>

#include <CommBasicObjects/commBatteryState.hh>

#include <CommBasicObjects/commNavigationVelocity.hh>

#include <CommBasicObjects/commVoid.hh>

class NavigationVelocityHandlerCore: public CHS::SendServerHandler<
		CommBasicObjects::CommNavigationVelocity>
{
public:
	void handleSend(const CommBasicObjects::CommNavigationVelocity &r) throw()
	{
	}
	;
};
#endif
