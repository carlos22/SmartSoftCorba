//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.2
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
//  Copyright (C) 2010 Andreas Steck, Matthias Lutz
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

#include "commSimpleObjectEventResult.hh"

#include <sstream>

using namespace CommObjectRecognitionObjects;

void CommSimpleObjectEventResult::get(CORBA::Any &a) const
{
	a <<= idl_CommSimpleObjectEventResult;
}

void CommSimpleObjectEventResult::set(const CORBA::Any &a)
{
	CommObjectRecognitionObjectsIDL::CommSimpleObjectEventResult *tmp_cmd;
	if (a >>= tmp_cmd)
	{
		idl_CommSimpleObjectEventResult = *tmp_cmd;
	}
}


void CommSimpleObjectEventResult::set(SimpleObjectState s, int x, int y)
{
	idl_CommSimpleObjectEventResult.state = s.get_value_idl();
	idl_CommSimpleObjectEventResult.x = x;
	idl_CommSimpleObjectEventResult.y = y;
}

void CommSimpleObjectEventResult::get(SimpleObjectState& s, int& x, int& y) const
{
  s = idl_CommSimpleObjectEventResult.state;
  x = idl_CommSimpleObjectEventResult.x;
  y = idl_CommSimpleObjectEventResult.y;
}

void CommSimpleObjectEventResult::get(char* r)const
{
  switch(idl_CommSimpleObjectEventResult.state) {
    case SimpleObjectState::OBJECT_FOUND:
    {
      std::stringstream strs;
      strs.str("");
      strs << "(object-found (" << idl_CommSimpleObjectEventResult.x << " " << idl_CommSimpleObjectEventResult.y << "))";
      std::string str = strs.str();
      strcpy(r, str.c_str());
      break;
    }
    case SimpleObjectState::OBJECT_NOT_FOUND:
    {
        strcpy(r,"(object-not-found)");
        break;
    }
    default:
      strcpy(r,"(error)");
      break;
  }
}

void CommSimpleObjectEventResult::print(std::ostream &os) const
{
  os << "Result : " << idl_CommSimpleObjectEventResult.state << std::endl;
}



