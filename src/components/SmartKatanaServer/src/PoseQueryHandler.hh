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
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

#ifndef _POSEQUERYHANDLER_HH
#define _POSEQUERYHANDLER_HH

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

#include "gen/PoseQueryHandlerCore.hh"

class PoseQueryHandler: public PoseQueryHandlerCore
{

public:

	PoseQueryHandler();

	void
			handleQuery(
					CHS::QueryServer<CommBasicObjects::CommVoid,
							CommManipulatorObjects::CommMobileManipulatorState> & server,
					const CHS::QueryId id,
					const CommBasicObjects::CommVoid & request) throw();

	CommBasicObjects::CommBaseState default_baseState;
};

#endif
