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
// --------------------------------------------------------------------------

#ifndef SMART_COMM_WIRING_HH
#define SMART_COMM_WIRING_HH

#include "smartCommWiringC.hh"

#include "smartErrno.hh"

#include <string>

namespace CHS {

  class SmartCommWiring
  {
  protected:
    //
    // This is the data structure which is described in the IDL. Therefore
    // CORBA can marshal / unmarshal this type for communication purposes.
    //
    SmartWiring wiring;

  public:
    //
    // constructors, destructors, copy constructors etc. ...
    //
    SmartCommWiring();
    virtual ~SmartCommWiring();

    //
    // The following methods MUST be available in a communication object.
    // This however is not too bad for implementers of a communication
    // object since you can get cookbook like instructions on how
    // to implement these. They are always the same since they have to
    // set / get the above IDL structure. They are used by the communication
    // patterns and should not be used by users.
    //
    void get(CORBA::Any &) const;
    void set(const CORBA::Any &);
    static inline std::string identifier(void) {
      return "CHS::smartWiring";
    };

    //
    // The following methods provide the user interface. They are provided
    // by the developer of the communication object and provide everything
    // needed to work with that object. Those methods do not necessarily
    // return CORBA types but can e.g. convert CORBA sequences of the
    // IDL data structure into STL vectors ...
    //

    void setCommand(const std::string,const std::string,const std::string,const std::string);
    void getCommand(std::string&,std::string&,std::string&,std::string&) const;
    void setStatus(const CHS::StatusCode);
    void getStatus(CHS::StatusCode&) const;
  };

}
#endif

