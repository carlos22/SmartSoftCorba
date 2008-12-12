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

#include <iostream>

#include "commBaseState.hh"

Smart::CommTimeStamp make_ts()
{
  return Smart::CommTimeStamp::now();
}

Smart::CommBasePosition make_pos()
{
  Smart::CommBasePosition pos;
  
  pos.set_x(0.1);
  pos.set_y(0.2);
  pos.set_z(0.3);

  pos.set_base_alpha(0.4);
  pos.set_steer_alpha(0.5);
  pos.set_turret_alpha(0.6);

  pos.set_update_count(42);
  
  for(unsigned int i=0; i<3; ++i)
  for(unsigned int j=0; j<3; ++j)
  {
    pos.set_cov(i, j, 0.1*(4-(i-j)*(i-j)));
  }

  return pos;
}

Smart::CommBaseVelocity make_vel()
{
  Smart::CommBaseVelocity vel;
  vel.set_v(0.1);
  vel.set_omega_base(0.2);
  vel.set_omega_steer(0.3);
  vel.set_omega_turret(0.4);
  return vel;
}

int main()
{
  Smart::CommBaseState bs;
  bs.set_time_stamp(make_ts());
  bs.set_base_position(make_pos());
  bs.set_base_velocity(make_vel());
  
  std::cout << "bs  = " << bs << std::endl;
  
  CORBA::Any a;
  bs.get(a);
  
  Smart::CommBaseState bs2;
  bs2.set(a);
  
  std::cout << "bs2 = " << bs2 << std::endl;

  return 0;
}
