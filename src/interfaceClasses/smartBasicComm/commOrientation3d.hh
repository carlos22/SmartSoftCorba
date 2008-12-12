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

#ifndef SMART_COMM_ORIENTATION_3D_HH
#define SMART_COMM_ORIENTATION_3D_HH

#include <string>
#include <iostream>

#include "smartOrientation3dC.hh"

namespace Smart {

/**
  The orientation of a body in 3d space.
  The orientation is given by the three angles
  <ul>
    <li>azimuth,
    <li>elevation, and
    <li>roll.
  </ul>
  Azimuth and elevation define the direction of the x-axis of the body's
  coordinate system, roll defines the rotation of the body arround the direction
  of this coordinate systems's x-axis.
  \sa Smart::CommDirection3d
 */
class CommOrientation3d
{
protected:
  SmartIDL::Orientation3d _ori;

public:

  CommOrientation3d();
  CommOrientation3d(double azimuth, double elevation, double roll);
  CommOrientation3d(const SmartIDL::Orientation3d &ori);
  virtual ~CommOrientation3d();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommOrientation3d"; };

  /// internal use only
  inline const SmartIDL::Orientation3d &get_idl() const { return _ori; }
  /// internal use only
  inline void set_idl(const SmartIDL::Orientation3d &ori) { _ori = ori; }

  /**
    Get the azimuth in radians.
   */
  inline double get_azimuth() const { return _ori.azimuth; }
  /**
    Get the elevation in radians.
   */
  inline double get_elevation() const { return _ori.elevation; }
  /**
    Get the roll in radians.
   */
  inline double get_roll() const { return _ori.roll; }

  /**
    Set the azimuth in radians.
   */
  inline void set_azimuth(double a) { _ori.azimuth = a; }
  /**
    Set the elevation in radians.
   */
  inline void set_elevation(double e) { _ori.elevation = e; }
  /**
    Set the roll in radians.
   */
  inline void set_roll(double r) { _ori.roll = r; }

  /**
    Write information about this orientation to the given ouput stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this 3d orientation to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

inline std::ostream &operator<<(std::ostream &os, const CommOrientation3d &ori)
{
  ori.print(os);
  return os;
}

} // namespace Smart

#endif
