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
#include "commDirection3d.hh"

using namespace Smart;

CommDirection3d::CommDirection3d()
{
  _dir.azimuth = 0;
  _dir.elevation = 0;
}

CommDirection3d::CommDirection3d(double azimuth, double elevation)
{
  _dir.azimuth = azimuth;
  _dir.elevation = elevation;
}

CommDirection3d::CommDirection3d(const SmartIDL::Direction3d &dir)
: _dir(dir)
{
}

CommDirection3d::~CommDirection3d()
{
}

void CommDirection3d::get(CORBA::Any &a) const
{
  a <<= _dir;
}

void CommDirection3d::set(const CORBA::Any &a)
{
  SmartIDL::Direction3d *tmp_dir;
  if(a >>= tmp_dir)
  {
    _dir = *tmp_dir;
  }
}

void CommDirection3d::print(std::ostream &os) const
{
  os << "CommDirection3d(azimuth=" << _dir.azimuth << ",elevation=" << _dir.elevation << ")";
}

void CommDirection3d::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<direction3d>" << std::endl;
  os << indent << "  <azimuth>" << _dir.azimuth << "</azimuth>" << std::endl;
  os << indent << "  <elevation>" << _dir.elevation << "</elevation>" << std::endl;
  os << indent << "</direction3d>" << std::endl;
}

void CommDirection3d::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<direction3d>");
  static const KnuthMorrisPratt kmp_azimuth("<azimuth>");
  static const KnuthMorrisPratt kmp_elevation("<elevation>");
  static const KnuthMorrisPratt kmp_end("</direction3d>");

  kmp_begin.search(is);
  kmp_azimuth.search(is);
  is >> _dir.azimuth;
  kmp_elevation.search(is);
  is >> _dir.elevation;
  kmp_end.search(is);
}
