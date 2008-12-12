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

#include <iostream>
#include <stdexcept>

#include "smartErrno.hh"

namespace CHS {

std::string StatusCodeConversion(StatusCode code)
{
  std::string r;

  if ((code >= SMART_OK) && (code < SMART_STATUS)) {
    switch (code) {
      default:
        r = "STATUS: SMART_OK";
        break;
    }
  } else if ((code >= SMART_STATUS) && (code < SMART_ERROR)) {
    switch (code) {
      case SMART_NODATA:
        r = "STATUS: NODATA";
        break;
      case SMART_CANCELLED:
        r = "STATUS: CANCELLED";
        break;
      case SMART_UNSUBSCRIBED:
        r = "STATUS: UNSUBSCRIBED";
        break;
      case SMART_WRONGID:
        r = "STATUS: WRONGID";
        break;
      case SMART_UNKNOWNSTATE:
        r = "STATUS: UNKNOWNSTATE";
        break;
      case SMART_NOTACTIVATED:
        r = "STATUS: NOTACTIVATED";
        break;
      case SMART_NOTALLOWED:
        r = "STATUS: NOTALLOWED";
        break;
      case SMART_ACTIVE:
        r = "STATUS: ACTIVE";
        break;
      case SMART_PASSIVE:
        r = "STATUS: PASSIVE";
        break;
      case SMART_DISCONNECTED:
        r = "STATUS: DISCONNECTED";
        break;
      case SMART_INCOMPATIBLESERVICE:
        r = "STATUS: INCOMPATIBLESERVICE";
        break;
      case SMART_PORTALREADYUSED:
        r = "STATUS: PORTALREADYUSED";
        break;
      case SMART_UNKNOWNPORT:
        r = "STATUS: UNKNOWNPORT";
        break;
      case SMART_SERVICEUNAVAILABLE:
        r = "STATUS: SERVICEUNAVAILABLE";
        break;
      case SMART_UNKNOWNCOMPONENT:
        r = "STATUS: UNKNOWNCOMPONENT";
        break;
      default:
        r = "STATUS: unknown status code";
        break;
    }
  } else if (code >= SMART_ERROR) {
    switch (code) {
      case SMART_ERROR_COMMUNICATION:
        r = "ERROR: COMMUNICATION";
        break;
      case SMART_ERROR_UNKNOWN:
        r = "ERROR: UNKNOWN";
        break;
      case SMART_ERROR_FATAL:
        r = "ERROR: FATAL";
        break;
      default:
        r = "ERROR: unknown error code";
        break;
    }
  } else {
    r = "unknown status / error code";
  }

  return r;
};

  std::ostream & operator<<(std::ostream & o, const std::exception & e)
  {
    return o << e.what();
  }

  
}

