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
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------

#ifndef _COMM_COMMSPEECHOBJECTS_COMMSPEECHOUTPUTMESSAGE_HH
#define _COMM_COMMSPEECHOBJECTS_COMMSPEECHOUTPUTMESSAGE_HH

#include <string>

// include files of classes

// include header-files of included communication objects
#include <CommBasicObjects/commTimeStamp.hh>

// include enums


// include client-side file generated by IDL compiler
#include "CommSpeechObjects/gen/smartCommSpeechOutputMessageC.hh"

namespace CommSpeechObjects
{

class CommSpeechOutputMessage
{
protected:
	CommSpeechObjectsIDL::CommSpeechOutputMessage idl_CommSpeechOutputMessage;

public:
	CommSpeechOutputMessage()
	{
	}

	CommSpeechOutputMessage(const CommSpeechObjectsIDL::CommSpeechOutputMessage &obj) :
		idl_CommSpeechOutputMessage(obj)
	{
	}

	operator CommSpeechObjectsIDL::CommSpeechOutputMessage() const
	{
		return idl_CommSpeechOutputMessage;
	}

	virtual ~CommSpeechOutputMessage()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommSpeechObjects::CommSpeechOutputMessage";
	}

	//
	// user interfaces
	  CommSpeechOutputMessage(const std::string &text);


	  /**
	    Get the message text.
	   */
	  inline std::string get_message() const { return (const char*)(idl_CommSpeechOutputMessage.text); }

	  /**
	    Get the creation time.
	   */
	  inline CommBasicObjects::CommTimeStamp get_creation_time() const { return CommBasicObjects::CommTimeStamp(idl_CommSpeechOutputMessage.time); }

	  /**
	    Get time to live of this message in seconds.
	   */
	  inline double get_time_to_live() const { return idl_CommSpeechOutputMessage.ttl; }

	  /**
	    Compute time when message shall be discarded.
	   */
	  inline CommBasicObjects::CommTimeStamp get_discard_time() const { return CommBasicObjects::CommTimeStamp(idl_CommSpeechOutputMessage.time).advance(idl_CommSpeechOutputMessage.ttl); }

	  /**
	    Set the message text.
	   */
	  inline void set_message(const std::string &msg) { idl_CommSpeechOutputMessage.text = msg.c_str(); }

	  /**
	    Set the creation time to now.
	   */
	  void set_creation_time_now();

	  /**
	    Set time to live of this message in seconds.
	   */
	  inline void set_time_to_live(double ttl) { idl_CommSpeechOutputMessage.ttl = ttl; }

	  /**
	    Write information about this timestamp to the given output stream.
	   */
	  void print(std::ostream &os = std::cout) const;

	  /**
	    Save an XML like representation of this time stamp to the given output stream.
	   */
	  void save_xml(std::ostream &os, const std::string &indent = "") const;

	  /**
	    Load from XML represenation on an istream.
	    Be warned, no error checking is done.
	    For details, see \ref smart_basic_com-xml.
	   */
	  void load_xml(std::istream &is);
	};

	//
	// Implementation
	//

	inline std::ostream &operator<<(std::ostream &os, const CommSpeechOutputMessage &msg)
	{
	  msg.print(os);
	  return os;
	}

}
#endif
