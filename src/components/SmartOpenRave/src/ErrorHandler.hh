//--------------------------------------------------------------------------
//  Copyright (C) 2010 Jonas Brich
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartOpenRave component".
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
//--------------------------------------------------------------------------

#ifndef _ERRORHANDLER_HH_
#define _ERRORHANDLER_HH_

#include <iostream>
#include <string>

#include <commManipulationPlannerEventState.hh>

/**
 * This class can handle messages.
 * It sends the specified message to an output stream depending on the error type.
 *
 * Available Error Types:
 * 	- INFO: 	The message will be displayed on the std::cout stream with INFO: in front.
 * 	- WARNING:	The message will be displayed on the std::cout stream with WARNING: in front.
 * 	- ERROR:	The message will be displayed on the std::cerr stream with ERROR: in front.
 *
 * Furthermore it is possible to send a event over the event port to notify other components.
 *
 * Possible Events can be seen in enumManipulationPlannerEvent
 */
class ErrorHandler {
public:
	enum ErrorTypes {
		INFO, WARNING, ERROR
	};

	/**
	 * This method sends a specified message to an output stream depending on the error type.
	 * Furthermore it is possible to send the message or not.
	 *
	 * message:		string message which will be displayed on the output stream
	 * type:		type of the message, specifies on which output stream the message will be displayed
	 * sendMessage:	true, the message will be displayed, otherwise not
	 */
	void static handleMessage(const std::string& message, ErrorHandler::ErrorTypes type = ErrorHandler::INFO, bool sendMessage = true);

	/**
	 * This method sends a specified message to an output stream depending on the error type.
	 * Furthermore it is possible to send a event and display the message or not.
	 *
	 * message:		string message which will be displayed on the output stream
	 * event:		event which will be sent to other components
	 * type:		type of the message, specifies on which output stream the message will be displayed
	 * sendMessage:	true, the message will be displayed, otherwise not
	 */
	void static handleMessage(const std::string& message, Smart::ManipulationPlannerEvent event, ErrorHandler::ErrorTypes type =
			ErrorHandler::INFO, bool sendMessage = true);
};

#endif /* _ERRORHANDLER_HH_ */
