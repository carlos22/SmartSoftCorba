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

#ifndef SMART_COMM_BASEPARAMETER_HH
#define SMART_COMM_BASEPARAMETER_HH

#include <string>
#include <iostream>

#include "smartBaseParameterC.hh"

#define LISP_SEPARATOR " ()\n"
#define LISP_STRING    1000

namespace Smart {

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

typedef enum {
  BASE_RESET
} BaseTagType;


/** 
  A class representing the parameters to control the Mapper (GridMap).
 */
class CommBaseParameter
{
protected:
  SmartIDL::BaseParameter cmd;

public:

  CommBaseParameter();
  virtual ~CommBaseParameter();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommBaseParameter"; };

  void get(BaseTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const; 

  int set(const std::string& inString);
};

//
// Implementation
//


inline void CommBaseParameter::get(BaseTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const 
{
  tag = (BaseTagType)cmd.tag;
  value1 = cmd.parameter1;
  value2 = cmd.parameter2;
  value3 = cmd.parameter3;
  value4 = cmd.parameter4;
  value5 = cmd.parameter5;
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
    parse = (char *)calloc(LISP_STRING,sizeof(char));
    cmd.tag = BASE_RESET;
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
