//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.3
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

#ifndef _PERSONDETECTIONTASK_HH
#define _PERSONDETECTIONTASK_HH

#include "gen/PersonDetectionTaskCore.hh"

#include "PersonDetectionVisualization.hh"

class PersonDetectionTask: public PersonDetectionTaskCore
{

private:
	PersonDetectionVisualization* personDetection;

public:
	PersonDetectionTask();
	void init();
	int svc();
};

#endif
