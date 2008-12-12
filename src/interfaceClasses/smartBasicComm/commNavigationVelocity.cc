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
#include "commNavigationVelocity.hh"

using namespace Smart;

CommNavigationVelocity::CommNavigationVelocity()
{
  _vel.v = 0;
  _vel.omega = 0;
}

CommNavigationVelocity::CommNavigationVelocity(const SmartIDL::NavigationVelocity &vel)
: _vel(vel)
{
}

void CommNavigationVelocity::get(CORBA::Any &a) const
{
  a <<= _vel;
}

void CommNavigationVelocity::set(const CORBA::Any &a)
{
  SmartIDL::NavigationVelocity *tmp_vel;
  if(a >>= tmp_vel)
  {
    _vel = *tmp_vel;
  }
  else
  {
    _vel.v = 0;
    _vel.omega = 0;
  }
}

void CommNavigationVelocity::print(std::ostream &os) const
{
  os << "CommNavigationVelocity  (v=" << _vel.v << ",omega=" << _vel.omega << ")";
}

void CommNavigationVelocity::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<navigation_velocity>" << std::endl;
  os << indent << "  <v>" << _vel.v << "</v>" << std::endl;
  os << indent << "  <omega>" << _vel.omega << "</omega>" << std::endl;
  os << indent << "</navigation_velocity>" << std::endl;
}

void CommNavigationVelocity::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<navigation_velocity>");
  static const KnuthMorrisPratt kmp_v("<v>");
  static const KnuthMorrisPratt kmp_omega("<omega>");
  static const KnuthMorrisPratt kmp_end("</navigation_velocity>");

  kmp_begin.search(is);
  kmp_v.search(is);
  is >> _vel.v;
  kmp_omega.search(is);
  is >> _vel.omega;
  kmp_end.search(is);
}
