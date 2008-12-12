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


#include "smartKnuthMorrisPratt.hh"
#include "commSpeechOutputMessage.hh"

using namespace Smart;

CommSpeechOutputMessage::CommSpeechOutputMessage(const std::string &text)
{
  _msg.time = CommTimeStamp::now().get_idl();
  _msg.reset = false;
  _msg.ttl = 3600;
  _msg.text = text.c_str();
}

CommSpeechOutputMessage::CommSpeechOutputMessage(const SmartIDL::SpeechOutputMessage &msg)
: _msg(msg)
{
}

void CommSpeechOutputMessage::get(CORBA::Any &a) const
{
  a <<= _msg;
}

void CommSpeechOutputMessage::set(const CORBA::Any &a)
{
  SmartIDL::SpeechOutputMessage *tmp_msg;
  if(a >>= tmp_msg)
  {
    _msg = *tmp_msg;
  }
}

void CommSpeechOutputMessage::set_creation_time_now()
{
  _msg.time = CommTimeStamp::now().get_idl();
}

void CommSpeechOutputMessage::print(std::ostream &os) const
{
  os << "SpeechOutputMessage(\"" << _msg.text << "\", " << get_creation_time() << ", ttl=" << _msg.ttl << "sec, reset=" << int(_msg.reset) << ")" << std::endl;
}

void CommSpeechOutputMessage::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<speech_output_message>" << std::endl;
  os << indent << "  <text>" << _msg.text << "</text>" << std::endl;
  get_creation_time().save_xml(os, indent + "  ");
  os << indent << "  <ttl>" << _msg.ttl << "</ttl>" << std::endl;
  os << indent << "  <reset>" << int(_msg.reset) << "</reset>" << std::endl;
  os << indent << "</speech_output_message>" << std::endl;
}

void CommSpeechOutputMessage::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<speech_output_message>");
  static const KnuthMorrisPratt kmp_text("<text>");
  static const KnuthMorrisPratt kmp_ttl("<ttl>");
  static const KnuthMorrisPratt kmp_reset("<reset>");
  static const KnuthMorrisPratt kmp_end("</speech_output_message>");

  kmp_begin.search(is);

  kmp_text.search(is);
  std::string txt;
  is >> txt;
  _msg.text = txt.c_str();

  CommTimeStamp ts;
  ts.load_xml(is);
  _msg.time = ts.get_idl();

  kmp_ttl.search(is);
  is >> _msg.ttl;

  int b;
  kmp_reset.search(is);
  is >> b;
  _msg.reset = b;
  
  kmp_end.search(is);
}
