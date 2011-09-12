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
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck, Matthias Lutz
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

#ifndef _COMM_COMMNAVIGATIONOBJECTS_COMMAMCLPARAMETER_HH
#define _COMM_COMMNAVIGATIONOBJECTS_COMMAMCLPARAMETER_HH

#include <string>
#include <iostream>

#ifndef LISP_SEPARATOR
#define LISP_SEPARATOR " ()\"\n"
#define LISP_STRING    1000
#endif

// include enums
#include <CommNavigationObjects/gen/enumAmclTag.hh>

// include client-side file generated by IDL compiler
#include "CommNavigationObjects/gen/smartCommAmclParameterC.hh"

namespace CommNavigationObjects
{

class CommAmclParameter
{
protected:
	CommNavigationObjectsIDL::CommAmclParameter idl_CommAmclParameter;

public:
	CommAmclParameter()
	{
	}

	CommAmclParameter(const CommNavigationObjectsIDL::CommAmclParameter &obj) :
		idl_CommAmclParameter(obj)
	{
	}

	operator CommNavigationObjectsIDL::CommAmclParameter() const
	{
		return idl_CommAmclParameter;
	}

	virtual ~CommAmclParameter()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommNavigationObjects::CommAmclParameter";
	}

	//
	// user interfaces
	//
	inline void get(AmclTag &tag, int & value1, int & value2, int & value3) const
	{
	  tag = idl_CommAmclParameter.tag;
	  value1 = idl_CommAmclParameter.parameter1;
	  value2 = idl_CommAmclParameter.parameter2;
	  value3 = idl_CommAmclParameter.parameter3;
	}


	inline int set(std::string& inString)
	{
	  // ----------------------------------------------------
	  //
	  // handle tag to AmclParameter
	  //
	  // input
	  //   GLOBALLOCALIZATION
	  //   INITIALPOSE ?x ?y ?a
	  // output
	  //   0  everything ok
	  //  -1  format error occured
	  // ----------------------------------------------------

	  char *param = (char *)NULL;
	  char *parse = (char *)NULL;
	  char *input = (char *)NULL;

	  int  error;
	  int  i;

	  input = (char *)inString.c_str();
	  error = 0;

	  do
	  {
	    param = strsep(&input,LISP_SEPARATOR);
	  } while ((param != NULL) && (strlen(param)==0));

	  if (strcasecmp(param,"GLOBALLOCALIZATION")==0)
	  {
	    // --------------------
	    // load current map from longterm map
	    // --------------------
		idl_CommAmclParameter.tag = CommNavigationObjectsIDL::LITERAL_GLOBALLOCALIZATION;

	  }
	  else if (strcasecmp(param,"INITIALPOSE")==0)
	  {
	    // ---------------------------------------
	    // set initial pose  -- x/y/a
	    // ---------------------------------------

		idl_CommAmclParameter.tag = CommNavigationObjectsIDL::LITERAL_INITIALPOSE;

	    parse = (char *)calloc(LISP_STRING,sizeof(char));

	    for (i=0; i<3; i++)
	    {
	      do
	      {
	        param = strsep(&input,LISP_SEPARATOR);
	      } while ((param != NULL) && (strlen(param)==0));
	      parse = strcat(parse,param);
	      parse = strcat(parse," ");
	    }
	    if (sscanf(parse,"%d %d %d",&idl_CommAmclParameter.parameter1,
	                                &idl_CommAmclParameter.parameter2,
	                                &idl_CommAmclParameter.parameter3) == 3)
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

};

}
#endif