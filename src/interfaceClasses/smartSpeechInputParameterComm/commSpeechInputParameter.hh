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

#ifndef SMART_COMM_SPEECH_INPUT_PARAMETER_HH
#define SMART_COMM_SPEECH_INPUT_PARAMETER_HH

#include <string>
#include <iostream>

#include "smartSpeechInputParameterC.hh"

#define LISP_SEPARATOR " ()\n"
#define LISP_STRING    1000

namespace Smart {

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

typedef enum 
{
  SPEECH_SET_GRAMMER,    //
  
} SpeechInputTagType;


/** 
  A class representing the parameters to control the CdlServer.
 */
class CommSpeechInputParameter
{
protected:
  SmartIDL::SpeechInputParameter cmd;

public:

  CommSpeechInputParameter();
  virtual ~CommSpeechInputParameter();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommSpeechInputParameter"; };

  /// extract all relevant parameters from the communication class of the cdl parameter class
  void get(SpeechInputTagType &tag,std::string &value1) const; 

  /// sets all relevant parameters to the communication class of the cdl parameter class
  int set(std::string& inString);
};

//
// Implementation
//


inline void CommSpeechInputParameter::get(SpeechInputTagType &tag,std::string &value1) const 
{
  tag = (SpeechInputTagType)cmd.tag;
  value1 = cmd.parameter1;
}
 
inline int CommSpeechInputParameter::set(std::string& inString)
{
  // ----------------------------------------------------
  //
  // handle command to curvature distance lookup
  //
  // input
  //   SETGRAMMER ?grammer_file
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


  if (strcasecmp(param,"SETGRAMMER")==0)
  {
    // ---------------------------------------
    // set global cdl safety clearance -- dist
    // ---------------------------------------
    cmd.tag = SPEECH_SET_GRAMMER;
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

    char* tmp = (char*)NULL;
    tmp = (char *)calloc(LISP_STRING,sizeof(char));

    if (sscanf(parse,"%s",tmp) == 1)
    {
      error = 0;
      std::string str(tmp);

      cmd.parameter1 = str.c_str();
    }
    else
    {
      error = -1;
    }
    free(parse);
    free(tmp);
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
