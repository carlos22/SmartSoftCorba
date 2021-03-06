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

#ifndef _COMM_COMMNAVIGATIONOBJECTS_COMMCDLPARAMETER_HH
#define _COMM_COMMNAVIGATIONOBJECTS_COMMCDLPARAMETER_HH

#include <string>
#include <iostream>

// include enums
#include <CommNavigationObjects/gen/enumCdlTagType.hh>

// include client-side file generated by IDL compiler
#include "CommNavigationObjects/gen/smartCommCdlParameterC.hh"

#ifndef LISP_SEPARATOR
#define LISP_SEPARATOR " ()\"\n"
#define LISP_STRING    1000
#endif

// ------------------------------------------------------------------
// tags for communicating with the planner are defined in enumCdlTagType.hh
// ------------------------------------------------------------------

/**
 * The repository for navigation objects.
 */
namespace CommNavigationObjects
{

/**
  A class representing the parameters to control the CdlServer.
 */
class CommCdlParameter
{
protected:
	CommNavigationObjectsIDL::CommCdlParameter idl_CommCdlParameter;

public:
	CommCdlParameter()
	{
		//idl_CommCdlParameter.tag = 0;
		idl_CommCdlParameter.parameter1 = 0;
		idl_CommCdlParameter.parameter2 = 0;
		idl_CommCdlParameter.parameter3 = 0;
		idl_CommCdlParameter.parameter4 = 0;
		idl_CommCdlParameter.parameter5 = 0;
	}

	CommCdlParameter(const CommNavigationObjectsIDL::CommCdlParameter &obj) :
		idl_CommCdlParameter(obj)
	{
	}

	operator CommNavigationObjectsIDL::CommCdlParameter() const
	{
		return idl_CommCdlParameter;
	}

	virtual ~CommCdlParameter()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommNavigationObjects::CommCdlParameter";
	}

