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

#ifndef SMART_COMM_JOYSTICK_HH
#define SMART_COMM_JOYSTICK_HH

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>

#include "smartJoystickC.hh"

namespace Smart {

/**
  Simple joystick data.
  The joystick is assumed to have an x-axis and a y-axis, and may have
  up to eight buttons.
 */
class CommJoystick
{
protected:
  SmartIDL::Joystick _js;

public:

  CommJoystick();
  CommJoystick(const SmartIDL::Joystick &js);
  virtual ~CommJoystick();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommJoystick"; };

  /// internal use only
  inline const SmartIDL::Joystick &get_idl() const { return _js; }
  /// internal use only
  inline void set_idl(const SmartIDL::Joystick &js) { _js = js; }

  /**
    Get the position of the x axis as value between -1 and +1.
   */
  inline float get_x() const { return _js.xpos * 3.05185e-5; }

  /**
    Get the position of the y axis as value between -1 and +1.
   */
  inline float get_y() const { return _js.ypos * 3.05185e-5; }

  /**
    Get the position of the x axis as value between -32767 and +32767.
   */
  inline short get_x_int() const { return _js.xpos; }

  /**
    Get the position of the y axis as value between -32767 and +32767.
   */
  inline short get_y_int() const { return _js.ypos; }

  /**
    Get the state of the i-th button.
   */
  inline bool get_button(unsigned int i) const { return _js.buttons & (1 << i); }

  /**
    Set the position of the x axis as value between -1 and 1.
   */
  inline void set_x(float p) { _js.xpos = _float2int(p); }

  /**
    Set the position of the y axis as value between -1 and 1.
   */
  inline void set_y(float p) { _js.ypos = _float2int(p); }

  /**
    Set the position of the x axis as value between -32767 and +32767.
   */
  inline void set_x_int(short p) { _js.xpos = p; }

  /**
    Set the position of the y axis as value between -32767 and +32767.
   */
  inline void set_y_int(short p) { _js.ypos = p; }

  /**
    Set the state of the i-th button.
   */
  inline void set_button(unsigned int i, bool b);

  /**
    Write information about this joystick state to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this joystick state to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);

private:

  inline int _float2int(float f) const
  {
    if(f>1)  return  32767;
    if(f<-1) return -32767;
    return int(::rint(f*32767));
  }
};

//
// Implementation
//


inline void CommJoystick::set_button(unsigned int i, bool b)
{
  if(b)
  {
    _js.buttons = _js.buttons | (1 << i);
  }
  else
  {
    _js.buttons = _js.buttons & ~(1 << i);
  }
}

inline std::ostream &operator<<(std::ostream &os, const CommJoystick &js)
{
  js.print(os);
  return os;
}

} // namespace Smart

#endif
