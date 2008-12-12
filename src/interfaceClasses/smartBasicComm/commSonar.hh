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

#ifndef SMART_COMM_SONAR_HH
#define SMART_COMM_SONAR_HH

#include <cmath>
#include <string>
#include <iostream>

#include "commTimeStamp.hh"
#include "commBaseState.hh"
#include "commPosition3d.hh"
#include "commDirection3d.hh"

#include "smartSonarC.hh"

namespace Smart {

/**
  A description of the sonar sensor configuration.
 */
class CommSonarConfig
{
protected:
  SmartIDL::SonarSystemConfig _config;

public:

  CommSonarConfig();
  CommSonarConfig(const SmartIDL::SonarSystemConfig &config);
  virtual ~CommSonarConfig();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommSonarConfig"; };

  /// internal use only
  inline const SmartIDL::SonarSystemConfig &get_idl() const { return _config; }

  //
  // GET
  //
  
  /**
    Return the number of sonar sensors in the system.
   */
  inline unsigned int get_num_sensors() const 
  { 
    return _config.sonar_config.length(); 
  }

  /**
    Return the robot component (base or turret) 
    on which the sensor number \a i is mounted.
   */
  inline CommBasePosition::RobotComponent get_mount_point(unsigned int i) const
  {
    return (CommBasePosition::RobotComponent)(_config.sonar_config[i].mount_point);
  }

  /**
    Return the position of sensor \a i relative the robot.
   */
  inline CommPosition3d get_position(unsigned int i) const 
  { 
    return CommPosition3d(_config.sonar_config[i].position); 
  }

  /**
    Return the direction in which sensor number \a i is pointing relative to the robot.
   */
  inline CommDirection3d get_direction(unsigned int i) const
  {
    return CommDirection3d(_config.sonar_config[i].direction);
  }

  /**
    Return the half beam width of the sensor.
   */
  inline double get_half_width(unsigned int i) const
  {
    return _config.sonar_config[i].half_width;
  }

  //
  // SET
  //
  
  /**
    Define the number of sonar sensors in the system.
   */
  inline void set_num_sensors(unsigned int n)
  {
    _config.sonar_config.length(n);
  }

  /**
    Define the part of the robot where sonar sensor number \a i is mounted.
   */
  inline void set_mount_point(unsigned int i, CommBasePosition::RobotComponent c)
  {
    _config.sonar_config[i].mount_point = (SmartIDL::RobotComponentEnum)c;
  }

  /**
    Define the position of sensor \a i relative to the robot.
   */
  inline void set_position(unsigned int i, const CommPosition3d &pos)
  {
    _config.sonar_config[i].position = pos.get_idl();
  }

  /**
    Define the direction of sensor \a i relative to the robot.
   */
  inline void set_direction(unsigned int i, const CommDirection3d &dir)
  {
    _config.sonar_config[i].direction = dir.get_idl();
  }
  
  /**
    Define the half beam width of sensor number \a i in radians.
   */
  inline void set_half_width(unsigned int i, double hw)
  {
    _config.sonar_config[i].half_width = hw;
  }

  /**
    Write information about this timestamp to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this time stamp to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};


/**
  A description of sonar sensor data.
 */
class CommSonarData
{
protected:
  SmartIDL::SonarSystemData _data;

public:

  CommSonarData();
  CommSonarData(const SmartIDL::SonarSystemData &data);
  virtual ~CommSonarData();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommSonarData"; };

  /// internal use only
  inline const SmartIDL::SonarSystemData &get_idl() const { return _data; }

  //
  // GET
  //
  
  /**
    Return the number of measurements in this data record.
   */
  inline unsigned int get_num_measurements() const
  {
    return _data.sonar_data.length();
  }
  
  /**
    Return a current base state that is attached to this data record.
   */
  inline CommBaseState get_base_state() const
  {
    return CommBaseState(_data.base_state);
  }

  /**
    Return the index of the sensor that produced measurement number \a i.
    Using this index you can query the sonar system configuration for the
    parameters (position, orientation,...) of the sensor.
    \sa CommSonarConfig
   */
  inline unsigned int get_sensor_id(unsigned int i) const
  {
    return _data.sonar_data[i].sensor_id;
  }

  /**
    Return the time when measurement \a i was created.
   */
  inline CommTimeStamp get_measurement_time_stamp(unsigned int i) const
  {
    return CommTimeStamp(_data.sonar_data[i].time_stamp);
  }

  /**
    Return information if measurement \a i returned valid data.
   */
  inline bool is_measurement_valid(unsigned int i) const
  {
    return _data.sonar_data[i].is_valid;
  }

  /**
    Return the distance perceived by measurement \a i.
   */
  inline double get_measured_distance(unsigned int i, const double unit = 0.001) const
  {
    return _data.sonar_data[i].distance * 0.001 / unit;
  }

  //
  // SET
  //
  
  /**
    Define the number of measurements.
    Each measurement \c i is associated with sensor index \c i and 
    initialized to be invalid.
   */
  inline void set_num_measurements(unsigned int n)
  {
    _data.sonar_data.length(n);
    for(unsigned int i=0; i<n; ++i)
    {
      set_sensor_id(i,i);
      set_measurement_valid(i,false);
    }
  }
  
  /**
    Associate the base state \a bs with this sonar data record.
   */
  inline void set_base_state(const CommBaseState &bs)
  {
    _data.base_state = bs.get_idl();
  }

  /**
    Define the ID of the sensor generating measurement \a i to be \a id.
   */
  inline void set_sensor_id(unsigned int i, unsigned int id)
  {
    _data.sonar_data[i].sensor_id = id;
  }

  /**
    Set the time stamp for measurement \a i.
   */
  inline void set_measurement_time_stamp(unsigned int i, const CommTimeStamp &ts)
  {
    _data.sonar_data[i].time_stamp = ts.get_idl();
  }

  /**
    Define if measurement \a i returned a valid distance.
   */
  inline void set_measurement_valid(unsigned int i, bool is_valid)
  {
    _data.sonar_data[i].is_valid = is_valid;
  }

  /**
    Define the distance for measurement \a i.
   */
  inline void set_measured_distance(unsigned int i, double dist, const double unit = 0.001)
  {
    const unsigned int d = int(::rint(dist * unit * 1000));
    if(d <= 65535)
    {
      _data.sonar_data[i].distance = d;
      set_measurement_valid(i, true);
    }
    else
    {
      _data.sonar_data[i].distance = 65535;
      set_measurement_valid(i, false);
    }
  }

  /**
    Write information about this timestamp to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this time stamp to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

//
// Implementation
//

inline std::ostream &operator<<(std::ostream &os, const CommSonarConfig &config)
{
  config.print(os);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const CommSonarData &data)
{
  data.print(os);
  return os;
}

} // namespace Smart

#endif
