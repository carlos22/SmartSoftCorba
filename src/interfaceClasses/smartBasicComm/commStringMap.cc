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


#include "commStringMap.hh"

using namespace Smart;

void CommStringMap::get(CORBA::Any &a) const
{
  const unsigned int n = _str_map.size();
  SmartIDL::StringMap str_map_idl;
  str_map_idl.length(n);
  std::map<std::string,std::string>::const_iterator iter = _str_map.begin();
  for(unsigned int i=0; i<n; ++i)
  {
    str_map_idl[i].key   = (*iter).first.c_str();
    str_map_idl[i].value = (*iter).second.c_str();
    ++iter;
  }
  a <<= str_map_idl;
}

void CommStringMap::set(const CORBA::Any &a)
{
  SmartIDL::StringMap *str_map_idl;
  if(a >>= str_map_idl)
  {
    const unsigned int n = str_map_idl->length();
    _str_map.clear();
    for(unsigned int i=0; i<n; ++i)
    {
      const char *key = (*str_map_idl)[i].key;
      const char *value = (*str_map_idl)[i].value;
      _str_map.insert(std::pair<std::string,std::string>(key,value));
    }
  }
}

void CommStringMap::print(std::ostream &os) const
{
  os << "StringMap(";
  std::map<std::string,std::string>::const_iterator iter = _str_map.begin();
  while(iter!=_str_map.end())
  {
    os << "(\"" << (*iter).first << "\"->\"" << (*iter).second << "\")";
    ++iter;
  }
  os << ")";
}

void CommStringMap::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<string_map n=\"" << _str_map.size() << "\">" << std::endl;
  std::map<std::string,std::string>::const_iterator iter = _str_map.begin();
  while(iter!=_str_map.end())
  {
    os << indent << "  <entry key=\"" << (*iter).first << "\">" << (*iter).second << "</entry>" << std::endl;
    ++iter;
  }
  os << indent << "</string_map>" << std::endl;
}