	//
	// user interfaces
	/// extract all relevant parameters from the communication class of the cdl parameter class
	void get(CdlTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const;

	/// sets all relevant parameters to the communication class of the cdl parameter class
	int set(std::string& inString);
};

//
// Implementation
//


inline void CommCdlParameter::get(CdlTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const
{
  tag = idl_CommCdlParameter.tag;
  value1 = idl_CommCdlParameter.parameter1;
  value2 = idl_CommCdlParameter.parameter2;
  value3 = idl_CommCdlParameter.parameter3;
  value4 = idl_CommCdlParameter.parameter4;
  value5 = idl_CommCdlParameter.parameter5;
}

inline int CommCdlParameter::set(std::string& inString)
{
  // ----------------------------------------------------
  //
  // handle command to curvature distance lookup
  //
  // input
  //   STRATEGY REACTIVE/JOYSTICK
  //   FREEBEHAVIOR ACTIVATE/DEACTIVATE
  //   LOOKUPTABLE DEFAULT/SECOND
  //   TRANSVEL ?vmin ?vmax (mm/sec)
  //   ROTVEL ?wmin ?wmax (deg/sec)
  //   GOALMODE ABSOLUTE/PLANNER
  //   GOALREGION ?x ?y ?h mm/mm/degree
  //   APPROACHDIST ?d
  //   ID ?id
// TODO
//  //   IGNOREREGION ?x ?y ?r
//  //
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


  if (strcasecmp(param,"STRATEGY")==0)
  {
    // --------------------
    // set cdl strategy
    // --------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_MODE_STRATEGY;

    //
    // get the first parameter
    //
    if (error == 0)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("REACTIVE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_REACTIVE;
      }
      else if(strcasecmp("JOYSTICK",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_JOYSTICK;
      }
      else if(strcasecmp("TURN",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_TURN;
      }
      else if(strcasecmp("APPROACH_FLAT_SURF",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_APPROACH_FLAT_SURF;
      }
      else if(strcasecmp("APPROACH_HALT",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_APPROACH_HALT;
      }
      else if(strcasecmp("APPROACH_COVERAGE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_APPROACH_COVERAGE;
      }
      else if(strcasecmp("APPROACH",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_APPROACH;
      }
      else if(strcasecmp("ROTATE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_ROTATE;
      }
      else if(strcasecmp("FOLLOW",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_FOLLOW;
      }
      else if(strcasecmp("BACKWARD",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_BACKWARD;
      }
      else
      {
        error = -1;
      }
    }
  }
  else if (strcasecmp(param,"FREEBEHAVIOR")==0)
  {
    // --------------------
    // set free behavior
    // --------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_FREE_BEHAVIOR;

    //
    // get the first parameter
    //
    if (error == 0)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("ACTIVATE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_FREE_BEHAVIOR;
      }
      else if(strcasecmp("DEACTIVATE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_NO_FREE_BEHAVIOR;
      }
      else
      {
        error = -1;
      }
    }
  }
  else if (strcasecmp(param,"LOOKUPTABLE")==0)
  {
    // ----------------------------------------------------------------
    // set lookup table for cdl (robot parameters: speed, shape, ...)
    // ----------------------------------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_LOOKUP_TABLE;

    //
    // get the first parameter
    //
    if (error == 0)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("DEFAULT",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_DEFAULT_LOOKUP;
      }
      else if(strcasecmp("SECOND",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_SECOND_LOOKUP;
      }
      else
      {
        error = -1;
      }
    }

  }
  else if (strcasecmp(param,"TRANSVEL")==0)
  {
    // ---------------------------------------
    // set translation velocity  -- vmin/vmax
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_TRANS_VELOCITY;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<2; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d %d",&idl_CommCdlParameter.parameter1,
                             &idl_CommCdlParameter.parameter2) == 2)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"ROTVEL")==0)
  {
    // ---------------------------------------
    // set rotational velocity  -- wmin/wmax
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_ROT_VELOCITY;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<2; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d %d",&idl_CommCdlParameter.parameter1,
                             &idl_CommCdlParameter.parameter2) == 2)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"GOALMODE")==0)
  {
    // ---------------------------------------
    // set goal type -- ABSOLUTE/PLANNER
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_MODE_GOAL;

    //
    // get the first parameter
    //
    if (error == 0)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("ABSOLUTE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_ABSOLUTE;
      }
      else if(strcasecmp("PLANNER",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_PLANNER;
      }
      else if(strcasecmp("PERSON",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_PERSON;
      }
      else if(strcasecmp("SAVED",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_SAVED;
      }
      else if(strcasecmp("ANGLEABSOLUT",param)==0 || strcasecmp("ANGLEABSOLUTE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_ANGLE_ABSOLUTE;
      }
      else if(strcasecmp("ANGLERELATIVE",param)==0)
      {
    	  idl_CommCdlParameter.parameter1 = CommNavigationObjectsIDL::LITERAL_CDL_ANGLE_RELATIVE;
      }

      else
      {
        error = -1;
      }
    }
  }
  else if (strcasecmp(param,"ID")==0)
  {
    // ---------------------------------------
    // set cdl goal id  -- id
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_ID;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<1; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d",&idl_CommCdlParameter.parameter1) == 1)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"GOALREGION")==0)
  {
    // ---------------------------------------
    // set goal  -- x/y/a
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_GOAL;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<4; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d %d %d %d",&idl_CommCdlParameter.parameter1,
                                &idl_CommCdlParameter.parameter2,
                                &idl_CommCdlParameter.parameter3,
				&idl_CommCdlParameter.parameter4) == 4)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"APPROACHDIST")==0)
  {
    // ---------------------------------------
    // set goal approach distance  -- dist
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_APPROACH_DIST;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<1; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d",&idl_CommCdlParameter.parameter1) == 1)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"SAVECURPOS")==0)
  {
    // ---------------------------------------
    // set goal approach distance  -- dist
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SAVE_CURRENT_POS;

    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<1; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d",&idl_CommCdlParameter.parameter1) == 1)
    {
      error = 0;
    }
    else
    {
      error = -1;
    }
    free(parse);
  }
  else if (strcasecmp(param,"SAFETYCL")==0)
  {
    // ---------------------------------------
    // set global cdl safety clearance -- dist
    // ---------------------------------------
	  idl_CommCdlParameter.tag = CommNavigationObjectsIDL::LITERAL_CDL_SET_SAFETY_CLEARANCE;
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<1; i++)
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d",&idl_CommCdlParameter.parameter1) == 1)
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

} // end namespace
#endif
