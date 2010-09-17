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
#include "commLaserScan.hh"
#include <limits.h>

using namespace Smart;

CommLaserScan::CommLaserScan()
{
  _scan.is_valid = false;
  _scan.length_unit = 1;
  _scan.update_count = 0;
  _scan.time.sec = 0;
  _scan.time.usec = 0;
  _scan.start_angle = 27000; // -90 deg
  _scan.resolution = 50;     // 0.5 deg
  _scan.distance_min = 0;
  _scan.distance_max = USHRT_MAX;
}

CommLaserScan::CommLaserScan(const SmartIDL::LaserScan &scan)
: _scan(scan)
{
}

CommLaserScan::~CommLaserScan()
{
}

void CommLaserScan::get(CORBA::Any &a) const
{
  a <<= _scan;
}

void CommLaserScan::set(const CORBA::Any &a)
{
  SmartIDL::LaserScan *tmp_scan = 0;
  if(a >>= tmp_scan)
  {
    _scan = *tmp_scan;
  }
}

void CommLaserScan::print(std::ostream &os) const
{
  os << "CommLaserScan(";
  if(is_scan_valid())
  {
    os << "id=" << get_scan_update_count() << ","
       << "size=" << get_scan_size() << ",";
  }
  else
  {
    os << "invalid";
  }
  os << ")";
}

void CommLaserScan::save_xml(std::ostream &os, const std::string &indent) const
{
  const unsigned int n = get_scan_size();
  os << indent << "<laser_scan>" << std::endl;
  os << indent << "  <valid>" << int(_scan.is_valid) << "</valid>" << std::endl;
  get_scan_time_stamp().save_xml(os, indent + "  ");
  os << indent << "  <update_count>" << _scan.update_count << "</update_count>" << std::endl;
  os << indent << "  <start_angle>" << _scan.start_angle << "</start_angle>" << std::endl;
  os << indent << "  <resolution>" << _scan.resolution << "</resolution>" << std::endl;
  os << indent << "  <length_unit>" << _scan.length_unit << "</length_unit>" << std::endl;
  os << indent << "  <distance_min>" << _scan.distance_min << "</distance_min>" << std::endl;
  os << indent << "  <distance_max>" << _scan.distance_max << "</distance_max>" << std::endl;
  os << indent << "  <points n=\"" << n << "\">" << std::endl;
  for(unsigned int i=0; i<n; ++i)
  {
    os << indent << "    <point index=\"" << _scan.scan_points[i].index << "\" distance=\"" << _scan.scan_points[i].distance << "\" intensity=\"" << int(_scan.scan_points[i].intensity) << "\"/>" << std::endl;
  }
  os << indent << "  </points>" << std::endl;
  os << indent << "</laser_scan>" << std::endl;
}

void CommLaserScan::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<laser_scan>");
  static const KnuthMorrisPratt kmp_valid("<valid>");
  static const KnuthMorrisPratt kmp_update_count("<update_count>");
  static const KnuthMorrisPratt kmp_start_angle("<start_angle>");
  static const KnuthMorrisPratt kmp_resolution("<resolution>");
  static const KnuthMorrisPratt kmp_length_unit("<length_unit>");
  static const KnuthMorrisPratt kmp_distance_min("<distance_min>");
  static const KnuthMorrisPratt kmp_distance_max("<distance_max>");
  static const KnuthMorrisPratt kmp_points("<points n=\"");
  static const KnuthMorrisPratt kmp_index("<point index=\"");
  static const KnuthMorrisPratt kmp_distance("distance=\"");
  static const KnuthMorrisPratt kmp_intensity("intensity=\"");
  static const KnuthMorrisPratt kmp_end("</laser_scan>");

  kmp_begin.search(is);
  int b;
  kmp_valid.search(is);
  is >> b;
  _scan.is_valid = b;
  
  CommTimeStamp ts;
  ts.load_xml(is);
  set_scan_time_stamp(ts);

  kmp_update_count.search(is);
  is >> _scan.update_count;
  kmp_start_angle.search(is);
  is >> _scan.start_angle;
  kmp_resolution.search(is);
  is >> _scan.resolution;
  kmp_length_unit.search(is);
  is >> _scan.length_unit;
  kmp_distance_min.search(is);
  is >> _scan.distance_min;
  kmp_distance_max.search(is);
  is >> _scan.distance_max;

  unsigned int n;
  kmp_points.search(is);
  is >> n;
  _scan.scan_points.length(n);

  for(unsigned int i=0; i<n; ++i)
  {
    kmp_index.search(is);
    is >> _scan.scan_points[i].index;
    kmp_distance.search(is);
    is >> _scan.scan_points[i].distance;
    kmp_intensity.search(is);
    is >> b;
    _scan.scan_points[i].intensity = b;
  }
  kmp_end.search(is);
}
