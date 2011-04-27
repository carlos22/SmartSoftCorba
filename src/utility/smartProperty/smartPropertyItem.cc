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

#include "smartPropertyItem.hh"

using namespace Smart;

PropertyItem::PropertyItem()
: _value() 
{
}

PropertyItem *PropertyItem::clone() const
{
  return new PropertyItem(_value);
}

bool PropertyItem::operator==(const PropertyItem & other) const
{
  return (_value == other._value);
}

void PropertyItem::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<property-item>" << std::endl;
  os << indent << "  " << _value << std::endl;
  os << indent << "</property-item>" << std::endl;
}

void PropertyItem::print(std::ostream &os, const std::string &indent) const
{
  os << indent << "PropertyItem { " << _value << " }" << std::endl;
}

int PropertyItem::getInteger() const throw(ConversionFailed)
{
  const char *start = _value.c_str();
  char *end = 0;
  const int i = strtol(start, &end, 0);
  if (start == end) 
  {
    throw ConversionFailed(_value + ": not an integer");
  }
  return i;
}

double PropertyItem::getDouble() const throw(ConversionFailed)
{
  const char *start = _value.c_str();
  char *end = 0;
  const double d = strtod(start, &end);
  if (start == end) 
  {
    throw ConversionFailed(_value + ": not a double");
  }
  return d;
}

bool PropertyItem::getBool() const throw(ConversionFailed)
{
  std::istringstream iss(_value);
  std::ostringstream oss;
  iss >> std::ws; // skip whitespace
  char ch;
  while(!iss.get(ch).eof())
  {
    if(isspace(ch)) break;
    oss.put(tolower(ch));
  }
  const std::string value = oss.str();
  if(value=="yes" || value=="true" || value=="y" || value =="t" || value=="+" || value=="1" ) 
  {
    return true;
  } 
  else if (value=="no" || value=="false" || value=="n" || value=="f" || value=="-" ||  value=="0") 
  {
    return false;
  } 
  else 
  {
    throw ConversionFailed(value + ": not recognized as boolean");
  }
  return false;
}

void PropertyItem::setString(const std::string & str)
{
  if (str != _value) 
  {
    // only notify if the value has actually changed.
    _value = str;
    this->_notify_property_changed();
  }
}

void PropertyItem::setInteger(int i)
{
  std::ostringstream oss;
  oss << i;
  setString(oss.str());
}

void PropertyItem::setDouble(double d)
{
  std::ostringstream oss;
  oss << d;
  setString(oss.str());
}

void PropertyItem::setBool(bool b)
{
  if(b)
  {
    setString("true");
  }
  else
  {
    setString("false");
  }
}
