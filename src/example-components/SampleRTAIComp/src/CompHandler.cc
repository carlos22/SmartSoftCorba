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
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

#include "CompHandler.hh"
#include "gen/SampleRTAIComp.hh"

#include <iostream>

// include communication objects

void CompHandler::onStartup()
{
	std::cout << "startup - put your startupCode in CompHandler::onStartup()!!!\n";

	// run all tasks
	COMP->task1.open();
	COMP->task2.open();
	COMP->rtTask1.open();
	COMP->rtTask2.open();
	COMP->rtTask3.open();

	std::cout << "all tasks running...\n";
}
