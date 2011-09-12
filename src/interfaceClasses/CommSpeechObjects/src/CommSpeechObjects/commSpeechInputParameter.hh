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

#ifndef _COMM_COMMSPEECHOBJECTS_COMMSPEECHINPUTPARAMETER_HH
#define _COMM_COMMSPEECHOBJECTS_COMMSPEECHINPUTPARAMETER_HH

#ifndef LISP_SEPARATOR
#define LISP_SEPARATOR " ()\"\n"
#define LISP_STRING    1000
#endif

#include <string>

// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler
#include "CommSpeechObjects/gen/smartCommSpeechInputParameterC.hh"
#include "CommSpeechObjects/gen/enumSpeechInputTagType.hh"


/**
 * The repository for speech objects.
 */
namespace CommSpeechObjects
{

class CommSpeechInputParameter
{
protected:
	CommSpeechObjectsIDL::CommSpeechInputParameter idl_CommSpeechInputParameter;

public:
	CommSpeechInputParameter()
	{
		  //idl_CommSpeechInputParameter.tag = 0;
		  idl_CommSpeechInputParameter.parameter1 = "";
	}

	CommSpeechInputParameter(
			const CommSpeechObjectsIDL::CommSpeechInputParameter &obj) :
		idl_CommSpeechInputParameter(obj)
	{
	}

	operator CommSpeechObjectsIDL::CommSpeechInputParameter() const
	{
		return idl_CommSpeechInputParameter;
	}

	virtual ~CommSpeechInputParameter()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);


	static inline std::string identifier(void)
	{
		return "CommSpeechObjects::CommSpeechInputParameter";
	}

	//
	// user interfaces
	/// extract all relevant parameters from the communication class of the cdl parameter class
	void get(CommSpeechObjects::SpeechInputTagType &tag,std::string &value1) const;

	/// sets all relevant parameters to the communication class of the cdl parameter class
	int set(std::string& inString);

};

inline void CommSpeechInputParameter::get(CommSpeechObjects::SpeechInputTagType &tag,std::string &value1) const
{
  tag = (SpeechInputTagType)idl_CommSpeechInputParameter.tag;
  value1 = idl_CommSpeechInputParameter.parameter1;
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


  if (
		  strcasecmp(param,"SETGRAMMER")==0 || // keep GRAMMER for compatibility even if spelling error
		  strcasecmp(param,"SETGRAMMAR")==0)
  {
    // ---------------------------------------
    // set global cdl safety clearance -- dist
    // ---------------------------------------
	  idl_CommSpeechInputParameter.tag = CommSpeechObjectsIDL::LITERAL_SPEECH_SET_GRAMMAR;
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

      idl_CommSpeechInputParameter.parameter1 = str.c_str();
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

}
#endif