// --------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
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
//
// --------------------------------------------------------------------------


#include "smartSys.hh"

namespace CHS {

  std::ostream & operator<<(std::ostream & o, const ACE_Time_Value & ts)
  {
    time_t sec = ts.sec();
    struct tm * t = ACE_OS::localtime( &sec );
    char s[40];
    ACE_OS::strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", t);
    return o << s << "." << std::setfill('0') << std::setw(6) << ts.usec();
  }

  std::ostream & operator<<(std::ostream & o, const CORBA::Exception & e)
  {
    return o << e._name () << " (" << e._rep_id () << ')';
  }

}

