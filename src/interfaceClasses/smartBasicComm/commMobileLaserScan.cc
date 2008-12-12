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
#include "commMobileLaserScan.hh"
#include "commLaserScan.hh"

using namespace Smart;

CommMobileLaserScan::CommMobileLaserScan()
{
  _scan.laser_scan.is_valid = false;
  _scan.laser_scan.length_unit = 1;
  _scan.laser_scan.update_count = 0;
  _scan.laser_scan.time.sec = 0;
  _scan.laser_scan.time.usec = 0;
  _scan.laser_scan.start_angle = 27000; // -90 deg
  _scan.laser_scan.resolution = 50;     // 0.5 deg
  _scan.scanner_x = 0;
  _scan.scanner_y = 0;
  _scan.scanner_z = 0;
  _scan.scanner_a = 0;
}

CommMobileLaserScan::CommMobileLaserScan(const SmartIDL::MobileLaserScan &scan)
: _scan(scan)
{
}

CommMobileLaserScan::~CommMobileLaserScan()
{
}

void CommMobileLaserScan::get(CORBA::Any &a) const
{
  a <<= _scan;
}

void CommMobileLaserScan::set(const CORBA::Any &a)
{
  SmartIDL::MobileLaserScan *tmp_scan = 0;
  if(a >>= tmp_scan)
  {
    _scan = *tmp_scan;
  }
}

void CommMobileLaserScan::limit_scan_distance_integer(unsigned int max_dist)
{
  SmartIDL::LaserScan tmp_scan = _scan.laser_scan;
  const unsigned int size = tmp_scan.scan_points.length();
  
  unsigned int num_points_to_remove = 0;
  for(unsigned int i=0; i<size; ++i)
  {
    if(tmp_scan.scan_points[i].distance > max_dist) ++num_points_to_remove;
  }

  _scan.laser_scan.scan_points.length(size - num_points_to_remove);
  unsigned int num_points_removed = 0;
  for(unsigned int i=0; i<size; ++i)
  {
    if(tmp_scan.scan_points[i].distance > max_dist)
    {
      ++num_points_removed;
    }
    else
    {
      _scan.laser_scan.scan_points[i - num_points_removed] = tmp_scan.scan_points[i];
    }
  }
}

void CommMobileLaserScan::print(std::ostream &os) const
{
  os << "CommMobileLaserScan(";
  if(is_scan_valid())
  {
    os << "id=" << get_scan_update_count() << ","
       << "size=" << get_scan_size() << ",";
  }
  else
  {
    os << "invalid,";
  }
  os << get_base_state() << "," 
     << "scanner=(" << get_scanner_x() << "," << get_scanner_y() << "," << get_scanner_z() << "," << get_scanner_azimuth() << ")" 
     << ")";
}

void CommMobileLaserScan::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<mobile_laser_scan>" << std::endl;
  get_base_state().save_xml(os, indent + "  ");
  os << indent << "  <scanner_x>" << _scan.scanner_x << "</scanner_x>" << std::endl;
  os << indent << "  <scanner_y>" << _scan.scanner_y << "</scanner_y>" << std::endl;
  os << indent << "  <scanner_z>" << _scan.scanner_z << "</scanner_z>" << std::endl;
  os << indent << "  <scanner_azimuth>" << _scan.scanner_a << "</scanner_azimuth>" << std::endl;
  CommLaserScan(_scan.laser_scan).save_xml(os, indent + "  ");
  os << indent << "</mobile_laser_scan>" << std::endl;
}

void CommMobileLaserScan::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<mobile_laser_scan>");
  static const KnuthMorrisPratt kmp_scanner_x("<scanner_x>");
  static const KnuthMorrisPratt kmp_scanner_y("<scanner_y>");
  static const KnuthMorrisPratt kmp_scanner_z("<scanner_z>");
  static const KnuthMorrisPratt kmp_scanner_azimuth("<scanner_azimuth>");
  static const KnuthMorrisPratt kmp_end("</mobile_laser_scan>");

  kmp_begin.search(is);
  
  CommBaseState bs;
  bs.load_xml(is);
  set_base_state(bs);
  
  kmp_scanner_x.search(is);
  is >> _scan.scanner_x;
  kmp_scanner_y.search(is);
  is >> _scan.scanner_y;
  kmp_scanner_z.search(is);
  is >> _scan.scanner_z;
  kmp_scanner_azimuth.search(is);
  is >> _scan.scanner_a;

  CommLaserScan scan;
  scan.load_xml(is);
  _scan.laser_scan = scan.get_idl();

  kmp_end.search(is);
}
