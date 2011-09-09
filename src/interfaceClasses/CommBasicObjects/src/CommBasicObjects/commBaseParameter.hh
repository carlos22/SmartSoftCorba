
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

#ifndef _COMM_COMMBASICOBJECTS_COMMBASEPARAMETER_HH
#define _COMM_COMMBASICOBJECTS_COMMBASEPARAMETER_HH

#include <string>
#include <iostream>

// include files of classes

// include header-files of included communication objects

// include enums
#include <CommBasicObjects/gen/enumBaseTagType.hh>


// include client-side file generated by IDL compiler
#include "CommBasicObjects/gen/smartCommBaseParameterC.hh"

#ifndef LISP_SEPARATOR
#define LISP_SEPARATOR " ()\"\n"
#define LISP_STRING    1000
#endif

/**
 * The repository for basic communication objects that may be used in every robot application.
 */
namespace CommBasicObjects
{

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

//typedef enum {
//  BASE_RESET
//} BaseTagType;


/**
  A class representing the parameters to control the Mapper (GridMap).
 */
class CommBaseParameter
{
protected:
	CommBasicObjectsIDL::CommBaseParameter idl_CommBaseParameter;

public:
	CommBaseParameter()
	{
		//idl_CommBaseParameter.tag = 0;
		idl_CommBaseParameter.parameter1 = 0;
		idl_CommBaseParameter.parameter2 = 0;
		idl_CommBaseParameter.parameter3 = 0;
		idl_CommBaseParameter.parameter4 = 0;
		idl_CommBaseParameter.parameter5 = 0;
	}

	CommBaseParameter(const CommBasicObjectsIDL::CommBaseParameter &obj) :
		idl_CommBaseParameter(obj)
	{
	}

	operator CommBasicObjectsIDL::CommBaseParameter() const
	{
		return idl_CommBaseParameter;
	}

	virtual ~CommBaseParameter()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommBasicObjects::CommBaseParameter";
	}

	//
	// user interfaces
	void get(CommBasicObjects::BaseTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const;
	int set(const std::string& inString);

};

//
// Implementation
//


inline void CommBaseParameter::get(CommBasicObjects::BaseTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const
{
  tag = (CommBasicObjects::BaseTagType)idl_CommBaseParameter.tag;
  value1 = idl_CommBaseParameter.parameter1;
  value2 = idl_CommBaseParameter.parameter2;
  value3 = idl_CommBaseParameter.parameter3;
  value4 = idl_CommBaseParameter.parameter4;
  value5 = idl_CommBaseParameter.parameter5;
}

inline int CommBaseParameter::set(const std::string& inString)
{
  char *param = (char *)NULL;
  char *parse = (char *)NULL;
  char *input = (char *)NULL;

  int  error;

  input = (char *)inString.c_str();
  error = 0;

  do
  {
    param = strsep(&input,LISP_SEPARATOR);
  } while ((param != NULL) && (strlen(param)==0));


  if (strcasecmp(param,"RESET")==0)
  {
	  // --------------------
	  // set parameter RESET
	  // --------------------
	  idl_CommBaseParameter.tag = CommBasicObjectsIDL::LITERAL_BASE_RESET;
  }
  else if (strcasecmp(param,"SONAR")==0)
  {
	  // --------------------
	  // set parameter SONAR
	  // --------------------
	  idl_CommBaseParameter.tag = CommBasicObjectsIDL::LITERAL_BASE_SONAR;

          parse = (char *)calloc(LISP_STRING,sizeof(char));
	  do
	  {
		  param = strsep(&input,LISP_SEPARATOR);
	  } while ((param != NULL) && (strlen(param)==0));
	  parse = strcat(parse,param);
	  parse = strcat(parse," ");

	  if (sscanf(parse,"%d",&idl_CommBaseParameter.parameter1) == 1)
	{
		  error = 0;
	}
	  else
	  {
		  error = -1;
	  }
	  free(parse);

  }
  else
  {
    // --------------------
    // got garbage
    // --------------------
    error = -1;
  }

  return error;
}


}
#endif
