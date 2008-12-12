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

#include "commBasePosition.hh"

int main()
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

  std::cout << "pos  = " << pos << std::endl;
  
  CORBA::Any a;
  pos.get(a);
  
  Smart::CommBasePosition pos2;
  pos2.set(a);
  
  std::cout << "pos2 = " << pos2 << std::endl;
  
  return 0;
}
