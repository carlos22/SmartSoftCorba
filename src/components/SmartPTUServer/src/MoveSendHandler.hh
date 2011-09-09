//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.2
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

#ifndef _MOVESENDHANDLER_HH
#define _MOVESENDHANDLER_HH

#include "smartSoft.hh"

#include "gen/MoveSendHandlerCore.hh"

class MoveSendHandler: public MoveSendHandlerCore
{
	void handleSend(const CommPTUObjects::CommPTUMoveRequest &r) throw();
};

#endif
