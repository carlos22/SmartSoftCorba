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
#include "commOrientation3d.hh"

using namespace Smart;

CommOrientation3d::CommOrientation3d()
{
  _ori.azimuth = 0;
  _ori.elevation = 0;
  _ori.roll = 0;
}

CommOrientation3d::CommOrientation3d(double azimuth, double elevation, double roll)
{
  _ori.azimuth = azimuth;
  _ori.elevation = elevation;
  _ori.roll = roll;
}

CommOrientation3d::CommOrientation3d(const SmartIDL::Orientation3d &ori)
: _ori(ori)
{
}

CommOrientation3d::~CommOrientation3d()
{
}

void CommOrientation3d::get(CORBA::Any &a) const
{
  a <<= _ori;
}

void CommOrientation3d::set(const CORBA::Any &a)
{
  SmartIDL::Orientation3d *tmp_ori;
  if(a >>= tmp_ori)
  {
    _ori = *tmp_ori;
  }
}

void CommOrientation3d::print(std::ostream &os) const
{
  os << "CommOrientation3d(azimuth=" << _ori.azimuth << ",elevation=" << _ori.elevation << ",roll=" << _ori.roll << ")";
}

void CommOrientation3d::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<orientation3d>" << std::endl;
  os << indent << "  <azimuth>" << _ori.azimuth << "</azimuth>" << std::endl;
  os << indent << "  <elevation>" << _ori.elevation << "</elevation>" << std::endl;
  os << indent << "  <roll>" << _ori.roll << "</roll>" << std::endl;
  os << indent << "</orientation3d>" << std::endl;
}

void CommOrientation3d::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<orientation3d>");
  static const KnuthMorrisPratt kmp_azimuth("<azimuth>");
  static const KnuthMorrisPratt kmp_elevation("<elevation>");
  static const KnuthMorrisPratt kmp_roll("<roll>");
  static const KnuthMorrisPratt kmp_end("</orientation3d>");

  kmp_begin.search(is);
  kmp_azimuth.search(is);
  is >> _ori.azimuth;
  kmp_elevation.search(is);
  is >> _ori.elevation;
  kmp_roll.search(is);
  is >> _ori.roll;
  kmp_end.search(is);
}
