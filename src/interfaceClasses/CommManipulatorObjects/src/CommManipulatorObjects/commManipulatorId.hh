//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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
//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "CommManipulatorObjects".
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
//--------------------------------------------------------------------------

#ifndef _COMM_COMMMANIPULATORID_HH
#define _COMM_COMMMANIPULATORID_HH

#include <string>

// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommManipulatorObjects/gen/smartCommManipulatorIdC.hh"

namespace CommManipulatorObjects {

class CommManipulatorId {
protected:
	CommManipulatorObjectsIDL::CommManipulatorId idl_CommManipulatorId;

public:
	CommManipulatorId() {
	}

	CommManipulatorId(const CommManipulatorObjectsIDL::CommManipulatorId &obj) :
		idl_CommManipulatorId(obj) {
	}

	operator CommManipulatorObjectsIDL::CommManipulatorId() const
	{
		return idl_CommManipulatorId;
	}

	virtual ~CommManipulatorId() {
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void) {
		return "CommManipulatorObjects::CommManipulatorId";
	}

	//
	// user interfaces

	/**
	 * Set the id.
	 */
	inline void set_id(uint32_t id) {
		idl_CommManipulatorId.id = id;
	}

	/**
	 * Get the id.
	 */
	inline uint32_t get_id() const {
		return idl_CommManipulatorId.id;
	}

	/**
	 * Set if the id is valid.
	 */
	inline void set_valid(bool valid) {
		idl_CommManipulatorId.valid = valid;
	}

	/**
	 * Get if the id is valid.
	 */
	inline bool is_valid() const {
		return idl_CommManipulatorId.valid;
	}

};

}
#endif
