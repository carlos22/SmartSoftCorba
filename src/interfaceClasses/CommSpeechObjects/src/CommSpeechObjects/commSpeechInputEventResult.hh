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

// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Andreas Steck, Matthias Lutz
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

#ifndef _COMM_COMMSPEECHOBJECTS_COMMSPEECHINPUTEVENTRESULT_HH
#define _COMM_COMMSPEECHOBJECTS_COMMSPEECHINPUTEVENTRESULT_HH

#include <string>

// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommSpeechObjects/gen/smartCommSpeechInputEventResultC.hh"

namespace CommSpeechObjects
{

class CommSpeechInputEventResult
{
protected:
	CommSpeechObjectsIDL::CommSpeechInputEventResult
			idl_CommSpeechInputEventResult;

public:
	CommSpeechInputEventResult()
	{
		idl_CommSpeechInputEventResult.text = "";
		idl_CommSpeechInputEventResult.confidence = 0.0;
		idl_CommSpeechInputEventResult.semantic = "";
	}

	CommSpeechInputEventResult(
			const CommSpeechObjectsIDL::CommSpeechInputEventResult &obj) :
		idl_CommSpeechInputEventResult(obj)
	{
	}

	operator CommSpeechObjectsIDL::CommSpeechInputEventResult() const
	{
		return idl_CommSpeechInputEventResult;
	}

	virtual ~CommSpeechInputEventResult()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommSpeechObjects::CommSpeechInputEventResult";
	}

	//
	// user interfaces
	  void set_text(std::string);
	  std::string get_text() const;

	  void set_semantic(std::string);
	  std::string get_semantic() const;

	  void set_confidence(double);
	  double get_confidence() const;

};

}
#endif
