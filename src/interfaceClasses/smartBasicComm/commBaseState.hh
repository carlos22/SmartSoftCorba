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

#ifndef SMART_COMM_BASE_STATE_HH
#define SMART_COMM_BASE_STATE_HH

#include <math.h>

#include "smartBaseStateC.hh"

#include "commTimeStamp.hh"
#include "commBasePosition.hh"
#include "commBaseVelocity.hh"

namespace Smart {

/**
  The state of a mobile robot at a given time.
  The state consists of the following components:
  <ul>
    <li>a time stamp when the state was present,
    <li>the position (i.e. the configuration) of the base, and
    <li>the velocity of the base.
  </ul>
  \sa Smart::CommTimeStamp Smart::CommBasePosition Smart::CommBaseVelocity
 */
class CommBaseState
{
protected:
  SmartIDL::BaseState _state;

public:

  CommBaseState() {}
  CommBaseState(const SmartIDL::BaseState &state) : _state(state) {}
  virtual ~CommBaseState() {}

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommBaseState"; };

  /// internal use only
  inline const SmartIDL::BaseState &get_idl() const { return _state; }
  /// internal use only
  inline void set_idl(const SmartIDL::BaseState &state) { _state = state; }

  /**
    Set the time stamp.
   */
  inline void set_time_stamp(const CommTimeStamp &ts) { _state.time = ts.get_idl(); }
  
  /**
    Set the base position.
   */
  inline void set_base_position(const CommBasePosition &bp) { _state.base_position = bp.get_idl(); }

  /**
    Set the base velocity.
   */
  inline void set_base_velocity(const CommBaseVelocity &bv) { _state.base_velocity = bv.get_idl(); }
  
  /**
    Get the time stamp.
   */
  inline CommTimeStamp get_time_stamp() const { return CommTimeStamp(_state.time); }
  
  /**
    Get the base position.
   */
  inline CommBasePosition get_base_position() const { return CommBasePosition(_state.base_position); }

  /**
    Get the base velocity.
   */
  inline CommBaseVelocity get_base_velocity() const { return CommBaseVelocity(_state.base_velocity); }

  /**
    Return this basestate at time \a ts, assuming constant velocities.
   */
  CommBaseState extrapolate(const CommTimeStamp &ts) const;

  /**
    Print information about this base state to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this base state to the given output stream.
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

inline std::ostream &operator<<(std::ostream &os, const CommBaseState &state)
{
  state.print(os);
  return os;
}

} // namespace Smart

#endif
