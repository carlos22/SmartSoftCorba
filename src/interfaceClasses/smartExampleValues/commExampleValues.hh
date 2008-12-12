// --------------------------------------------------------------------------
//
//  Copyright (C) 2002 Christian Schlegel
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

//
// This is another example of a communication class where one uses the IDL
// data structure only for communication purposes. Therefore the IDL data
// structure is filled within the mandatory get / set methods.
//
#ifndef _COMM_EXAMPLE_VALUES_HH
#define _COMM_EXAMPLE_VALUES_HH

#include <string>
#include <list>

#include "exampleValuesC.hh"

namespace CHS {

class CommExampleValues
{
protected:
  std::list<int> values;

public:
  CommExampleValues();
  virtual ~CommExampleValues();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::exampleValues";
  };


  //
  // user interface
  //
  void set(std::list<int>);
  void get(std::list<int>&) const;
};

}

#endif

