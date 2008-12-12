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

#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "smartKnuthMorrisPratt.hh"
#include "commTimeStamp.hh"

using namespace Smart;

CommTimeStamp::CommTimeStamp()
{
  _ts.sec = 0;
  _ts.usec = 0;
}

CommTimeStamp::CommTimeStamp(const SmartIDL::TimeStamp &ts)
: _ts(ts)
{
}

CommTimeStamp::~CommTimeStamp()
{
}

void CommTimeStamp::get(CORBA::Any &a) const
{
  a <<= _ts;
}

void CommTimeStamp::set(const CORBA::Any &a)
{
  SmartIDL::TimeStamp *tmp_ts;
  if(a >>= tmp_ts)
  {
    _ts = *tmp_ts;
  }
}

std::string CommTimeStamp::get_string() const
{
  std::string date_time;
  time_t t = _ts.sec;
  unsigned int buffer_length = 50;
  unsigned int used_buffer_length;
  while(true)
  {
    char buffer[buffer_length];
    used_buffer_length = strftime(buffer, buffer_length, "%Y-%m-%dT%H:%M:%S", localtime(&t));
    if(used_buffer_length<buffer_length) 
    {
      date_time = buffer;
      break;
    }
    else
    {
      buffer_length += buffer_length;
    }
  }
  std::ostringstream oss;
  oss << "." << std::setw(6) << std::setfill('0') << _ts.usec;
  return date_time + oss.str();
}

void CommTimeStamp::set_now()
{
  timeval t;
  gettimeofday(&t, 0);
  set(t);
}

CommTimeStamp CommTimeStamp::now()
{
  CommTimeStamp ts;
  ts.set_now();
  return ts;
}

CommTimeStamp &CommTimeStamp::advance(double dt)
{
  const int n_sec  = (int)::floor(dt); // may be negative
  const int n_usec = (int)::rint((dt - n_sec) * 1e6); // 0 <= n_usec < 1000000 holds by construction
  _ts.usec += n_usec;
  while(_ts.usec > 1000000)
  {
    _ts.usec -= 1000000;
    _ts.sec  += 1;
  }
  _ts.sec += n_sec;
  return *this;
}

void CommTimeStamp::print(std::ostream &os) const
{
  os << "TimeStamp(" << _ts.sec << "sec," << std::setw(6) << std::setfill('0') << _ts.usec << "usec)";
}

void CommTimeStamp::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<time_stamp>" << std::endl;
  os << indent << "  <sec>" << _ts.sec << "</sec>" << std::endl;
  os << indent << "  <microsec>" << _ts.usec << "</microsec>" << std::endl;
  os << indent << "</time_stamp>" << std::endl;
}

void CommTimeStamp::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<time_stamp>");
  static const KnuthMorrisPratt kmp_sec("<sec>");
  static const KnuthMorrisPratt kmp_microsec("<microsec>");
  static const KnuthMorrisPratt kmp_end("</time_stamp>");

  kmp_begin.search(is);
  kmp_sec.search(is);
  is >> _ts.sec;
  kmp_microsec.search(is);
  is >> _ts.usec;
  kmp_end.search(is);
}
