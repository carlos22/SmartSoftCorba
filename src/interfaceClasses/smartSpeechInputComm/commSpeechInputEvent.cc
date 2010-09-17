// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Andreas Steck, Matthias Lutz
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
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

#include "commSpeechInputEvent.hh"

using namespace Smart;

/////////////////////////////////////////////////////////////////////////////
// parameter
/////////////////////////////////////////////////////////////////////////////
CommSpeechInputEventParameter::CommSpeechInputEventParameter()
{
  parameter.confidence = 0.0;
  parameter.topic = "";
}

CommSpeechInputEventParameter::~CommSpeechInputEventParameter()
{
}

void CommSpeechInputEventParameter::get(CORBA::Any &a) const
{
  a <<= parameter;
}

void CommSpeechInputEventParameter::set(const CORBA::Any &a)
{
  SmartIDL::SpeechInputEventParameter *t;

  a >>= t;

  parameter = *t;
}

void CommSpeechInputEventParameter::set_topic(std::string topic)
{
  parameter.topic = topic.c_str();
}

std::string CommSpeechInputEventParameter::get_topic() const
{
  return (const char*)parameter.topic;
}

void CommSpeechInputEventParameter::set_confidence(double confidence)
{
  parameter.confidence = confidence;
}

double CommSpeechInputEventParameter::get_confidence() const
{
  return parameter.confidence;
}


/////////////////////////////////////////////////////////////////////////////
// result
/////////////////////////////////////////////////////////////////////////////

CommSpeechInputEventResult::CommSpeechInputEventResult()
{
}

CommSpeechInputEventResult::~CommSpeechInputEventResult()
{
  result.text = "";
  result.confidence = 0.0;
  result.semantic = "";
}

void CommSpeechInputEventResult::get(CORBA::Any &a) const
{
  a <<= result;
}

void CommSpeechInputEventResult::set(const CORBA::Any &a)
{
  SmartIDL::SpeechInputEventResult *t;

  a >>= t;

  result = *t;
}

void CommSpeechInputEventResult::set_text(std::string text)
{
  result.text = text.c_str();
}

std::string CommSpeechInputEventResult::get_text() const
{
  return (const char*)result.text;
}

void CommSpeechInputEventResult::set_semantic(std::string semantic)
{
  result.semantic = semantic.c_str();
}

std::string CommSpeechInputEventResult::get_semantic() const
{
  return (const char*)result.semantic;
}

void CommSpeechInputEventResult::set_confidence(double confidence)
{
  result.confidence = confidence;
}

double CommSpeechInputEventResult::get_confidence() const
{
  return result.confidence;
}



/////////////////////////////////////////////////////////////////////////////
// state
/////////////////////////////////////////////////////////////////////////////
SpeechInputEventState::SpeechInputEventState()
{
  topic = "";
  text = "";
  semantic = 0.0;
  confidence = 0.0;
}

SpeechInputEventState::~SpeechInputEventState()
{
}

void SpeechInputEventState::set_topic(std::string t)
{
  topic = t;
}

std::string SpeechInputEventState::get_topic() const
{
  return topic;
}

void SpeechInputEventState::set_confidence(double c)
{
  confidence = c;
}

double SpeechInputEventState::get_confidence() const
{
  return confidence;
}

void SpeechInputEventState::set_text(std::string t)
{
  text = t;
}

std::string SpeechInputEventState::get_text() const
{
  return text;
}

void SpeechInputEventState::set_semantic(std::string s)
{
  semantic = s;
}

std::string SpeechInputEventState::get_semantic() const
{
  return semantic;
}

