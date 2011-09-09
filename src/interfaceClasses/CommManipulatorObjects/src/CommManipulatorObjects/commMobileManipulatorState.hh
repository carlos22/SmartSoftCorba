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
// If you want the workflow to re-generate this file, please
// delete it before running the workflow.
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

#ifndef _COMM_COMMMOBILEMANIPULATORSTATE_HH
#define _COMM_COMMMOBILEMANIPULATORSTATE_HH

#include <string>

// include header-files of includes communication objects
#include <CommBasicObjects/commBaseState.hh>
#include <CommManipulatorObjects/commManipulatorState.hh>

// include client-side file generated by IDL compiler
#include "CommManipulatorObjects/gen/smartCommMobileManipulatorStateC.hh"

namespace CommManipulatorObjects {

class CommMobileManipulatorState {
protected:
	CommManipulatorObjectsIDL::CommMobileManipulatorState idl_CommMobileManipulatorState;

public:
	CommMobileManipulatorState() {
	}

	CommMobileManipulatorState(const CommManipulatorObjectsIDL::CommMobileManipulatorState &obj) :
		idl_CommMobileManipulatorState(obj) {
	}

	operator CommManipulatorObjectsIDL::CommMobileManipulatorState() const
	{
		return idl_CommMobileManipulatorState;
	}

	virtual ~CommMobileManipulatorState() {
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void) {
		return "CommManipulatorObjects::CommMobileManipulatorState";
	}

	//////////////////////
	// user interfaces
	//////////////////////

	/**
	 * Set the state of the base.
	 */
	inline void set_base_state(const CommBasicObjects::CommBaseState &baseState) {
		idl_CommMobileManipulatorState.base_state = baseState;
	}

	/**
	 * Get the state of the base.
	 */
	inline CommBasicObjects::CommBaseState get_base_state() const {
		return idl_CommMobileManipulatorState.base_state;
	}

	/**
	 * Set the state of the manipulator.
	 */
	inline void set_manipulator_state(const CommManipulatorState &manipState) {
		idl_CommMobileManipulatorState.manipulator_state = manipState;
	}

	/**
	 * Get the state of the manipulator.
	 */
	inline CommManipulatorState get_manipulator_state() const {
		return 	idl_CommMobileManipulatorState.manipulator_state;
	}

	/**
	 * Set if the state is valid.
	 */
	inline void set_valid(bool valid) {
		idl_CommMobileManipulatorState.manipulator_state.isValid = valid;
	}

	/**
	 * Get if the state is valid.
	 */
	inline bool is_valid() const {
		return idl_CommMobileManipulatorState.manipulator_state.isValid;
	}

	/**
	 * Print the object to an output stream.
	 * @param os Output stream to which should be printed
	 */
	void print(std::ostream &os = std::cout) const;

};

//
// Implementation
//

inline std::ostream &operator<<(std::ostream &os,
		const CommMobileManipulatorState &state) {
	state.print(os);
	return os;
}

} // namespace Smart
#endif
