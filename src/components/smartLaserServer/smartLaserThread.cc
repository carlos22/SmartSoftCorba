//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
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

#include <memory>

#include "smartLaserThread.hh"

#include "smartSickInterface.hh"

#include "commMobileLaserScan.hh"
#include "commBaseState.hh"

using namespace Smart;

LaserThread::LaserThread
(
    CHS::SmartComponent &component, 
    CHS::SmartParameter &parameters,
    Smart::CommMobileLaserScan &global_scan,
    CHS::SmartMutex &global_scan_mutex
)
: _component(component),
  _global_scan(global_scan),
  _global_scan_mutex(global_scan_mutex),
  _push_service_name("scan"),
  _verbose(true),
  _laser_type("LMS"),
  _laser_device("/dev/ttyS0"),
  _laser_bitrate(38400),
  _term_bitrate(38400),
  _laser_resolution(50),
  _laser_length_unit(1),
  _laser_verbose(false),
  _use_base_server(false),
  _base_server_name("baseserver"),
  _base_service_name("state"),
  _scanner_x(0),
  _scanner_y(0),
  _scanner_z(0),
  _scanner_a(0),
  _scanner_on_turret(true)
{
  parameters.getString("service","name",_push_service_name);
  parameters.getTruthValue("service","verbose",_verbose);

  parameters.getString("scanner","sick_type",_laser_type);
  parameters.getString("scanner","device",_laser_device);
  parameters.getInt("scanner","speed",_laser_bitrate);
  parameters.getInt("scanner","term_speed",_term_bitrate);
  parameters.getInt("scanner","resolution",_laser_resolution);
  parameters.getInt("scanner","length_unit",_laser_length_unit);
  parameters.getTruthValue("scanner","verbose",_laser_verbose);

  parameters.getTruthValue("base","subscribe",_use_base_server);
  parameters.getString("base","component",_base_server_name);
  parameters.getString("base","service",_base_service_name);

  parameters.getDouble("scanner","x",_scanner_x);
  parameters.getDouble("scanner","y",_scanner_y);
  parameters.getDouble("scanner","z",_scanner_z);
  parameters.getDouble("scanner","azimuth",_scanner_a);
  parameters.getTruthValue("scanner","on_turret",_scanner_on_turret);
  
  double base_x = 0;
  double base_y = 0;
  double base_z = 0;
  double base_a_base = 0;
  double base_a_turret = 0;
  double base_a_steer = 0;
  parameters.getDouble("base","x",base_x);
  parameters.getDouble("base","y",base_y);
  parameters.getDouble("base","z",base_z);
  parameters.getDouble("base","base_a",base_a_base);
  parameters.getDouble("base","steer_a",base_a_steer);
  parameters.getDouble("base","turret_a",base_a_turret);
  _default_base_position.set_x(base_x);
  _default_base_position.set_y(base_y);
  _default_base_position.set_z(base_z);
  _default_base_position.set_base_alpha(base_a_base);
  _default_base_position.set_steer_alpha(base_a_steer);
  _default_base_position.set_turret_alpha(base_a_turret);
}

