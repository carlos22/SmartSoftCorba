// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004 Christian Schlegel
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
//
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#ifndef _SMARTERRNO_HH
#define _SMARTERRNO_HH

#include <string>
#include <stdexcept>
#include <iostream>


/////////////////////////////////////////////////////////////////////////
//
// some standard error codes
//
/////////////////////////////////////////////////////////////////////////

namespace CHS {

  /** Status Code
   *
   *  SmartSoft so far doesn't use exceptions generally, only in constructors.
   *  It uses return values for status & error handling of methods.
   *
   */
  enum StatusCode {
    SMART_OK              = 0,     ///< Operation completed succesfully

    SMART_STATUS          = 256,
    SMART_NODATA,
    SMART_CANCELLED,
    SMART_UNSUBSCRIBED,
    SMART_WRONGID,
    SMART_UNKNOWNSTATE,
    SMART_NOTACTIVATED,
    SMART_ACTIVATED,
    SMART_NOTALLOWED,
    SMART_ACTIVE,
    SMART_PASSIVE,
    SMART_DISCONNECTED,
    SMART_INCOMPATIBLESERVICE,
    SMART_PORTALREADYUSED,
    SMART_UNKNOWNPORT,
    SMART_SERVICEUNAVAILABLE,
    SMART_UNKNOWNCOMPONENT,

    SMART_ERROR           = 512,   ///< values >= SMART_ERROR indicate errors
    SMART_ERROR_COMMUNICATION,
    SMART_ERROR_UNKNOWN,
    SMART_ERROR_FATAL
  };

  /** global function used to convert status code into ASCII representation.
   *
   *  @param code SMART_ERROR_CODE
   */
  std::string StatusCodeConversion(StatusCode code);

  //
  // exceptions
  //

  /** allows printing exceptions based on std::exception */
  std::ostream & operator<<(std::ostream & o, const std::exception & e);


  /** Exception class to describe errors in constructors of communication patterns.
   */
  class SmartError : public std::invalid_argument{
    public:
      StatusCode status;

      SmartError(const CHS::StatusCode s,const std::string & reason) : std::invalid_argument(reason),status(s) {};
  };

}

#endif    // ifndef _SMARTERRNO_HH

