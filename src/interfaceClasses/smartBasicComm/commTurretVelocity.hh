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

#ifndef SMART_COMM_TURRET_VELOCITY_HH
#define SMART_COMM_TURRET_VELOCITY_HH

#include <string>
#include <iostream>

#include "smartTurretVelocityC.hh"

namespace Smart {

/**
  The angular velocity of the turret of a robot.
  Instances of this class are used to command such a velocity to a robot.
  Generally, the robot reports this velocity together with its complete state.
  \sa Smart::CommBaseState, Smart::CommBaseVelocity
 */
class CommTurretVelocity
{
protected:
  SmartIDL::TurretVelocity _vel;

public:

  CommTurretVelocity();
  CommTurretVelocity(const SmartIDL::TurretVelocity &vel);
  virtual ~CommTurretVelocity();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommTurretVelocity"; };

  inline const SmartIDL::TurretVelocity &get_idl() const { return _vel; }
  inline void set_idl(const SmartIDL::TurretVelocity &vel) { _vel = vel; }

  //
  // GET
  //

  /**
    Get the turret velocity in radians per second.
   */
  inline double get_omega() const { return _vel.omega; }

  //
  // SET
  //

  /**
    Set the angular velocity of the turret in radians per second.
   */
  inline void set_omega(double omega) { _vel.omega = omega; }

  //
  // I/O
  //

  /**
    Write information about this turret velocity to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this turret velocity to the given output stream.
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

inline std::ostream &operator<<(std::ostream &os, const CommTurretVelocity &vel)
{
  vel.print(os);
  return os;
}

} // namespace Smart

#endif
