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

#ifndef SMART_COMM_DIRECTION_3D_HH
#define SMART_COMM_DIRECTION_3D_HH

#include <string>
#include <iostream>

#include "smartDirection3dC.hh"

namespace Smart {

/**
  A direction in 3d space.
  A direction is given by an azimuth angle \f$\alpha\f$ and an elevation angle \f$\epsilon\f$.
  Then, an according direction vector \f$(x,y,z)^T\f$ is given by 
  \f$x=\cos\alpha\cos\epsilon\f$, \f$y=\sin\alpha\cos\epsilon\f$, and
  \f$z=\sin\epsilon\f$.
 */
class CommDirection3d
{
protected:
  SmartIDL::Direction3d _dir;

public:

  CommDirection3d();
  CommDirection3d(double azimuth, double elevation);
  CommDirection3d(const SmartIDL::Direction3d &dir);
  virtual ~CommDirection3d();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommDirection3d"; };

  /// internal use only
  inline const SmartIDL::Direction3d &get_idl() const { return _dir; }

  /**
    Get the azimuth angle in radians.
   */
  inline double get_azimuth() const { return _dir.azimuth; }
  
  /**
    Get the elevation angle in radians.
   */
  inline double get_elevation() const { return _dir.elevation; }

  /**
    Set the azimuth angle in radians.
   */
  inline void set_azimuth(double a) { _dir.azimuth = a; }
  
  /**
    Set the elevation angle in radians.
   */
  inline void set_elevation(double e) { _dir.elevation = e; }

  /**
    Write information about this direction to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this 3d direction to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

inline std::ostream &operator<<(std::ostream &os, const CommDirection3d &dir)
{
  dir.print(os);
  return os;
}

} // namespace Smart

#endif
