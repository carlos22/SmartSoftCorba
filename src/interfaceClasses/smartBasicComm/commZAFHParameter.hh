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

#ifndef SMART_COMM_ZAFH_PARAMETER_HH
#define SMART_COMM_ZAFH_PARAMETER_HH

#include <string>
#include <iostream>

#include "smartZAFHParameterC.hh"

#define LISP_SEPARATOR " ()\n"
#define LISP_STRING    1000

namespace Smart {

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

typedef enum {
  PLANNER_DELETE_GOAL,			// delete all specified goals
  PLANNER_SET_DESTINATION_LINE,		// send new goal line (x1,y1,x2,y2)
  PLANNER_SET_DESTINATION_CIRCLE,   	// send new goal circle (x1,y1,r)
  PLANNER_ID				// for synchronization purposes
} PlannerTagType;


class CommZAFHParameter
{
protected:
  SmartIDL::ZAFHParameter cmd;

public:

  CommZAFHParameter();
  virtual ~CommZAFHParameter();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommZAFHParameter"; };

  void get(PlannerTagType &tag,int &value1,int &value2,int &value3,int &value4) const; 
  int set(std::string& inString);
};

//
// Implementation
//


inline void CommZAFHParameter::get(PlannerTagType &tag,int &value1,int &value2,int &value3,int &value4) const 
{
  tag = (PlannerTagType)cmd.command;
  value1 = cmd.parameter1;
  value2 = cmd.parameter2;
  value3 = cmd.parameter3;
  value4 = cmd.parameter4;
}
 
inline int CommZAFHParameter::set(std::string& inString)
{
  // ----------------------------------------------------
  //
  // handle command to path planner
  //
  // input
  //   ID ?id
  //   DELETEGOAL
  //   SETDESTINATIONLINE ?x1 ?y1 ?x2 ?y2
  //   SETDESTINATIONCIRCLE ?x ?y ?r
  // output
  //   0  everything ok
  //  -1  format error occured
  //
  // ----------------------------------------------------

  char *param = (char *)NULL;
  char *parse = (char *)NULL;
  char *input = (char *)NULL;

  int  error;
  int  i;

  input = (char *)inString.c_str();
  error = 0;

  do {
    param = strsep(&input,LISP_SEPARATOR);
  } while ((param != NULL) && (strlen(param)==0));

  if (strcasecmp(param,"ID")==0) {
    // --------------------
    // set id
    // --------------------
    do {
      param = strsep(&input,LISP_SEPARATOR);
    } while ((param != NULL) && (strlen(param)==0));
    if (sscanf(param,"%d",&cmd.parameter1)==1) {
      cmd.command = PLANNER_ID;
      error       = 0;
    } else {
      error = -1;
    }
  } else if (strcasecmp(param,"DELETEGOAL")==0) {
    // --------------------
    // delete all specified goals in path planner
    // --------------------
    cmd.command = PLANNER_DELETE_GOAL;
    error       = 0;
  } else if (strcasecmp(param,"SETDESTINATIONLINE")==0) {
    // --------------------
    // set destination line in path planner
    // --------------------
    parse = (char *)calloc(LISP_STRING,sizeof(char));
    for (i=0; i<4; i++) {
      do {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d %d %d %d",&cmd.parameter1,
                                   &cmd.parameter2,
                                   &cmd.parameter3,
                                   &cmd.parameter4)==4) {
      cmd.command = PLANNER_SET_DESTINATION_LINE;
      error       = 0;
    } else {
      error = -1;
    }
    free(parse);
  } else if (strcasecmp(param,"SETDESTINATIONCIRCLE")==0) {
    // --------------------
    // set destination circle in path planner
    // --------------------
    std::cout << "start\n";
    parse = (char *)calloc(LISP_STRING,sizeof(char));

    for (i=0; i<3; i++) {
      do {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      printf("nach while - parse = %s;  param = %s\n", parse, param);
      parse = strcat(parse,param);
      parse = strcat(parse," ");
      printf("parse = %s\n",  parse);
    }
    if (sscanf(parse,"%d %d %d",&cmd.parameter1,
                                &cmd.parameter2,
                                &cmd.parameter3)==3) {
      cmd.command = PLANNER_SET_DESTINATION_CIRCLE;
      error       = 0;
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

} // namespace Smart

#endif
