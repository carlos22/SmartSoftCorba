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

#ifndef SMART_COMM_TIME_STAMP_HH
#define SMART_COMM_TIME_STAMP_HH

#include <string>
#include <iostream>

#include "smartTimeStampC.hh"

namespace Smart {

/**
  A multi purpose time stamp class.
  The time stamp is stored similar to C's <tt>struct timeval</tt>,
  and has a (theoretical) mircosecond resolution.
 */
class CommTimeStamp
{
protected:
  SmartIDL::TimeStamp _ts;

public:

  CommTimeStamp();
  inline CommTimeStamp(const timeval &tv) { set(tv); }
  CommTimeStamp(const SmartIDL::TimeStamp &ts);
  virtual ~CommTimeStamp();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommTimeStamp"; };

  /// internal use only
  inline const SmartIDL::TimeStamp &get_idl() const { return _ts; }
  /// internal use only
  inline void set_idl(const SmartIDL::TimeStamp &ts) { _ts = ts; }

  /**
    Convert this timestamp into a \c timeval.
   */
  inline timeval get() const;
  
  /**
    Copy this timestamp to the given \c timeval \a t reference argument.
   */
  inline void get(timeval &t) const;
  
  /**
    Get the seconds of this timestamp.
   */
  inline unsigned long get_seconds() const { return _ts.sec; }

  /**
    Get the microseconds of this timestamp.
   */
  inline unsigned long get_microseconds() const { return _ts.usec; }

  /**
    Return a string representation of the timestamp.
    The format of the string complies with the format of the \c dateTime type in XML Schema.
   */
  std::string get_string() const;

  /**
    Set this timestamp.
   */
  inline void set(const timeval &t);

  /**
    Set this timestamp to the given seconds and microseconds.
   */
  inline void set(unsigned long seconds, unsigned long microseconds) 
  { 
    _ts.sec = seconds + microseconds / 1000000; 
    _ts.usec = microseconds % 1000000; 
  }

  /**
    Set this timestamp to the current time.
   */
  void set_now();

  /**
    Create a timestamp representing the current time.
   */
  static CommTimeStamp now();

  /**
    Advance this time stamp by the given amount in seconds.
    The amount may be negative.
    \return this time stamp after having been advances.
   */
  CommTimeStamp &advance(double dt);

  /**
    Decide if this timestamp is before the \a other timestamp.
   */
  inline bool is_before(const CommTimeStamp &other) const;

  /**
    Decide if this and the \a other timestamp are coincident.
   */
  inline bool is_equal(const CommTimeStamp &other) const;

  /**
    Decide if this timestamp is after the \a other timestamp.
   */
  inline bool is_after(const CommTimeStamp &other) const { return other.is_before(*this); }
  
  /**
    Return the signed interval between this timestamp and the \a other timestamp in seconds.
    A negative value is returned if the \a other timestamp lies before this timestamp.
   */
  inline double seconds_before(const CommTimeStamp &other) const;

  /**
    Return the signed interval between the \a other timestamp and this timestamp in seconds.
    A negative value is returned if this timestamp lies before the \a other timestamp.
   */
  inline double seconds_after(const CommTimeStamp &other) const { return other.seconds_before(*this); }

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


inline timeval CommTimeStamp::get() const
{
  timeval t;
  this->get(t);
  return t;
}

inline void CommTimeStamp::get(timeval &t) const
{
  t.tv_sec  = _ts.sec;
  t.tv_usec = _ts.usec;
}

inline void CommTimeStamp::set(const timeval &t)
{
  _ts.sec  = t.tv_sec;
  _ts.usec = t.tv_usec;
}

inline bool CommTimeStamp::is_before(const CommTimeStamp &other) const
{
  return (_ts.sec < other._ts.sec) || ((_ts.sec == other._ts.sec) && (_ts.usec < other._ts.usec));
}

inline bool CommTimeStamp::is_equal(const CommTimeStamp &other) const
{
  return (_ts.sec == other._ts.sec) && (_ts.usec == other._ts.usec);
}

inline double CommTimeStamp::seconds_before(const CommTimeStamp &other) const
{
  return (other._ts.sec - double(_ts.sec)) // convert to double before subtract as we're using unsigned integers
       + 1e-6 * (other._ts.usec - double(_ts.usec)); // same here
}

inline std::ostream &operator<<(std::ostream &os, const CommTimeStamp &ts)
{
  ts.print(os);
  return os;
}

} // namespace Smart

#endif
