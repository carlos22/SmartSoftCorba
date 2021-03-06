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


#ifndef _COMM_COMMBASICOBJECTS_COMMDIRECTION3D_HH
#define _COMM_COMMBASICOBJECTS_COMMDIRECTION3D_HH

#include <string>
#include <iostream>
#include "smartKnuthMorrisPratt.hh" //from utilits xml


// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommBasicObjects/gen/smartCommDirection3dC.hh"

namespace CommBasicObjects
{


/**
  A direction in 3d space.
  A direction is given by an azimuth angle \f$\alpha\f$ and an elevation angle \f$\epsilon\f$.
  Then, an according direction vector \f$(x,y,z)^T\f$ is given by
  \f$x=\cos\alpha\cos\epsilon\f$, \f$y=\sin\alpha\cos\epsilon\f$, and
  \f$z=\sin\epsilon\f$.
 */
class CommDirection3d
{
protected:
	CommBasicObjectsIDL::CommDirection3d idl_CommDirection3d;

public:
	CommDirection3d();

	CommDirection3d(const CommBasicObjectsIDL::CommDirection3d &obj) :
		idl_CommDirection3d(obj)
	{
	}

	operator CommBasicObjectsIDL::CommDirection3d() const
	{
		return idl_CommDirection3d;
	}

	virtual ~CommDirection3d() {
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommBasicObjects::CommDirection3d";
	}

	//
	// user interfaces
	CommDirection3d(double azimuth, double elevation);
	//CommDirection3d(const CommBasicObjectsIDL::CommDirection3d &dir);

	/**
	 Get the azimuth angle in radians.
	 */
	inline double get_azimuth() const {
		return idl_CommDirection3d.azimuth;
	}

	/**
	 Get the elevation angle in radians.
	 */
	inline double get_elevation() const {
		return idl_CommDirection3d.elevation;
	}

	/**
	 Set the azimuth angle in radians.
	 */
	inline void set_azimuth(double a) {
		idl_CommDirection3d.azimuth = a;
	}

	/**
	 Set the elevation angle in radians.
	 */
	inline void set_elevation(double e) {
		idl_CommDirection3d.elevation = e;
	}

	/**
	 Write information about this direction to the given output stream.
	 */
	void print(std::ostream &os = std::cout) const;

	/**
	 Save an XML like representation of this 3d direction to the given output stream.
	 */
	void save_xml(std::ostream &os, const std::string &indent = "") const;

	/**
	 Load from XML represenation on an istream.
	 Be warned, no error checking is done.
	 For details, see \ref smart_basic_com-xml.
	 */
	void load_xml(std::istream &is);


};


inline std::ostream &operator<<(std::ostream &os, const CommDirection3d &dir)
{
  dir.print(os);
  return os;
}


}
#endif
