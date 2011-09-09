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

#ifndef _PARAMHANDLERCORE_HH
#define _PARAMHANDLERCORE_HH

#include "smartSoft.hh"

// include communication objects
#include <CommSpeechObjects/commSpeechInputEventParameter.hh>

#include <CommSpeechObjects/commSpeechInputEventResult.hh>

#include <CommSpeechObjects/commSpeechInputParameter.hh>

#include <CommSpeechObjects/speechInputEventState.hh>

class ParamHandlerCore: public CHS::SendServerHandler<
		CommSpeechObjects::CommSpeechInputParameter>
{
public:
	void handleSend(const CommSpeechObjects::CommSpeechInputParameter &r) throw()
	{
	}
	;
};
#endif
