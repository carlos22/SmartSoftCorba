//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge, Andreas Steck
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License 
//  as published by the Free Software Foundation; either version 2.1 
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along 
//  with this library; if not, write to the Free Software Foundation, Inc., 
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#ifndef SMART_LASER_THREAD_HH
#define SMART_LASER_THREAD_HH

#include "smartSoft.hh"
#include "commBaseState.hh"
#include "commMobileLaserScan.hh"
#include "smartSickInterface.hh"

namespace Smart {

class LaserThread: public CHS::SmartTask
{
public:

  LaserThread
  (
    CHS::SmartComponent &component, 
    CHS::SmartParameter &parameters,
    Smart::CommMobileLaserScan &global_scan,
    CHS::SmartMutex &global_scan_mutex
  );
    
  int svc();

  bool init_laser(Smart::SickInterface &laser) const;

private:

  CHS::SmartComponent &_component;
  Smart::CommMobileLaserScan &_global_scan;
  CHS::SmartMutex &_global_scan_mutex;
  std::string _push_service_name;

  bool _verbose;

  // laser parameters
  std::string  _laser_type;
  std::string  _laser_device;
  unsigned int _laser_bitrate;
  unsigned int _laser_resolution;
  unsigned int _laser_length_unit;
  bool _laser_verbose;
  
  // base server parameters
  bool _use_base_server;
  std::string _base_server_name;
  std::string _base_service_name;

  Smart::CommBasePosition _default_base_position;

  // position of scanner on robot
  double _scanner_x;
  double _scanner_y;
  double _scanner_z;
  double _scanner_a; // azimuth relative to coordinate system of mounting point
  bool _scanner_on_turret; // if false, scanner on base.
};

} // namespace Smart

#endif
