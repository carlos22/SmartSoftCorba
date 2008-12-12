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

#ifndef SMART_COMM_POSITION_3D_HH
#define SMART_COMM_POSITION_3D_HH

#include <string>
#include <iostream>

#include "smartPosition3dC.hh"

namespace Smart {

/**
  A position in 3d space is a point in 3d space.
 */
class CommPosition3d
{
protected:
  SmartIDL::Position3d _pos;

public:

  CommPosition3d();
  CommPosition3d(double x, double y, double z, const double unit = 0.001);
  CommPosition3d(const SmartIDL::Position3d &pos);
  virtual ~CommPosition3d();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommPosition3d"; };

  inline const SmartIDL::Position3d &get_idl() const { return _pos; }
  inline void set_idl(const SmartIDL::Position3d &pos) { _pos = pos; }

  /**
    Get the x coordinate of the position.
   */
  inline double get_x(const double unit = 0.001) const { return _pos.x * 0.001 / unit; }
  /**
    Get the y coordinate of the position.
   */
  inline double get_y(const double unit = 0.001) const { return _pos.y * 0.001 / unit; }
  /**
    Get the z coordinate of the position.
   */
  inline double get_z(const double unit = 0.001) const { return _pos.z * 0.001 / unit; }

  /**
    Set the x coordinate of the position.
   */
  inline void set_x(double x, const double unit = 0.001) { _pos.x = x * unit * 1000; }
  /**
    Set the y coordinate of the position.
   */
  inline void set_y(double y, const double unit = 0.001) { _pos.y = y * unit * 1000; }
  /**
    Set the z coordinate of the position.
   */
  inline void set_z(double z, const double unit = 0.001) { _pos.z = z * unit * 1000; }

  /**
    Write information about this position to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this 3d position to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

inline std::ostream &operator<<(std::ostream &os, const CommPosition3d &pos)
{
  pos.print(os);
  return os;
}

} // namespace Smart

#endif
