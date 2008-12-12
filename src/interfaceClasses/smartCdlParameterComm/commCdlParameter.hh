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

#ifndef SMART_COMM_CDL_PARAMETER_HH
#define SMART_COMM_CDL_PARAMETER_HH

#include <string>
#include <iostream>

#include "smartCdlParameterC.hh"

#define LISP_SEPARATOR " ()\n"
#define LISP_STRING    1000

namespace Smart {

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

typedef enum 
{
  CDL_SET_MODE_STRATEGY,    //
  CDL_SET_FREE_BEHAVIOR,    // select behavior to free robot when stalled
  CDL_SET_LOOKUP_TABLE,     // select the lookup table to be used
  CDL_SET_TRANS_VELOCITY,   // set min, max trans velocity [mm/s,mm/s]
  CDL_SET_ROT_VELOCITY,     // set min, max rot velocity [deg/s,deg/s]
//  CDL_MODE_GOAL,            //
//  CDL_SET_ID,               // set the id used for synchronisation
//  CDL_SET_GOAL,             // set goal x,y,heading [mm,mm,deg]
//  CDL_SET_APPROACH_DIST,    // set the minimum distance to stop at goal [mm]
//  CDL_SET_IGNORE_REGION,    // set circular region where scan points are ignored
  
  CDL_NEUTRAL,              // indicate that no selection has been made
  
  CDL_REACTIVE,             // Strategy
  CDL_JOYSTICK,             // Strategy
  CDL_TURN,                 // Strategy
  
  CDL_ABSOLUTE,             // goal specification
  CDL_PLANNER,              // goal specification
  
  CDL_DEFAULT_LOOKUP,       // lookup table
  CDL_SECOND_LOOKUP,        // lookup table
    
  CDL_NO_FREE_BEHAVIOR,     // free behavior
  CDL_FREE_BEHAVIOR         // free behavior

} CdlTagType;


/** 
  A class representing the parameters to control the CdlServer.
 */
class CommCdlParameter
{
protected:
  SmartIDL::CdlParameter cmd;

public:

  CommCdlParameter();
  virtual ~CommCdlParameter();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommCdlParameter"; };

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
  tag = (CdlTagType)cmd.tag;
  value1 = cmd.parameter1;
  value2 = cmd.parameter2;
  value3 = cmd.parameter3;
  value4 = cmd.parameter4;
  value5 = cmd.parameter5;
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

// TODO
//  //   ID ?id
//  //   APPROACHDIST ?d
//  //   IGNOREREGION ?x ?y ?r
//  //   GOALREGION ?x ?y ?h     mm/mm/degree
//  //   GOAL ABSOLUTE/PLANNER 
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
    cmd.tag = CDL_SET_MODE_STRATEGY;

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
        cmd.parameter1 = CDL_REACTIVE;
      }
      else if(strcasecmp("JOYSTICK",param)==0)
      {
        cmd.parameter1 = CDL_JOYSTICK;
      }
      else if(strcasecmp("TURN",param)==0)
      {
        cmd.parameter1 = CDL_TURN;
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
    cmd.tag = CDL_SET_FREE_BEHAVIOR;

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
        cmd.parameter1 = CDL_FREE_BEHAVIOR;
      }
      else if(strcasecmp("DEACTIVATE",param)==0)
      {
        cmd.parameter1 = CDL_NO_FREE_BEHAVIOR;
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
    cmd.tag = CDL_SET_LOOKUP_TABLE;

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
        cmd.parameter1 = CDL_DEFAULT_LOOKUP;
      } 
      else if(strcasecmp("SECOND",param)==0) 
      {
        cmd.parameter1 = CDL_SECOND_LOOKUP;
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
    cmd.tag = CDL_SET_TRANS_VELOCITY;
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
    if (sscanf(parse,"%d %d",&cmd.parameter1,
                             &cmd.parameter2) == 2)
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
    cmd.tag = CDL_SET_ROT_VELOCITY;
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
    if (sscanf(parse,"%d %d",&cmd.parameter1,
                             &cmd.parameter2) == 2)
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

} // namespace Smart

#endif
