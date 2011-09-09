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

#include "ImageTaskTimer.hh"
#include "SmartPMDServer.hh"

#include <iostream>

void ImageTaskTimer::timerExpired(const ACE_Time_Value & absolute_time,
		const void * arg)
{
	// signal condition mutex
	COMP->ImageTaskMutex.acquire();
	COMP->ImageTaskCondMutex.signal();
	COMP->ImageTaskMutex.release();
}
