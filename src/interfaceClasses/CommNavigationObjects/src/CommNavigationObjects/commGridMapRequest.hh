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
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
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
// --------------------------------------------------------------------------

#ifndef _COMM_COMMNAVIGATIONOBJECTS_COMMGRIDMAPREQUEST_HH
#define _COMM_COMMNAVIGATIONOBJECTS_COMMGRIDMAPREQUEST_HH

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>


// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommNavigationObjects/gen/smartCommGridMapRequestC.hh"

namespace CommNavigationObjects
{

/**
  A class representing a grid map request
  - for requesting a longterm map just leave all variables empty -> they will be ignored
 */
class CommGridMapRequest
{
protected:
	CommNavigationObjectsIDL::CommGridMapRequest idl_CommGridMapRequest;

public:
	// initialize the map request class
	CommGridMapRequest()
	{
		idl_CommGridMapRequest.id              = 0;
		idl_CommGridMapRequest.xOffsetMM       = 0;
		idl_CommGridMapRequest.yOffsetMM       = 0;
		idl_CommGridMapRequest.xSizeMM         = 0;
		idl_CommGridMapRequest.ySizeMM         = 0;
		idl_CommGridMapRequest.curLtmState     = false;
		idl_CommGridMapRequest.curLtmThreshold = 0;

	}

	// copy constructor
	CommGridMapRequest(const CommNavigationObjectsIDL::CommGridMapRequest &obj) :
	idl_CommGridMapRequest(obj)
	{
	}

	operator CommNavigationObjectsIDL::CommGridMapRequest() const
	{
		return idl_CommGridMapRequest;
	}

	// implicitly destroy the map class
	virtual ~CommGridMapRequest()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);


	static inline std::string identifier(void)
	{
		return "CommNavigationObjects::CommGridMapRequest";
	}

	//
	// user interfaces

	/// extract all relevant parameters from the communication class of the map
	int get_parameter( unsigned int &id,
			int &xOffsetMM,
			int &yOffsetMM,
			unsigned int &xSizeMM,
			unsigned int &ySizeMM,
			bool &curLtmState,
			unsigned int &curLtmThreshold ) const;


	/// set all relevant parameters in the communication class of the map
	int set_parameter(  unsigned int id,
			int xOffsetMM,
			int yOffsetMM,
			unsigned int xSizeMM,
			unsigned int ySizeMM,
			bool curLtmState,
			unsigned int curLtmThreshold );


};

}
#endif
