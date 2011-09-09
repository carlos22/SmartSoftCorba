//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.8.0
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
//  Copyright (C) 2010 Manuel Wopfner
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

#ifndef _COMM_COMMPTUOBJECTS_COMMPTUPARAMETER_HH
#define _COMM_COMMPTUOBJECTS_COMMPTUPARAMETER_HH

#include <string>
#include <iostream>

// include files of classes

// include header-files of included communication objects

// include enums
#include <CommPTUObjects/gen/enumPTUTagType.hh>

// include client-side file generated by IDL compiler
#include "CommPTUObjects/gen/smartCommPTUParameterC.hh"

#ifndef LISP_SEPARATOR
#define LISP_SEPARATOR " ()\"\n"
#define LISP_STRING    1000
#endif


/**
 * The repository for pan tilt units communication objects.
 */
namespace CommPTUObjects {

class CommPTUParameter {
protected:
	CommPTUObjectsIDL::CommPTUParameter idl_CommPTUParameter;

public:
	CommPTUParameter() {
	}

	CommPTUParameter(const CommPTUObjectsIDL::CommPTUParameter &obj) :
		idl_CommPTUParameter(obj) {
	}

	operator CommPTUObjectsIDL::CommPTUParameter() const
	{
		return idl_CommPTUParameter;
	}

	virtual ~CommPTUParameter() {
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void) {
		return "CommPTUObjects::CommPTUParameter";
	}

	//
	// user interfaces

	/**
	 * Translate the PTUTagType enum to a string representation.
	 */
	static std::string tagToString(PTUTagType status);

	/**
	 * Returns all generic parameters.
	 */
	inline void get(PTUTagType &flag, double& p1, double& p2, double& p3, double& p4, double& p5, double& p6) const {
		flag = idl_CommPTUParameter.tag;
		p1 = idl_CommPTUParameter.parameter01;
		p2 = idl_CommPTUParameter.parameter02;
		p3 = idl_CommPTUParameter.parameter03;
		p4 = idl_CommPTUParameter.parameter04;
		p5 = idl_CommPTUParameter.parameter05;
		p6 = idl_CommPTUParameter.parameter06;
	}

	inline int set(std::string& inString) {
		// ----------------------------------------------------
		//
		// handle tag to AmclParameter
		//
		// input
		// 	 RESET
		//   PANACC ?value  [rad/sec^2]
		//	 TILTACC ?value  [rad/sec^2]
		//	 PANSPEEDLIMITS ?min ?max  [rad/sec]
		//   TILTSPEEDLIMITS ?min ?max [rad/sec]
		//   PANSPEED ?value  [rad/sec]
		//   TILTSPEED ?value  [rad/sec]
		//   PANSTARTUPSPEED ?value  [rad/sec]
		//   TILTSTARTUPSPEED ?value  [rad/sec]
		//   SENSOROFFSET ?x ?y ?z ?azimuth ?elevation ?roll [mm] and [rad]
		// output
		//   0  everything ok
		//  -1  format error occured
		// ----------------------------------------------------

		char *param = (char *) NULL;
		char *parse = (char *) NULL;
		char *input = (char *) NULL;

		int error;
		int i;

		input = (char *) inString.c_str();
		error = 0;

		do {
			param = strsep(&input, LISP_SEPARATOR);
		} while ((param != NULL) && (strlen(param) == 0));

		if (strcasecmp(param, "RESET") == 0) {
			// --------------------
			// reset the ptu
			// --------------------
			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_RESET;

		} else if (strcasecmp(param, "PANACC") == 0) {
			// ---------------------------------------
			// set pan acceleration
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_ACCELERATION_PAN;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "TILTACC") == 0) {
			// ---------------------------------------
			// set tilt acceleration
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_ACCELERATION_TILT;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "PANSPEEDLIMITS") == 0) {
			// ---------------------------------------
			// set pan speed limits
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_SPEED_LIMIT_PAN;

			parse = (char *) calloc(LISP_STRING, sizeof(char));

			for (i = 0; i < 2; i++) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));
				parse = strcat(parse, param);
				parse = strcat(parse, " ");
			}
			if (sscanf(parse, "%lf %lf", &idl_CommPTUParameter.parameter01, &idl_CommPTUParameter.parameter02) == 2) {
				error = 0;
			} else {
				error = -1;
			}
			free(parse);
		} else if (strcasecmp(param, "TILTSPEEDLIMITS") == 0) {
			// ---------------------------------------
			// set tilt speed limits
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_SPEED_LIMIT_TILT;

			parse = (char *) calloc(LISP_STRING, sizeof(char));

			for (i = 0; i < 2; i++) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));
				parse = strcat(parse, param);
				parse = strcat(parse, " ");
			}
			if (sscanf(parse, "%lf %lf", &idl_CommPTUParameter.parameter01, &idl_CommPTUParameter.parameter02) == 2) {
				error = 0;
			} else {
				error = -1;
			}
			free(parse);
		} else if (strcasecmp(param, "PANSPEED") == 0) {
			// ---------------------------------------
			// set pan speed
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_SPEED_PAN;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "TILTSPEED") == 0) {
			// ---------------------------------------
			// set pan speed
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_SPEED_TILT;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "PANSTARTUPSPEED") == 0) {
			// ---------------------------------------
			// set startup speed
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_START_UP_SPEED_PAN;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "TILTSTARTUPSPEED") == 0) {
			// ---------------------------------------
			// set startup speed
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_START_UP_SPEED_TILT;

			if (error == 0) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));

				if ((param != NULL) && (sscanf(param, "%lf", &idl_CommPTUParameter.parameter01) == 1)) {
					// everything ok
				} else {
					error = -1;
				}
			}
		} else if (strcasecmp(param, "SENSOROFFSET") == 0) {
			// ---------------------------------------
			// set sensor offset
			// ---------------------------------------

			idl_CommPTUParameter.tag = CommPTUObjectsIDL::LITERAL_SENSOR_OFFSET;

			parse = (char *) calloc(LISP_STRING, sizeof(char));

			for (i = 0; i < 6; i++) {
				do {
					param = strsep(&input, LISP_SEPARATOR);
				} while ((param != NULL) && (strlen(param) == 0));
				parse = strcat(parse, param);
				parse = strcat(parse, " ");
			}
			if (sscanf(parse, "%lf %lf %lf %lf %lf %lf", &idl_CommPTUParameter.parameter01,
					&idl_CommPTUParameter.parameter02, &idl_CommPTUParameter.parameter03,
					&idl_CommPTUParameter.parameter04, &idl_CommPTUParameter.parameter05,
					&idl_CommPTUParameter.parameter06) == 6) {
				error = 0;
			} else {
				error = -1;
			}
			free(parse);
		} else {
			// --------------------
			// got garbage
			// --------------------
			error = -1;
		}

		return error;
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

inline std::ostream &operator<<(std::ostream &os, const CommPTUParameter &comm) {
	comm.print(os);
	return os;
}

}
#endif
