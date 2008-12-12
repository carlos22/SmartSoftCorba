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

#ifndef SMART_COMM_STRING_MAP_HH
#define SMART_COMM_STRING_MAP_HH

#include <string>
#include <map>
#include <iostream>

#include "smartStringMapC.hh"

namespace Smart {

/**
  A set of key/value string pairs usable for communication among components.
 */
class CommStringMap
{
protected:
  std::map<std::string,std::string> _str_map;

public:

  CommStringMap() : _str_map() {}

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommStringMap"; };

  /**
    Set a key / value pair.
   */
  inline void set(const std::string &key, const std::string &value)
  {
    _str_map[key] = value;
  }

  /**
    Retrieve the value associated with the given key.
    If there is a value for the given key, the value
    is assigned to the \a value parameter, and the method returns \c true.
    Otherwise, \c false is returned, leaving the \a value parameter untouched.
   */
  inline bool get(const std::string &key, std::string &value) const
  {
    std::map<std::string,std::string>::const_iterator iter = _str_map.find(key);
    if(iter!=_str_map.end())
    {
      value = (*iter).second;
      return true;
    }
    return false;
  }

  /**
    Clear the map. 
   */
  inline void clear()
  {
    _str_map.clear();
  }

  /**
    Write information about this string to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this string to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;
};

//
// Implementation
//


inline std::ostream &operator<<(std::ostream &os, const CommStringMap &str_map)
{
  str_map.print(os);
  return os;
}

} // namespace Smart

#endif
