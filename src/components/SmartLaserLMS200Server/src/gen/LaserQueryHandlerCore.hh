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

#ifndef _LASERQUERYHANDLERCORE_HH
#define _LASERQUERYHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <CommBasicObjects/commBaseState.hh>

#include <CommBasicObjects/commMobileLaserScan.hh>

#include <CommBasicObjects/commVoid.hh>

class LaserQueryHandlerCore: public CHS::QueryServerHandler<
		CommBasicObjects::CommVoid, CommBasicObjects::CommMobileLaserScan>
{
public:
	void handleQuery(CHS::QueryServer<CommBasicObjects::CommVoid,
			CommBasicObjects::CommMobileLaserScan> & server,
			const CHS::QueryId id, const CommBasicObjects::CommVoid & request) throw()
	{
	}
	;
};
#endif
