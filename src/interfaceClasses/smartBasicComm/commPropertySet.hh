// --------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo, Boris Kluge
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
//
// --------------------------------------------------------------------------

#ifndef SMART_COMM_PROPERTY_SET_HH
#define SMART_COMM_PROPERTY_SET_HH

#include "smartPropertySet.hh"
#include "smartPropertyItem.hh"
#include "smartPropertySetC.hh"

namespace Smart {

/** A transmittable property set
 *
 */
class CommPropertySet: public PropertySet
{
public:

  /** ctor */
  inline CommPropertySet()
  : PropertySet()
  {
  }

  inline CommPropertySet(const PropertySet &source)
  : PropertySet(source)
  {
  }

  inline CommPropertySet &operator=(const PropertySet &source)
  {
    PropertySet::operator=(source);
    return *this;
  }

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);

  template<class Value>
  inline void set(const Key &key, const Value &value)
  {
    PropertySet::set(key,value);
  }

  static inline std::string identifier()
  {
    return "Smart::CommPropertySet";
  }
};

} // namespace Smart

#endif
