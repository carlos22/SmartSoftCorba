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

#ifndef SMART_COMM_BASE_POSITION_HH
#define SMART_COMM_BASE_POSITION_HH

#include <string>
#include <iostream>

#include "commPosition3d.hh"
#include "commDirection3d.hh"
#include "commOrientation3d.hh"
#include "commPose3d.hh"

#include "smartBasePositionC.hh"

/** Namespace for communication objects and visualization stuff */
namespace Smart {

/**
  Description of the position of a mobile robot.
  The Description of the mobile robot position includes
  <ul>
    <li>a three dimensional position,
    <li>the orientation in the plane (azimuth) of 
      <ul>
        <li>the base, 
        <li>the steering direction, and 
        <li>the turret direction,
      </ul>
    <li>the covariance matrix for the (x,y) position and the base orientation, and
    <li>an update counter which is increased whenever the base server changes its
        coordinate system (due to self localization).
  </ul>
 */
class CommBasePosition
{
protected:
  SmartIDL::BasePosition _pos;

public:

  enum RobotComponent
  {
    BASE   = SmartIDL::ROBOT_BASE_COMPONENT,
    STEER  = SmartIDL::ROBOT_STEER_COMPONENT,
    TURRET = SmartIDL::ROBOT_TURRET_COMPONENT
  };

  /**
    Default constructor.
   */
  CommBasePosition();
  CommBasePosition(const SmartIDL::BasePosition &pos);
  virtual ~CommBasePosition();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommBasePosition"; };

  inline const SmartIDL::BasePosition &get_idl() const { return _pos; }
  inline void set_idl(const SmartIDL::BasePosition &pos) { _pos = pos; }

  //
  // GET
  //
  
  /**
    Get the x position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline double get_x(const double unit = 0.001) const { return _pos.x * (0.001 / unit); }

  /**
    Get the y position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline double get_y(const double unit = 0.001) const { return _pos.y * (0.001 / unit); }

  /**
    Get the z position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline double get_z(const double unit = 0.001) const { return _pos.z * (0.001 / unit); }

  /**
    Return the orientation of the base 
    in the global coordinate system.
   */
  inline double get_base_alpha() const { return _pos.a_base; }
  
  /**
    Return the steering direction in the global coordinate system.
   */
  inline double get_steer_alpha() const { return _pos.a_steer; }
  
  /**
    Return the orientation of the turret/enclosure in the global coordinate system.
   */
  inline double get_turret_alpha() const { return _pos.a_turret; }

  inline double get_alpha(int robot_component) const
  {
    switch(robot_component)
    {
      case BASE: return get_base_alpha();
      case STEER: return get_steer_alpha();
      case TURRET: return get_turret_alpha();
      default: return 0;
    }
  }

  /**
    Return the update count of this base position instance.
    The base server increases the update count whenever it changes its coordinate system.
   */
  inline unsigned long get_update_count() const { return _pos.update_count; }

  /**
    Return an element of the position covariance matrix.
    Index counting starts at zero.
   */
  inline double get_cov(unsigned int i, unsigned int j) const;

  //
  // CONVERSION METHODS
  //

  inline CommPosition3d get_position3d() const { return CommPosition3d(_pos.x, _pos.y, _pos.z); }
  inline CommDirection3d get_base_direction3d() const { return CommDirection3d(_pos.a_base, 0); }
  inline CommDirection3d get_steer_direction3d() const { return CommDirection3d(_pos.a_steer, 0); }
  inline CommDirection3d get_turret_direction3d() const { return CommDirection3d(_pos.a_turret, 0); }
  inline CommOrientation3d get_base_orientation3d() const { return CommOrientation3d(_pos.a_base, 0, 0); }
  inline CommOrientation3d get_steer_orientation3d() const { return CommOrientation3d(_pos.a_steer, 0, 0); }
  inline CommOrientation3d get_turret_orientation3d() const { return CommOrientation3d(_pos.a_turret, 0, 0); }
  inline CommPose3d get_base_pose3d() const { return CommPose3d(get_position3d(), get_base_orientation3d()); }
  inline CommPose3d get_steer_pose3d() const { return CommPose3d(get_position3d(), get_steer_orientation3d()); }
  inline CommPose3d get_turret_pose3d() const { return CommPose3d(get_position3d(), get_turret_orientation3d()); }

  //
  // COORDINATE SYSTEM CONVERSION METHODS
  //
/*
  inline CommPosition3d convert_base2world(const CommPosition3d &pos) const;
  inline CommPosition3d convert_turret2world(const CommPosition3d &pos) const;
  inline CommPosition3d convert_steer2world(const CommPosition3d &pos) const;

  inline CommDirection3d convert_base2world(const CommDirection3d &pos) const;
  inline CommDirection3d convert_turret2world(const CommDirection3d &pos) const;
  inline CommDirection3d convert_steer2world(const CommDirection3d &pos) const;

  inline CommOrientation3d convert_base2world(const CommOrientation3d &pos) const;
  inline CommOrientation3d convert_turret2world(const CommOrientation3d &pos) const;
  inline CommOrientation3d convert_steer2world(const CommOrientation3d &pos) const;

  inline CommPose3d convert_base2world(const CommPose3d &pos) const;
  inline CommPose3d convert_turret2world(const CommPose3d &pos) const;
  inline CommPose3d convert_steer2world(const CommPose3d &pos) const;


  inline CommPosition3d convert_world2base(const CommPosition3d &pos) const;
  inline CommPosition3d convert_world2turret(const CommPosition3d &pos) const;
  inline CommPosition3d convert_world2steer(const CommPosition3d &pos) const;

  inline CommDirection3d convert_world2base(const CommDirection3d &pos) const;
  inline CommDirection3d convert_world2turret(const CommDirection3d &pos) const;
  inline CommDirection3d convert_world2steer(const CommDirection3d &pos) const;

  inline CommOrientation3d convert_world2base(const CommOrientation3d &pos) const;
  inline CommOrientation3d convert_world2turret(const CommOrientation3d &pos) const;
  inline CommOrientation3d convert_world2steer(const CommOrientation3d &pos) const;

  inline CommPose3d convert_world2base(const CommPose3d &pos) const;
  inline CommPose3d convert_world2turret(const CommPose3d &pos) const;
  inline CommPose3d convert_world2steer(const CommPose3d &pos) const;
*/
  //
  // SET
  //