int LaserThread::svc()
{
  CommMobileLaserScan scan;
  CommBaseState base_state;
  unsigned long update_count = 0;

  CommBaseVelocity zero_velocity;
  zero_velocity.set_v(0);
  zero_velocity.set_omega_base(0);
  zero_velocity.set_omega_steer(0);
  zero_velocity.set_omega_turret(0);

  // create server object for newest scan push
  CHS::PushNewestServer<Smart::CommMobileLaserScan> push_server(&_component, _push_service_name);

  Smart::SickInterface laser;
  std::auto_ptr<CHS::PushTimedClient<Smart::CommBaseState> > base;

  while(!init_laser(laser))
  {
    std::cerr << "WARNING: failed to init laser, retry in one second" << std::endl;
    sleep(1);
  }

  // check if we have to connect a baseserver
  if(_use_base_server)
  {
    // set up connection to base server
    base.reset(new CHS::PushTimedClient<Smart::CommBaseState>(&_component, _base_server_name, _base_service_name));
    CHS::StatusCode status = base->subscribe(1);
    if(status!=CHS::SMART_OK)
    {
      std::cerr << "ERROR: failed to connect to base state service \"" << _base_service_name << "\" at component \"" << _base_server_name << "\"" << std::endl;
      std::cerr << "ERROR: (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
      return -1;
    }
    else
    {
      if(_verbose) std::cerr << "waiting for initial base state push" << std::endl;
      status = base->getUpdateWait(base_state);
      if(status!=CHS::SMART_OK)
      {
        std::cerr << "ERROR: failed to get data from base state service \"" << _base_service_name << "\" at component \"" << _base_server_name << "\"" << std::endl;
        std::cerr << "ERROR: (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
        status = base->unsubscribe();
        if(status!=CHS::SMART_OK)
        {
          std::cerr << "WARNING: failed to unsubscribe from base state service \"" << _base_service_name << "\" at component \"" << _base_server_name << "\"" << std::endl;
          std::cerr << "WARNING: (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
        }
        return -1;
      }
    }
  }

  while (! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self()))
  {
    if (laser.read_laser_data()==0)
    {
      bool base_state_valid = false;

      scan.set_scan_time_stamp(CommTimeStamp(laser.get_receive_timestamp()));
      scan.set_scan_update_count(update_count);
      
      //////////////////////////
      // set base state
      //
      if(_use_base_server)
      {
        CHS::StatusCode status = base->getUpdate(base_state);
        if(status!=CHS::SMART_OK)
        {
          std::cerr << "WARNING: failed to get current base state (" << CHS::StatusCodeConversion(status) << "), pushing invalid scan" << std::endl;
          scan.set_scan_valid(false);
        }
        else base_state_valid = true;
      }
      else 
      {
        base_state.set_time_stamp(CommTimeStamp(laser.get_receive_timestamp()));
        base_state.set_base_position(_default_base_position);
        base_state.set_base_velocity(zero_velocity);
        base_state_valid = true;
      }
      scan.set_base_state(base_state);

      //////////////////////////
      // set scanner position
      //
      double base_x = 0;
      double base_y = 0;
      double base_z = 0;
      double base_a = 0;
      if(base_state_valid)
      {
        base_x = base_state.get_base_position().get_x();
        base_y = base_state.get_base_position().get_y();
        base_z = base_state.get_base_position().get_z();
        if(_scanner_on_turret)
        {
          base_a = base_state.get_base_position().get_turret_alpha();
        }
        else
        {
          base_a = base_state.get_base_position().get_base_alpha();
        }
      }
      const double sin_base_a = sin(base_a);
      const double cos_base_a = cos(base_a);
      scan.set_scanner_x(base_x + _scanner_x * cos_base_a - _scanner_y * sin_base_a);
      scan.set_scanner_y(base_y + _scanner_x * sin_base_a + _scanner_y * cos_base_a);
      scan.set_scanner_z(base_z + _scanner_z);
      scan.set_scanner_azimuth(base_a + _scanner_a);
      
      //////////////////////////
      // set scan parameters
      //
      scan.set_scan_length_unit(laser.extract_length_unit());
      scan.set_scan_integer_field_of_view(-50 * int(laser.width), laser.resolution);

      //////////////////////////
      // get maximum non-overflow distance
      //
      const unsigned int max_dist = laser.get_max_distance();

      //////////////////////////
      // set scan points
      //
      const unsigned int num_points = laser.extract_num_points();
      unsigned int num_valid_points = 0;
      for(unsigned int i=0; i<num_points; ++i)
      {
        if(laser.extract_distance(i) <= max_dist) ++num_valid_points;
      }
      scan.set_scan_size(num_valid_points);
      unsigned int valid_point_index = 0;
      for(unsigned int i=0; i<num_points; ++i)
      {
        const unsigned int dist = laser.extract_distance(i);
        if(dist <= max_dist)
        {
          scan.set_scan_index(valid_point_index, i);
          scan.set_scan_integer_distance(valid_point_index, dist);
          scan.set_scan_intensity(valid_point_index, laser.extract_intensity(i));
          ++valid_point_index;
        }
      }
      scan.set_scan_valid(base_state_valid /* && ... */);
      
      CHS::StatusCode push_status = push_server.put(scan);
      if(push_status!=CHS::SMART_OK)
      {
        std::cerr << "WARNING: error on push (" << CHS::StatusCodeConversion(push_status) << ")" << std::endl;
      }
      
      _global_scan_mutex.acquire();
      _global_scan = scan;
      _global_scan_mutex.release();

      const unsigned int index = 180;
      if(_verbose) std::cout << "Scan " << update_count << " sent." << " Scan Position " << index << " = " << scan.get_scan_distance(index) << " mm" << std::endl;
      ++update_count;
    }
    else
    {
      std::cerr << "WARNING: received unexpected packet type" << std::endl;
    }
  }

  laser.close_device();

  if(_use_base_server)
  {
    CHS::StatusCode status = base->unsubscribe();
    if(status!=CHS::SMART_OK)
    {
      std::cerr << "WARNING: failed to unsubscribe from base state service \"" << _base_service_name << "\" at component \"" << _base_server_name << "\"" << std::endl;
      std::cerr << "WARNING: (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
    }
  }

  return 0;
}

bool LaserThread::init_laser(Smart::SickInterface &laser) const
{
  // set up laser connection
  laser.verbose = _laser_verbose;
  laser.length_unit = _laser_length_unit;

  laser.resolution = _laser_resolution;
  if(laser.resolution==25)
  {
    laser.width = 100;
  }
  else
  {
    laser.width = 180;
  }

  laser.bitrate = _laser_bitrate;
  laser.term_bitrate = _term_bitrate;
  
  if(_laser_type=="LMS")
  {
    laser.sick_type = Smart::SickInterface::LMS;
  }
  else if(_laser_type=="PLS")
  {
    laser.sick_type = Smart::SickInterface::PLS;
  }
  else
  {
    std::cerr << "WARNING: unknown SICK type \"" << _laser_type << "\", using default (\"LMS\")." << std::endl;
    laser.sick_type = Smart::SickInterface::LMS;
  }

  if(laser.open_device(_laser_device.c_str()))
  {
    std::cerr << "ERROR: laser.open_device(\"" << _laser_device << "\") failed." << std::endl;
    return false;
  }
  unsigned int current_laser_speed = 0;
  std::string laser_type;
  if(laser.probe_sick_speed(current_laser_speed, laser_type))
  {
    std::cerr << "ERROR: probe_laser_speed() failed." << std::endl;
    return false;
  }
  if(_verbose)
  {
    std::cout << "Connected to \"" << laser_type << "\", initially at " << current_laser_speed << "bps" << std::endl;
  }
  if(laser.set_laser_config())
  {
    std::cerr << "ERROR: set_laser_config() failed." << std::endl;
    return false;
  }
  for(unsigned int retry = 0; true; ++retry)
  {
    if(laser.request_laser_data()==0) break;
    if (retry >= 5)
    {
      std::cerr << "ERROR: request_laser_data() failed." << std::endl;
      return false;
    }
  }
  if(_verbose)
  {
    std::cout << "Laser ready." << std::endl;
  }
  return true;
}
