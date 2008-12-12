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

#ifndef SMART_COMM_BASE_VELOCITY_HH
#define SMART_COMM_BASE_VELOCITY_HH

#include <string>
#include <iostream>

#include "smartBaseVelocityC.hh"

namespace Smart {

/**
  A class describing the velocity of a mobile robot.
  The velocity description contains
  <ul>
    <li>the translational velocity of the robot motion,
    <li>the angular velocity of the robot motion (the steering),
    <li>the angular velocity of the base (this may be different from the steering
        velocity, for example on robots with turrets the base does not change its orientation),
    <li>the angular velocity of the turret.
  </ul>
  On robots with differential drive and without turret, the three angular velocities are equal.
 */
class CommBaseVelocity
{
protected:
  SmartIDL::BaseVelocity _vel;

public:

  CommBaseVelocity();
  CommBaseVelocity(const SmartIDL::BaseVelocity &vel);
  virtual ~CommBaseVelocity();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommBaseVelocity"; };

  inline const SmartIDL::BaseVelocity &get_idl() const { return _vel; }
  inline void set_idl(const SmartIDL::BaseVelocity &vel) { _vel = vel; }

  //
  // GET
  //

  /**
    Get the translation velocity in units of \a unit meters per second.
    Default unit is millimeters per second.
   */
  inline double get_v(const double unit = 0.001) const { return _vel.v * (0.001 / unit); }

  /**
    Get the angular velocity of the base in radians per secons.
   */
  inline double get_omega_base() const { return _vel.omega_base; }

  /**
    Get the angular velocity of the steering in radians per secons.
   */
  inline double get_omega_steer() const { return _vel.omega_steer; }

  /**
    Get the angular velocity of the turret in radians per secons.
   */
  inline double get_omega_turret() const { return _vel.omega_turret; }

  //
  // SET
  //

  /**
    Set the translation velocity in units of \a unit meters per second.
    Default unit is millimeters per second.
   */
  inline void set_v(double v, const double unit = 0.001) { _vel.v = v * (1000 * unit); }

  /**
    Set the angular velocity of the base.
    This method is only usefull to implementers of base servers.
   */
  inline void set_omega_base(double omega) { _vel.omega_base = omega; }

  /**
    Set the angular change rate of the steering.
    This method is only usefull to implementers of base servers.
   */
  inline void set_omega_steer(double omega) { _vel.omega_steer = omega; }

  /**
    Set the angular velocity of the turret.
    This method is only usefull to implementers of base servers.
   */
  inline void set_omega_turret(double omega) { _vel.omega_turret = omega; }

  /**
    Write information about this base velocity to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this base velocity to the given output stream.
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

inline std::ostream &operator<<(std::ostream &os, const CommBaseVelocity &vel)
{
  vel.print(os);
  return os;
}

} // namespace Smart

#endif
