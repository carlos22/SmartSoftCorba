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

#include <unistd.h>
#include <iostream>

#include "commTimeStamp.hh"

int main()
{
  Smart::CommTimeStamp ts;
  
  ts.set_now();

  std::cout << "ts  = " << ts << std::endl;
  
  CORBA::Any a;
  ts.get(a);
  
  Smart::CommTimeStamp ts2;
  ts2.set(a);
  
  std::cout << "ts2 = " << ts2 << std::endl;

  std::cout << "sleep 2..." << std::endl;
  sleep(2);
  
  ts2.set_now();
  
  std::cout << "now:" << std::endl << "ts2 = " << ts2 << std::endl;
  
  std::cout << "ts.seconds_before(ts2) = " << ts.seconds_before(ts2) << std::endl;
  
  return 0;
}
