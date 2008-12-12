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
#include "commTurretVelocity.hh"

using namespace Smart;

CommTurretVelocity::CommTurretVelocity()
{
  _vel.omega = 0;
}

CommTurretVelocity::CommTurretVelocity(const SmartIDL::TurretVelocity &vel)
: _vel(vel)
{
}

CommTurretVelocity::~CommTurretVelocity()
{
}

void CommTurretVelocity::get(CORBA::Any &a) const
{
  a <<= _vel;
}

void CommTurretVelocity::set(const CORBA::Any &a)
{
  SmartIDL::TurretVelocity *tmp_vel;
  if(a >>= tmp_vel)
  {
    _vel = *tmp_vel;
  }
}

void CommTurretVelocity::print(std::ostream &os) const
{
  os << "CommTurretVelocity(omega=" << _vel.omega << ")";
}

void CommTurretVelocity::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<turret_velocity>" << std::endl;
  os << indent << "  <omega>" << _vel.omega << "</omega>" << std::endl;
  os << indent << "</turret_velocity>" << std::endl;
}

void CommTurretVelocity::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<turret_velocity>");
  static const KnuthMorrisPratt kmp_omega("<omega>");
  static const KnuthMorrisPratt kmp_end("</turret_velocity>");

  kmp_begin.search(is);
  kmp_omega.search(is);
  is >> _vel.omega;
  kmp_end.search(is);
}
