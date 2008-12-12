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

#include "smartKnuthMorrisPratt.hh"
#include "commPosition3d.hh"

using namespace Smart;

CommPosition3d::CommPosition3d()
{
  _pos.x = 0;
  _pos.y = 0;
  _pos.z = 0;
}

CommPosition3d::CommPosition3d(double x, double y, double z, const double unit)
{
  _pos.x = x * unit * 1000;
  _pos.y = y * unit * 1000;
  _pos.z = z * unit * 1000;
}

CommPosition3d::CommPosition3d(const SmartIDL::Position3d &pos)
: _pos(pos)
{
}

CommPosition3d::~CommPosition3d()
{
}

void CommPosition3d::get(CORBA::Any &a) const
{
  a <<= _pos;
}

void CommPosition3d::set(const CORBA::Any &a)
{
  SmartIDL::Position3d *tmp_pos;
  if(a >>= tmp_pos)
  {
    _pos = *tmp_pos;
  }
}

void CommPosition3d::print(std::ostream &os) const
{
  os << "CommPosition3d(x=" << _pos.x << ",y=" << _pos.y << ",z=" << _pos.z << ")";
}

void CommPosition3d::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<position3d>" << std::endl;
  os << indent << "  <x>" << _pos.x << "</x>" << std::endl;
  os << indent << "  <y>" << _pos.y << "</y>" << std::endl;
  os << indent << "  <z>" << _pos.z << "</z>" << std::endl;
  os << indent << "</position3d>" << std::endl;
}

void CommPosition3d::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<position3d>");
  static const KnuthMorrisPratt kmp_x("<x>");
  static const KnuthMorrisPratt kmp_y("<y>");
  static const KnuthMorrisPratt kmp_z("<z>");
  static const KnuthMorrisPratt kmp_end("</position3d>");

  kmp_begin.search(is);
  kmp_x.search(is);
  is >> _pos.x;
  kmp_y.search(is);
  is >> _pos.y;
  kmp_z.search(is);
  is >> _pos.z;
  kmp_end.search(is);
}
