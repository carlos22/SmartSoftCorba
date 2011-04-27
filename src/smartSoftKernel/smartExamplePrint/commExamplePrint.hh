// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2010 Christian Schlegel
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
//  (partly based on joint work with Robert W�rz)
//
// --------------------------------------------------------------------------

//
//
//
// Example of a communication object whose data structure "Print" is
// described using IDL.
//
//
//

#ifndef _COMM_EXAMPLE_PRINT_HH
#define _COMM_EXAMPLE_PRINT_HH

#include <iostream>
#include <iomanip>
#include <string>

#include "examplePrintC.hh"

namespace Smart {

class CommExamplePrint
{
protected:
  //
  // This is the data structure which is described in the IDL. Therefore
  // CORBA can marshal / unmarshal this type for communication purposes.
  //
  ExamplePrint message;

  //
  // We can add further data structures here but only those data structures
  // described in IDL and being accessible by the get/set-methods are
  // transmitted. Currently I assume that everything to be transmitted
  // is wrapped by a single structure. This is no restriction since
  // IDL interfaces can easily composed of several structures.
  //
public:
  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommExamplePrint();
  virtual ~CommExamplePrint();

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
    return "CHS::examplePrint";
  };


  //
  // The following methods provide the user interface. They are provided
  // by the developer of the communication object and provide everything
  // needed to work with that object. Those methods do not necessarily
  // return CORBA types but can e.g. convert CORBA sequences of the
  // IDL data structure into STL vectors ...
  //

  void get(int&,int&,int&,std::string& text) const;
  void set(int,int,int,std::string);
  void print(std::ostream &os = std::cout) const;
};

}

#endif

