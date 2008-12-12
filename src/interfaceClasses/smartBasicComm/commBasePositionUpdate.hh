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

#ifndef SMART_COMM_BASE_POSITION_UPDATE_HH
#define SMART_COMM_BASE_POSITION_UPDATE_HH

#include "commTimeStamp.hh"
#include "commBasePosition.hh"

#include "smartBasePositionUpdateC.hh"

namespace Smart {

/**
  Relative update of the base position.
  This class is used for the communication between a self localization component
  and a base server.
  An update consists of 
  <ul>
    <li>an old position with an associated timestamp
        (where the robot believed to be located at the time given by the timestamp)
        and
    <li>a corrected position (where the self localization component found out that the robot
        has been at the time given by the timestamp).
  </ul>
 */
class CommBasePositionUpdate
{
protected:
  SmartIDL::BasePositionUpdate _upd;

public:

  CommBasePositionUpdate();
  CommBasePositionUpdate(const SmartIDL::BasePositionUpdate &upd);
  virtual ~CommBasePositionUpdate();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommBasePositionUpdate"; };

  /// internal use only
  inline const SmartIDL::BasePositionUpdate &get_idl() const { return _upd; }
  /// internal use only
  inline void set_idl(const SmartIDL::BasePositionUpdate &upd) { _upd = upd; }

  /**
    Get the time stamp.
   */
  inline CommTimeStamp get_time_stamp() const { return CommTimeStamp(_upd.time); }
  
  /**
    Get the old and inaccurate position.
   */
  inline CommBasePosition get_old_position() const { return CommBasePosition(_upd.position); }
  
  /**
    Get the new and corrected position.
   */
  inline CommBasePosition get_corrected_position() const { return CommBasePosition(_upd.corrected_position); }

  /**
    Set the time stamp.
   */
  inline void set_time_stamp(const CommTimeStamp &ts) { _upd.time = ts.get_idl(); }

  /**
    Set the old and inaccurate position.
   */
  inline void set_old_position(const CommBasePosition &bp) { _upd.position = bp.get_idl(); }

  /**
    Set the new and corrected position.
   */
  inline void set_corrected_position(const CommBasePosition &bp) { _upd.corrected_position = bp.get_idl(); }

  /**
    Write information about this update to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this base position update to the given output stream.
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

inline std::ostream &operator<<(std::ostream &os, const CommBasePositionUpdate &upd)
{
  upd.print(os);
  return os;
}

} // namespace Smart

#endif
