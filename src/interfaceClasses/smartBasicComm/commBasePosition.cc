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

#include "commBasePosition.hh"

using namespace Smart;

CommBasePosition::CommBasePosition()
{
  _pos.x = 0;
  _pos.y = 0;
  _pos.z = 0;
  _pos.a_base = 0;
  _pos.a_steer = 0;
  _pos.a_turret = 0;
  for(unsigned int i=0; i<3; ++i)
  for(unsigned int j=0; j<3; ++j)
  {
    _pos.m[i*3+j] = (i==j)?1:0;
  }
  _pos.update_count = 0;
}

CommBasePosition::CommBasePosition(const SmartIDL::BasePosition &pos)
: _pos(pos)
{
}

CommBasePosition::~CommBasePosition()
{
}

void CommBasePosition::get(CORBA::Any &a) const
{
  a <<= _pos;
}

void CommBasePosition::set(const CORBA::Any &a)
{
  SmartIDL::BasePosition *tmp_pos;
  if(a >>= tmp_pos)
  {
    _pos = *tmp_pos;
  }
}

void CommBasePosition::print(std::ostream &os) const
{
  // FIXME: check this
  os << "CommBasePosition(p=(x=" << _pos.x << ",y=" << _pos.y << ",z=" << _pos.z 
     << ",base_alpha=" << _pos.a_base << ",steer_alpha=" << _pos.a_steer << ",turret_alpha=" << _pos.a_turret << "),"
     << "m=((" << _pos.m[0] << "," << _pos.m[1] << "," << _pos.m[2] << "),"
     <<    "(" << _pos.m[3] << "," << _pos.m[4] << "," << _pos.m[5] << "),"
     <<    "(" << _pos.m[6] << "," << _pos.m[7] << "," << _pos.m[8] << ")),"
     << "upd_cnt=" << _pos.update_count << ")";
}

void CommBasePosition::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<base_position>" << std::endl;
  os << indent << "  <x>" << _pos.x << "</x>" << std::endl;
  os << indent << "  <y>" << _pos.y << "</y>" << std::endl;
  os << indent << "  <z>" << _pos.z << "</z>" << std::endl;
  os << indent << "  <alpha_base>" << _pos.a_base << "</alpha_base>" << std::endl;
  os << indent << "  <alpha_steer>" << _pos.a_steer << "</alpha_steer>" << std::endl;
  os << indent << "  <alpha_turret>" << _pos.a_turret << "</alpha_turret>" << std::endl;
  os << indent << "  <update_count>" << _pos.update_count << "</update_count>" << std::endl;
  for(unsigned int i=0; i<9; ++i)
  {
    // FIXME: check this
    os << indent << "  <cov i=\"" << i%3 << "\" j=\"" << i/3 << "\">" << _pos.m[i] << "</cov>" << std::endl;
  }
  os << indent << "</base_position>" << std::endl;
}

void CommBasePosition::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<base_position>");
  static const KnuthMorrisPratt kmp_x("<x>");
  static const KnuthMorrisPratt kmp_y("<y>");
  static const KnuthMorrisPratt kmp_z("<z>");
  static const KnuthMorrisPratt kmp_alpha_base("<alpha_base>");
  static const KnuthMorrisPratt kmp_alpha_steer("<alpha_steer>");
  static const KnuthMorrisPratt kmp_alpha_turret("<alpha_turret>");
  static const KnuthMorrisPratt kmp_update_count("<update_count>");
  static const KnuthMorrisPratt kmp_cov("\">");
  static const KnuthMorrisPratt kmp_end("</base_position>");

  kmp_begin.search(is);
  kmp_x.search(is);
  is >> _pos.x;
  kmp_y.search(is);
  is >> _pos.y;
  kmp_z.search(is);
  is >> _pos.z;
  kmp_alpha_base.search(is);
  is >> _pos.a_base;
  kmp_alpha_steer.search(is);
  is >> _pos.a_steer;
  kmp_alpha_turret.search(is);
  is >> _pos.a_turret;
  kmp_update_count.search(is);
  is >> _pos.update_count;
  for(unsigned int i=0; i<9; ++i)
  {
    kmp_cov.search(is);
    is >> _pos.m[i];
  }
  kmp_end.search(is);
}
