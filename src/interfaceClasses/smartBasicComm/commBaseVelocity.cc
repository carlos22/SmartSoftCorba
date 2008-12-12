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
#include "commBaseVelocity.hh"

using namespace Smart;

CommBaseVelocity::CommBaseVelocity()
{
  _vel.v = 0;
  _vel.omega_base = 0;
  _vel.omega_steer = 0;
  _vel.omega_turret = 0;
}

CommBaseVelocity::CommBaseVelocity(const SmartIDL::BaseVelocity &vel)
: _vel(vel)
{
}

CommBaseVelocity::~CommBaseVelocity()
{
}

void CommBaseVelocity::get(CORBA::Any &a) const
{
  a <<= _vel;
}

void CommBaseVelocity::set(const CORBA::Any &a)
{
  SmartIDL::BaseVelocity *tmp_vel;
  if(a >>= tmp_vel)
  {
    _vel = *tmp_vel;
  }
}

void CommBaseVelocity::print(std::ostream &os) const
{
  os << "CommBaseVelocity(v=" << _vel.v << ",omega_base=" << _vel.omega_base << ",omega_steer=" << _vel.omega_steer << ",omega_turret=" << _vel.omega_turret << ")";
}

void CommBaseVelocity::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<base_velocity>" << std::endl;
  os << indent << "  <v>" << _vel.v << "</v>" << std::endl;
  os << indent << "  <omega_base>" << _vel.omega_base << "</omega_base>" << std::endl;
  os << indent << "  <omega_steer>" << _vel.omega_steer << "</omega_steer>" << std::endl;
  os << indent << "  <omega_turret>" << _vel.omega_turret << "</omega_turret>" << std::endl;
  os << indent << "</base_velocity>" << std::endl;
}

void CommBaseVelocity::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<base_velocity>");
  static const KnuthMorrisPratt kmp_v("<v>");
  static const KnuthMorrisPratt kmp_omega_base("<omega_base>");
  static const KnuthMorrisPratt kmp_omega_steer("<omega_steer>");
  static const KnuthMorrisPratt kmp_omega_turret("<omega_turret>");
  static const KnuthMorrisPratt kmp_end("</base_velocity>");

  kmp_begin.search(is);
  kmp_v.search(is);
  is >> _vel.v;
  kmp_omega_base.search(is);
  is >> _vel.omega_base;
  kmp_omega_steer.search(is);
  is >> _vel.omega_steer;
  kmp_omega_turret.search(is);
  is >> _vel.omega_turret;
  kmp_end.search(is);
}
