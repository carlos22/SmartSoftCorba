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

#include "StateChangeHandler.hh"
#include "gen/SmartGMapping.hh"

#include <iostream>

// Called when a substate is entered
void StateChangeHandler::handleEnterState(const std::string & substate) throw()
{
	std::cout << "    enterHandler  activate   " << substate << std::endl;
}

// Called when a substate is left
void StateChangeHandler::handleQuitState(const std::string & substate) throw()
{
	std::cout << "    quitHandler   deactivate " << substate << std::endl;
}