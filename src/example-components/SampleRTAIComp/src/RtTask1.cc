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

#include "RtTask1.hh"
#include "gen/SampleRTAIComp.hh"

#include <iostream>

RtTask1::RtTask1()
{
	std::cout << "constructor RtTask1\n";
}

int RtTask1::svc()
{
	// do something -- put your code here !!!
	while (1)
	{
		std::cout << "RT 1 -- Hello from RtTask1  - periodic\n";
		smart_task_wait_period();
	}
	return 0;
}