//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

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



#include "commBaseState.hh"

using namespace CommBasicObjects;

void CommBaseState::get(CORBA::Any &a) const
{
	a <<= idl_CommBaseState;
}

void CommBaseState::set(const CORBA::Any &a)
{
	CommBasicObjectsIDL::CommBaseState *tmp_cmd;
	if (a >>= tmp_cmd)
	{
		idl_CommBaseState = *tmp_cmd;
	}
}


CommBaseState CommBaseState::extrapolate(const CommTimeStamp &ts) const
{
	CommBasicObjectsIDL::CommBaseState extrapolated_state;
  extrapolated_state.time = ts;
  extrapolated_state.base_velocity = idl_CommBaseState.base_velocity;
  extrapolated_state.base_position.z = idl_CommBaseState.base_position.z;

  const double dt = CommTimeStamp(idl_CommBaseState.time).seconds_before(ts);

  extrapolated_state.base_position.a_base   = idl_CommBaseState.base_position.a_base   + dt * idl_CommBaseState.base_velocity.omega_base;
  extrapolated_state.base_position.a_steer  = idl_CommBaseState.base_position.a_steer  + dt * idl_CommBaseState.base_velocity.omega_steer;
  extrapolated_state.base_position.a_turret = idl_CommBaseState.base_position.a_turret + dt * idl_CommBaseState.base_velocity.omega_turret;

  const double sin_steer = sin(idl_CommBaseState.base_position.a_steer);
  const double cos_steer = cos(idl_CommBaseState.base_position.a_steer);

  if(fabs(idl_CommBaseState.base_velocity.omega_steer)<1e-6)
  {
    // omega too small, assume only linear motion
    const double dx = idl_CommBaseState.base_velocity.v * dt * cos_steer;
    const double dy = idl_CommBaseState.base_velocity.v * dt * sin_steer;
    extrapolated_state.base_position.x = idl_CommBaseState.base_position.x + dx;
    extrapolated_state.base_position.y = idl_CommBaseState.base_position.y + dy;
  }
  else
  {
    // rotate the position by da radians around the center (cx,cy) of the circular motion
    const double da = idl_CommBaseState.base_velocity.omega_steer * dt;
    const double cos_da = cos(da);
    const double sin_da = sin(da);
    const double r = idl_CommBaseState.base_velocity.v / idl_CommBaseState.base_velocity.omega_steer;
    const double cx = idl_CommBaseState.base_position.x - r * sin_steer;
    const double cy = idl_CommBaseState.base_position.y + r * cos_steer;
    const double xrel = idl_CommBaseState.base_position.x - cx;
    const double yrel = idl_CommBaseState.base_position.y - cy;
    extrapolated_state.base_position.x = cx + xrel * cos_da - yrel * sin_da;
    extrapolated_state.base_position.y = cy + xrel * sin_da + yrel * cos_da;
  }
  return CommBaseState(extrapolated_state);
}

void CommBaseState::print(std::ostream &os) const
{
  os << "CommBaseState(time=" << get_time_stamp()
     << ",pos=" << get_base_position()
     << ",vel=" << get_base_velocity() << ")";
}

void CommBaseState::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<base_state>" << std::endl;
  get_time_stamp().save_xml(os, indent + "  ");
  get_base_position().save_xml(os, indent + "  ");
  get_base_velocity().save_xml(os, indent + "  ");
  os << indent << "</base_state>" << std::endl;
}

void CommBaseState::load_xml(std::istream &is)
{
  static const Smart::KnuthMorrisPratt kmp_begin("<base_state>");
  static const Smart::KnuthMorrisPratt kmp_end("</base_state>");

  kmp_begin.search(is);

  CommTimeStamp ts;
  CommBasePosition bp;
  CommBaseVelocity bv;

  ts.load_xml(is);
  bp.load_xml(is);
  bv.load_xml(is);

  set_time_stamp(ts);
  set_base_position(bp);
  set_base_velocity(bv);

  kmp_end.search(is);
}
