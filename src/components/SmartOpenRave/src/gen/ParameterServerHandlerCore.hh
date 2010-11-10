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
// Please do not modify this file. It will be re-generated
// running the code generator.
//--------------------------------------------------------------------------

#ifndef _PARAMETERSERVERHANDLERCORE_HH
#define _PARAMETERSERVERHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <commManipulationPlannerEventParameter.hh>

#include <commManipulationPlannerEventResult.hh>

#include <commManipulationPlannerEventState.hh>

#include <commManipulationPlannerParameter.hh>

#include <commManipulationTaskPlanning.hh>

#include <commManipulatorEventParameter.hh>

#include <commManipulatorEventResult.hh>

#include <commManipulatorTrajectory.hh>

#include <commMobileManipulatorState.hh>

#include <commObjectRecognitionEnvironment.hh>

#include <commObjectRecognitionId.hh>

#include <commObjectRecognitionObjectFeatures.hh>

class ParameterServerHandlerCore: public CHS::SendServerHandler<
		Smart::CommManipulationPlannerParameter>
{
public:
	void handleSend(const Smart::CommManipulationPlannerParameter &r) throw()
	{
	}
	;
};
#endif