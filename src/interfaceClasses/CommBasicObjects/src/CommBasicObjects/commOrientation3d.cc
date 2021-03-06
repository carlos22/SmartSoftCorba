//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

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

#include "commOrientation3d.hh"

using namespace CommBasicObjects;

void CommOrientation3d::get(CORBA::Any &a) const
{
	a <<= idl_CommOrientation3d;
}

void CommOrientation3d::set(const CORBA::Any &a)
{
	CommBasicObjectsIDL::CommOrientation3d *tmp_cmd;
	if (a >>= tmp_cmd)
	{
		idl_CommOrientation3d = *tmp_cmd;
	}
}



CommOrientation3d::CommOrientation3d()
{
	idl_CommOrientation3d.azimuth = 0;
	idl_CommOrientation3d.elevation = 0;
	idl_CommOrientation3d.roll = 0;
}

CommOrientation3d::CommOrientation3d(double azimuth, double elevation, double roll)
{
	idl_CommOrientation3d.azimuth = azimuth;
	idl_CommOrientation3d.elevation = elevation;
	idl_CommOrientation3d.roll = roll;
}

CommOrientation3d::~CommOrientation3d()
{
}

void CommOrientation3d::print(std::ostream &os) const
{
  os << "CommOrientation3d(azimuth=" << idl_CommOrientation3d.azimuth << ",elevation=" << idl_CommOrientation3d.elevation << ",roll=" << idl_CommOrientation3d.roll << ")";
}

void CommOrientation3d::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<orientation3d>" << std::endl;
  os << indent << "  <azimuth>" << idl_CommOrientation3d.azimuth << "</azimuth>" << std::endl;
  os << indent << "  <elevation>" << idl_CommOrientation3d.elevation << "</elevation>" << std::endl;
  os << indent << "  <roll>" << idl_CommOrientation3d.roll << "</roll>" << std::endl;
  os << indent << "</orientation3d>" << std::endl;
}

void CommOrientation3d::load_xml(std::istream &is)
{
  static const Smart::KnuthMorrisPratt kmp_begin("<orientation3d>");
  static const Smart::KnuthMorrisPratt kmp_azimuth("<azimuth>");
  static const Smart::KnuthMorrisPratt kmp_elevation("<elevation>");
  static const Smart::KnuthMorrisPratt kmp_roll("<roll>");
  static const Smart::KnuthMorrisPratt kmp_end("</orientation3d>");

  kmp_begin.search(is);
  kmp_azimuth.search(is);
  is >> idl_CommOrientation3d.azimuth;
  kmp_elevation.search(is);
  is >> idl_CommOrientation3d.elevation;
  kmp_roll.search(is);
  is >> idl_CommOrientation3d.roll;
  kmp_end.search(is);
}
