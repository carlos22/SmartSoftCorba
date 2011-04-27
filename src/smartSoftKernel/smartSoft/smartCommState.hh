// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Alex Lotz, Andreas Steck
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
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
// --------------------------------------------------------------------------

#ifndef SMART_COMM_STATE_HH
#define SMART_COMM_STATE_HH

#include <string>
#include <list>

#include "smartCommStateC.hh"

namespace CHS {

/////////////////////////////////////////////////////////////////////////
//
// Request
//
/////////////////////////////////////////////////////////////////////////

   enum StateCommand
   {
      STATE_CMD_ERROR,
      STATE_CMD_SET_STATE,
      STATE_CMD_GET_MAIN_STATES,
      STATE_CMD_GET_SUB_STATES
   };


  class SmartCommStateRequest
  {
  protected:
     // internal data structure
     SmartStateRequest smartStateRequest;
   
  public:
    //
    // constructors, destructors, copy constructors etc. ...
    //
     SmartCommStateRequest()
     {  
       smartStateRequest.command = static_cast<int>(STATE_CMD_ERROR);
     }

     virtual ~SmartCommStateRequest() {  };

    //
    // The following methods MUST be available in a communication object.
    // This however is not too bad for implementers of a communication
    // object since you can get cookbook like instructions on how
    // to implement these. They are always the same since they have to
    // set / get the above IDL structure. They are used by the communication
    // patterns and should not be used by users.
    //
    void get(CORBA::Any &) const;
    void set(const CORBA::Any &);
    static inline std::string identifier(void) {
      return "CHS::smartStateRequest";
    };


    /// Set/Get the command-enum defined above
    inline void setCommand(const StateCommand &command)
    {
       smartStateRequest.command = static_cast<int>(command);
    }
    inline StateCommand  getCommand(void) const
    {
      return static_cast<StateCommand>(smartStateRequest.command);
    }

    /// Set/Get state-name
    inline void setStateName(const std::string &name)
    {
      smartStateRequest.state_name = name.c_str();
    }
    inline std::string getStateName(void) const
    {
      return static_cast<const char*>(smartStateRequest.state_name);
    }
  };


/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

  class SmartCommStateResponse
  {
  protected:
     // internal data structure
     SmartStateResponse smartStateResponse;
 
  public:
    //
    // constructors, destructors, copy constructors etc. ...
    //
    SmartCommStateResponse()
    {
      smartStateResponse.status = 0;
    }
    virtual ~SmartCommStateResponse() {  }

    //
    // The following methods MUST be available in a communication object.
    // This however is not too bad for implementers of a communication
    // object since you can get cookbook like instructions on how
    // to implement these. They are always the same since they have to
    // set / get the above IDL structure. They are used by the communication
    // patterns and should not be used by users.
    //
    void get(CORBA::Any &) const;
    void set(const CORBA::Any &);
    static inline std::string identifier(void) {
      return "CHS::smartStateResponse";
    };


    /// Get/Set return status
    inline void setStatus(const int &status)
    {
       smartStateResponse.status = status;
    }
    inline int getStatus(void) const
    {
       return smartStateResponse.status;
    }

    ///
    inline void setStateList(const std::list<std::string> &states)
    {
      smartStateResponse.state_list.length(states.size());
      int j=0;
      for (std::list<std::string>::const_iterator i=states.begin(); i!=states.end(); ++i) 
      {
        smartStateResponse.state_list[j++] = i->c_str();
      }

    }

    inline std::list<std::string> getStateList(void) const
    {
      std::list<std::string> state_list;
      for (unsigned int i=0; i < smartStateResponse.state_list.length(); i++) 
      {
        state_list.push_back( static_cast<const char*>(smartStateResponse.state_list[i]) );
      }
      return state_list;
    }
  };


} // end namespace SMART_COMM_STATE_HH
#endif

