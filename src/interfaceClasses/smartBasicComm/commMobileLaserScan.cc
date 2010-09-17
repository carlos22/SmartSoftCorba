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
  _scan.laser_scan_pose.laser_scan.is_valid = false;
  _scan.laser_scan_pose.laser_scan.length_unit = 1;
  _scan.laser_scan_pose.laser_scan.update_count = 0;
  _scan.laser_scan_pose.laser_scan.time.sec = 0;
  _scan.laser_scan_pose.laser_scan.time.usec = 0;
  _scan.laser_scan_pose.laser_scan.start_angle = 27000; // -90 deg
  _scan.laser_scan_pose.laser_scan.resolution = 50;     // 0.5 deg
  _scan.laser_world_pose.position.x = 0;
  _scan.laser_world_pose.position.y = 0;
  _scan.laser_world_pose.position.z = 0;
  _scan.laser_world_pose.orientation.azimuth = 0;
  _scan.laser_world_pose.orientation.elevation = 0;
  _scan.laser_world_pose.orientation.roll = 0;
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
  SmartIDL::LaserScan tmp_scan = _scan.laser_scan_pose.laser_scan;
  const unsigned int size = tmp_scan.scan_points.length();
  
  unsigned int num_points_to_remove = 0;
  for(unsigned int i=0; i<size; ++i)
  {
    if(tmp_scan.scan_points[i].distance > max_dist) ++num_points_to_remove;
  }

  _scan.laser_scan_pose.laser_scan.scan_points.length(size - num_points_to_remove);
  unsigned int num_points_removed = 0;
  for(unsigned int i=0; i<size; ++i)
  {
    if(tmp_scan.scan_points[i].distance > max_dist)
    {
      ++num_points_removed;
    }
    else
    {
      _scan.laser_scan_pose.laser_scan.scan_points[i - num_points_removed] = tmp_scan.scan_points[i];
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
  CommPose3d(_scan.laser_world_pose).save_xml(os, indent + "  ");
  CommPose3d(_scan.laser_scan_pose.sensor_pose).save_xml(os, indent + "  ");
  CommLaserScan(_scan.laser_scan_pose.laser_scan).save_xml(os, indent + "  ");
  os << indent << "</mobile_laser_scan>" << std::endl;
}

void CommMobileLaserScan::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<mobile_laser_scan>");

  static const KnuthMorrisPratt kmp_end("</mobile_laser_scan>");

  kmp_begin.search(is);
  
  CommBaseState bs;
  bs.load_xml(is);
  set_base_state(bs);
  
  CommPose3d world_pose;
  world_pose.load_xml(is);
  _scan.laser_world_pose = world_pose.get_idl();

  CommPose3d sensor_pose;
  sensor_pose.load_xml(is);
  _scan.laser_scan_pose.sensor_pose = sensor_pose.get_idl();

  CommLaserScan scan;
  scan.load_xml(is);
  _scan.laser_scan_pose.laser_scan = scan.get_idl();

  kmp_end.search(is);
}
