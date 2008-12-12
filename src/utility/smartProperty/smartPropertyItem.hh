//--------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#ifndef SMART_PROPERTY_ITEM_HH
#define SMART_PROPERTY_ITEM_HH

#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "smartProperty.hh"

namespace Smart {

/** This class holds the Property with values.
 *
 *  It can only be a leaf in the property tree.
 */
class PropertyItem : public Property
{
public:
  PropertyItem();
  
  template<class Value>
  inline PropertyItem(const Value &value);

  template<class Value>
  inline PropertyItem &operator=(const Value &source);

  virtual PropertyItem *clone() const;

  virtual unsigned int num_sets() const { return 0; }
  virtual unsigned int num_items() const { return 1; }

  /** compare operator.
   *
   *  checks only value, the listeners are ignored
   */
  bool operator==(const PropertyItem &other) const;
  inline bool operator!=(const PropertyItem &other) const
  {
    return !this->operator==(other);
  }

  virtual void save_xml(std::ostream &os, const std::string &indent = "") const;

  virtual void print(std::ostream &os = std::cout, const std::string &indent = "") const;

  // methods to access the value
  // they can throw the InvalidOperation or ConversionFailed exception

  /** return value as a string */
  inline const std::string &getString() const throw()
  {
    return _value;
  }

  inline bool getString(std::string &s) const throw()
  {
    s = _value;
    return true;
  }

  /** Return value as a int.
   *  Decimal, hexadecimal, and octal representation are accepted through
   *  their common encodings (e.g. "25", "031", and "0x19" are equal).
   *  @throw ConversionFailed if value can't be converted to an integer
   */
  int getInteger() const throw(ConversionFailed);

  template<class Integer>
  inline bool getInteger(Integer &i) const throw()
  {
    try {
      i = this->getInteger();
      return true;
    } catch(ConversionFailed) {}
    return false;
  }

  /** return value as a double.
   *  @throw ConversionFailed if value can't be converted to double
   */
  double getDouble() const throw(ConversionFailed);

  template<class Double>
  inline bool getDouble(Double &d) const throw()
  {
    try {
      d = this->getDouble();
      return true;
    } catch(ConversionFailed) {}
    return false;
  }

  /** return value as a bool.
   *
   *  Truth values can be defined in many different formats:
   *    - true  :  yes | y | true  | t | + | 1
   *    - false :  no  | n | false | f | - | 0
   *
   *  @throw ConversionFailed if value can't be converted to a bool
   */
  bool getBool() const throw(ConversionFailed);

  inline bool getBool(bool &b) const throw()
  {
    try {
      b = this->getBool();
      return true;
    } catch(ConversionFailed) {}
    return false;
  }

  void setString(const std::string & str);

  void setInteger(int i);

  void setDouble(double d);

  void setBool(bool b);

private:
  std::string _value;
};

template<>
inline PropertyItem::PropertyItem(const bool &source)
{
  this->setBool(source);
}

template<>
inline PropertyItem::PropertyItem(const PropertyItem &source)
{
  _value = source._value;
}

template<class Value>
inline PropertyItem::PropertyItem(const Value &value)
{
  std::ostringstream oss;
  oss << value;
  _value = oss.str();
}

template<>
inline PropertyItem &PropertyItem::operator=(const bool &source)
{
  this->setBool(source);
  return *this;
}

template<>
inline PropertyItem &PropertyItem::operator=(const PropertyItem &source)
{
  this->setString(source._value);
  return *this;
}

template<class Value>
inline PropertyItem &PropertyItem::operator=(const Value &value)
{
  std::ostringstream oss;
  oss << value;
  this->setString(oss.str());
  return *this;
}

} // namespace Smart

#endif
