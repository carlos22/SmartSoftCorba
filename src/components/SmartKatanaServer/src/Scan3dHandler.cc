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
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

#include "Scan3dHandler.hh"
#include "gen/SmartKatanaServer.hh"

#include "Katana.hh"

#include <iostream>

void Scan3dHandler::handleQuery(CHS::QueryServer<Smart::CommManipulatorId,
		Smart::Comm3dPointCloud> & server, const CHS::QueryId id,
		const Smart::CommManipulatorId & request) throw ()
{
	// change this code to your needs !!!
	Smart::Comm3dPointCloud answer;

	KATANA->get3dPointCloud(request, answer);

	server.answer(id, answer);
}