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

#ifndef _COMM__SPEECHINPUT_EVENT_HH
#define _COMM__SPEECHINPUT_EVENT_HH

#include <iostream>
#include <string>
#include "smartSpeechInputEventC.hh"

namespace Smart {

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
class CommSpeechInputEventParameter
{
protected:
  SmartIDL::SpeechInputEventParameter parameter;

public:
  CommSpeechInputEventParameter();
  virtual ~CommSpeechInputEventParameter();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::smartSpeechInputEventParameter";
  };

  void set_topic(std::string);
  std::string get_topic() const;

  void set_confidence(double);
  double get_confidence() const;
};

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
class CommSpeechInputEventResult
{
protected:
  SmartIDL::SpeechInputEventResult result;

public:
  CommSpeechInputEventResult();
  virtual ~CommSpeechInputEventResult();

  void get(CORBA::Any &) const;
  void set(const CORBA::Any &);
  static inline std::string identifier(void) {
    return "CHS::smartSpeechInputEventResult";
  };

  void set_text(std::string);
  std::string get_text() const;

  void set_semantic(std::string);
  std::string get_semantic() const;

  void set_confidence(double);
  double get_confidence() const;
};

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
class SpeechInputEventState
{
protected:
  std::string topic;
  std::string text;
  std::string semantic;
  double confidence;

public:
  SpeechInputEventState();
  virtual ~SpeechInputEventState();

  void set_topic(std::string);
  std::string get_topic() const;

  void set_confidence(double);
  double get_confidence() const;

  void set_text(std::string);
  std::string get_text() const;

  void set_semantic(std::string);
  std::string get_semantic() const;
};

}

#endif