  /**
    Set the x position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline void set_x(double x, const double unit = 0.001) { _pos.x = x * (unit * 1000); }

  /**
    Set the y position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline void set_y(double y, const double unit = 0.001) { _pos.y = y * (unit * 1000); }

  /**
    Set the z position in units of \a unit meters.
    Default unit is millimeters.
   */
  inline void set_z(double z, const double unit = 0.001) { _pos.z = z * (unit * 1000); }

  /**
    Set the base orientation in radians.
    Either the baseserver sets this value to report the current orientation of
    the base, or the client sets this value in order to redefine the base orientation
    (for self-localisation).
   */
  inline void set_base_alpha(double alpha) { _pos.a_base = alpha; }

  /**
    Set the steering direction in radians.
    This method is usefull for implementers of a baseservers only, since
    the client may only define the base direction (the server knows the exact offset
    between base and steering direction).
   */
  inline void set_steer_alpha(double alpha) { _pos.a_steer = alpha; }

  /**
    Set the turret direction in radians.
    This method is usefull for implementers of a baseservers only, since
    the client may only define the base direction (the server knows the exact offset
    between base and turret direction).
   */
  inline void set_turret_alpha(double alpha) { _pos.a_turret = alpha; }

  /**
    Set the update counter of this base position instance.
    The baseserver increases the update counter whenever it changes its coordinate system.
   */
  inline void set_update_count(unsigned long l) { _pos.update_count = l; }

  /**
    Set an element of the covariance matrix of the position.
    Index counting starts at zero.
   */
  inline void set_cov(unsigned int i, unsigned int j, double d);

  /**
    Print this base position to the \a os output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this base position to the given output stream.
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

inline double CommBasePosition::get_cov(unsigned int i, unsigned int j) const
{
  // FIXME: check this
  return _pos.m[i*3+j];
}

inline void CommBasePosition::set_cov(unsigned int i, unsigned int j, double d)
{
  // FIXME: check this
  _pos.m[i*3+j] = d;
}

inline std::ostream &operator<<(std::ostream &os, const CommBasePosition &pos)
{
  pos.print(os);
  return os;
}

/*
inline CommPosition3d CommBasePosition::convert_base2world(const CommPosition3d &pos) const
{
}


inline CommPosition3d CommBasePosition::convert_turret2world(const CommPosition3d &pos) const
{
}


inline CommPosition3d CommBasePosition::convert_steer2world(const CommPosition3d &pos) const
{
}



inline CommDirection3d CommBasePosition::convert_base2world(const CommDirection3d &pos) const
{
}


inline CommDirection3d CommBasePosition::convert_turret2world(const CommDirection3d &pos) const
{
}


inline CommDirection3d CommBasePosition::convert_steer2world(const CommDirection3d &pos) const
{
}



inline CommOrientation3d CommBasePosition::convert_base2world(const CommOrientation3d &pos) const
{
}


inline CommOrientation3d CommBasePosition::convert_turret2world(const CommOrientation3d &pos) const
{
}


inline CommOrientation3d CommBasePosition::convert_steer2world(const CommOrientation3d &pos) const
{
}



inline CommPose3d CommBasePosition::convert_base2world(const CommPose3d &pos) const
{
}


inline CommPose3d CommBasePosition::convert_turret2world(const CommPose3d &pos) const
{
}


inline CommPose3d CommBasePosition::convert_steer2world(const CommPose3d &pos) const
{
}




inline CommPosition3d CommBasePosition::convert_world2base(const CommPosition3d &pos) const
{
}


inline CommPosition3d CommBasePosition::convert_world2turret(const CommPosition3d &pos) const
{
}


inline CommPosition3d CommBasePosition::convert_world2steer(const CommPosition3d &pos) const
{
}



inline CommDirection3d CommBasePosition::convert_world2base(const CommDirection3d &pos) const
{
}


inline CommDirection3d CommBasePosition::convert_world2turret(const CommDirection3d &pos) const
{
}


inline CommDirection3d CommBasePosition::convert_world2steer(const CommDirection3d &pos) const
{
}



inline CommOrientation3d CommBasePosition::convert_world2base(const CommOrientation3d &pos) const
{
}


inline CommOrientation3d CommBasePosition::convert_world2turret(const CommOrientation3d &pos) const
{
}


inline CommOrientation3d CommBasePosition::convert_world2steer(const CommOrientation3d &pos) const
{
}



inline CommPose3d CommBasePosition::convert_world2base(const CommPose3d &pos) const
{
}


inline CommPose3d CommBasePosition::convert_world2turret(const CommPose3d &pos) const
{
}


inline CommPose3d CommBasePosition::convert_world2steer(const CommPose3d &pos) const
{
}

*/

} // namespace Smart

#endif
