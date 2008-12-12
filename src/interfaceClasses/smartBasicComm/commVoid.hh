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

#ifndef SMART_COMM_VOID_HH
#define SMART_COMM_VOID_HH

#include <string>
#include <iostream>

#include <tao/corba.h>

namespace Smart {

/**
  Void class for queries with empty request.
 */
class CommVoid
{
public:
  virtual ~CommVoid() {}

  inline void get(CORBA::Any&) const {}
  inline void set(const CORBA::Any&) {}
  static inline std::string identifier() { return "Smart::CommVoid"; };

  /**
    Write void information to the given output stream.
   */
  inline void print(std::ostream &os = std::cout) const { os << "CommVoid()"; }

  /**
    Save an XML like representation of void to the given output stream.
    Emtpy dummy.
   */
  inline void save_xml(std::ostream&, const std::string& = "") const {}

  /**
    Load from XML represenation on an istream.
    Empty dummy.
   */
  inline void load_xml(std::istream&) {}
};

inline std::ostream &operator<<(std::ostream &os, const CommVoid &d)
{
  d.print(os);
  return os;
}

} // namespace Smart

#endif
