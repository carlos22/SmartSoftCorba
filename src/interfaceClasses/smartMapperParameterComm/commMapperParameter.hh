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

#ifndef SMART_COMM_MAPPER_PARAMETER_HH
#define SMART_COMM_MAPPER_PARAMETER_HH

#include <string>
#include <iostream>

#include "smartMapperParameterC.hh"

#define LISP_SEPARATOR " ()\n"
#define LISP_STRING    1000

namespace Smart {

// ------------------------------------------------------------------
// tags which are used for communicating with the planner
// ------------------------------------------------------------------

typedef enum {
  MAPPER_CUR_PARAMETER,
  MAPPER_CUR_LTM,
  MAPPER_CUR_EMPTY,
  MAPPER_CUR_SAVE,
  MAPPER_CUR_LOAD,
  MAPPER_CUR_LOAD_LTM,
  MAPPER_CUR_SAVE_XPM,
  MAPPER_LTM_PARAMETER,
  MAPPER_LTM_INITIALIZE,
  MAPPER_LTM_SAVE,
  MAPPER_LTM_LOAD,
  MAPPER_LTM_SAVE_XPM
} MapperTagType;


/** 
  A class representing the parameters to control the Mapper (GridMap).
 */
class CommMapperParameter
{
protected:
  SmartIDL::MapperParameter cmd;

public:

  CommMapperParameter();
  virtual ~CommMapperParameter();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommMapperParameter"; };

  /// extract all relevant parameters from the communication class of the map parameter class
  void get(MapperTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const; 

  /// sets all relevant parameters to the communication class of the map parameter class 
  int set(std::string& inString);
};

//
// Implementation
//


inline void CommMapperParameter::get(MapperTagType &tag,int &value1,int &value2,int &value3,int &value4,int &value5) const 
{
  tag = (MapperTagType)cmd.tag;
  value1 = cmd.parameter1;
  value2 = cmd.parameter2;
  value3 = cmd.parameter3;
  value4 = cmd.parameter4;
  value5 = cmd.parameter5;
}

inline int CommMapperParameter::set(std::string& inString)
{
  // ----------------------------------------------------
  //
  // handle tag to mapper
  //
  // input
  //   CURPARAMETER ?xsize ?ysize ?xpos ?ypos ?id
  //   CURLTM DISABLE/ENABLE ?threshold
  //   CUREMPTY ACCUMULATE/EMPTY
  //   CURSAVE ?number
  //   CURLOAD ?number
  //   CURLOADLTM
  //   CURSAVEXPM ?number
  //   LTMPARAMETER ?xsize ?ysize ?xpos ?ypos ?id
  //   LTMINITIALIZE ?init
  //   LTMSAVE ?number
  //   LTMLOAD ?number
  //   LTMSAVEXPM ?number
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


  if (strcasecmp(param,"CURPARAMETER")==0) 
  {
    // --------------------
    // set parameter of current map
    // --------------------
    parse = (char *)calloc(LISP_STRING,sizeof(char));
    cmd.tag = MAPPER_CUR_PARAMETER;
    for (i=0; i<5; i++) 
    {
      do 
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));
      parse = strcat(parse,param);
      parse = strcat(parse," ");
    }
    if (sscanf(parse,"%d %d %d %d %d",&cmd.parameter1,
                                      &cmd.parameter2,
                                      &cmd.parameter3,
                                      &cmd.parameter4,
                                      &cmd.parameter5)==5) 
    {
      error = 0;
    } 
    else 
    {
      error = -1;
    }
    free(parse);
  } 
  else if (strcasecmp(param,"CURLTM")==0) 
  {
    // --------------------
    // set preoccupation of current map
    // --------------------
    cmd.tag = MAPPER_CUR_LTM;

    //
    // get the first parameter
    //

    if (error == 0) 
    {
      do 
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("DISABLE",param)==0) 
      {
        cmd.parameter1 = 0;
      } 
      else if(strcasecmp("ENABLE",param)==0) 
      {
        cmd.parameter1 = 1;
      } 
      else 
      {
        error = -1;
      }
    }

    //
    // get the second parameter if available
    //

    if ((error == 0) && (cmd.parameter1 == 1)) 
    {
      do 
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter2)==1)) 
      {
        // everything ok
      } 
      else 
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"CUREMPTY")==0) 
  {
    // --------------------
    // set current map mode
    // --------------------
    cmd.tag = MAPPER_CUR_EMPTY;

    if (error == 0) 
    {
      do 
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if (strcasecmp("ACCUMULATE",param)==0) 
      {
        cmd.parameter1 = 0;
      } 
      else if(strcasecmp("EMPTY",param)==0) 
      {
        cmd.parameter1 = 1;
      } 
      else 
      {
        error = -1;
      }
    }

  }
  else if (strcasecmp(param,"CURSAVE")==0) 
  {
    // --------------------
    // save current map as xml
    // --------------------
    cmd.tag = MAPPER_CUR_SAVE;

    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"CURLOAD")==0) 
  {
    // --------------------
    // load current map from xml 
    // --------------------
    cmd.tag = MAPPER_CUR_LOAD;
    
    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"CURLOADLTM")==0)
  {
    // --------------------
    // load current map from longterm map 
    // --------------------
    cmd.tag = MAPPER_CUR_LOAD_LTM;

  }
  else if (strcasecmp(param,"CURSAVEXPM")==0) 
  {
    // --------------------
    // save current map as xpm
    // --------------------
    cmd.tag = MAPPER_CUR_SAVE_XPM;
    
    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"LTMPARAMETER")==0) 
  {
    // --------------------
    // set parameter of longterm map
    // --------------------
    parse = (char *)calloc(LISP_STRING,sizeof(char));
    cmd.tag = MAPPER_LTM_PARAMETER;

    if (error == 0) 
    {
      for (i=0; i<5; i++) 
      {
        do 
        {
          param = strsep(&input,LISP_SEPARATOR);
        } while ((param != NULL) && (strlen(param)==0));
        parse = strcat(parse,param);
        parse = strcat(parse," ");
      }
      if (sscanf(parse,"%d %d %d %d %d",&cmd.parameter1,
                                        &cmd.parameter2,
                                        &cmd.parameter3,
                                        &cmd.parameter4,
                                        &cmd.parameter5)==5) 
      {
      } 
      else 
      {
        error = -1;
      }
    }
    free(parse);

  } 
  else if (strcasecmp(param,"LTMINITIALIZE")==0) 
  {
    // --------------------
    // clears the map and initializes to value
    // --------------------
    cmd.tag = MAPPER_LTM_INITIALIZE;

    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"LTMSAVE")==0) 
  {
    // --------------------
    // save longterm map as xml
    // --------------------
    cmd.tag = MAPPER_LTM_SAVE;

    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"LTMLOAD")==0) 
  {
    // --------------------
    // load longterm map from xml
    // --------------------
    cmd.tag = MAPPER_LTM_LOAD;

    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

  } 
  else if (strcasecmp(param,"LTMSAVEXPM")==0) 
  {
    // --------------------
    // save longterm map as xpm
    // --------------------
    cmd.tag = MAPPER_LTM_SAVE_XPM;

    if (error == 0) 
    {
      do
      {
        param = strsep(&input,LISP_SEPARATOR);
      } while ((param != NULL) && (strlen(param)==0));

      if ((param != NULL) && (sscanf(param,"%d",&cmd.parameter1)==1))
      {
        // everything ok
      }
      else
      {
        error = -1;
      }
    }

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
