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

#include "smartProperty.hh"

using namespace Smart;

Property::Key::Key(const std::string &composite_key)
: _subkey(0)
{
  _assign(composite_key);
}

Property::Key::Key(const char *composite_key)
: _subkey(0)
{
  _assign(std::string(composite_key));
}

Property::Key::Key(const Key &key, const Key *subkey)
: _subkey(0)
{
  _assign(key);
  if(subkey)
  {
    Key *iter = this;
    while(iter->_subkey!=0) iter = iter->_subkey;
    iter->_subkey = new Key(*subkey);
  }
}

Property::Key::Key(const std::string &key, const Key &subkey)
: _subkey(0)
{
  _assign(key);
  Key *iter = this;
  while(iter->_subkey!=0) iter = iter->_subkey;
  iter->_subkey = new Key(subkey);
}

Property::Key &Property::Key::operator=(const Key &key)
{
  _assign(key);
  return *this;
}

Property::Key::~Key ()
{
  if(_subkey) delete _subkey;
}

bool Property::Key::operator==(const Key &other) const 
{
  if(_key!=other._key) return false;
  if((_subkey==0) && (other._subkey==0)) return true;
  if((_subkey!=0) && (other._subkey!=0)) return (*_subkey)==(*(other._subkey));
  return false;
}

/// get the full name of this key.
std::string Property::Key::toString() const
{
  std::string composite_key = _key;
  if (_subkey)
  {
    composite_key += ".";
    composite_key += _subkey->toString();
  }
  return composite_key;
}

void Property::Key::_assign(const std::string &composite_key)
{
  if(_subkey) delete _subkey;
  std::string::size_type dotpos = composite_key.find('.');
  if(dotpos != std::string::npos)
  {
    _key = composite_key.substr(0, dotpos);
    _subkey = new Key(composite_key.substr(dotpos + 1));
  }
  else
  {
    _key = composite_key;
    _subkey = 0;
  }
}

void Property::Key::_assign(const Key &key)
{
  if(_subkey) delete _subkey;
  _key = key._key;
  if (key._subkey)
  {
    _subkey = new Key(*(key._subkey));
  }
  else
  {
    _subkey = 0;
  }
}
