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

#ifndef SMART_COMM_SPEECH_OUTPUT_MESSAGE_HH
#define SMART_COMM_SPEECH_OUTPUT_MESSAGE_HH

#include <string>
#include <iostream>

#include "commTimeStamp.hh"
#include "smartSpeechOutputMessageC.hh"

namespace Smart {

/**
  A message that is to be output as spoken language.
 */
class CommSpeechOutputMessage
{
protected:
  SmartIDL::SpeechOutputMessage _msg;

public:

  CommSpeechOutputMessage(const std::string &text = "");
  CommSpeechOutputMessage(const SmartIDL::SpeechOutputMessage &msg);

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommSpeechOutputMessage"; };



  /**
    Get the message text.
   */
  inline std::string get_message() const { return (const char*)(_msg.text); }
  
  /** 
    Get the creation time.
   */
  inline CommTimeStamp get_creation_time() const { return CommTimeStamp(_msg.time); }
  
  /**
    Get time to live of this message in seconds.
   */
  inline double get_time_to_live() const { return _msg.ttl; }
  
  /**
    Compute time when message shall be discarded.
   */
  inline CommTimeStamp get_discard_time() const { return CommTimeStamp(_msg.time).advance(_msg.ttl); }
  
  /**
    Set the message text.
   */
  inline void set_message(const std::string &msg) { _msg.text = msg.c_str(); }

  /** 
    Set the creation time to now.
   */
  void set_creation_time_now();
  
  /**
    Set time to live of this message in seconds.
   */
  inline void set_time_to_live(double ttl) { _msg.ttl = ttl; }
  
  /**
    Write information about this timestamp to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this time stamp to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

//
// Implementation
//

inline std::ostream &operator<<(std::ostream &os, const CommSpeechOutputMessage &msg)
{
  msg.print(os);
  return os;
}

} // namespace Smart

#endif
