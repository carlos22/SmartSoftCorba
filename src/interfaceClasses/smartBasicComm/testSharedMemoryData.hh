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


#ifndef TEST_SHARED_MEMORY_DATA_HH
#define TEST_SHARED_MEMORY_DATA_HH

#include <iostream>

namespace Smart {

struct ShmData
{
  // Don't use dynamic data structures in objects 
  // that are communicated through shared memory!

  char name[200];
  double size;
  int age;

  char lots_of_data[16*1024*1024];

  static inline std::string identifier() { return "ShmData"; }
};

inline std::ostream &operator<<(std::ostream &os, const ShmData &data)
{
  return os << "ShmData(name=" << data.name << ",size=" << data.size << ",age=" << data.age << ")";
}

} // namespace Smart

#endif
