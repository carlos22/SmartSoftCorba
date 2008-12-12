//--------------------------------------------------------------------------
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
//  This file is part of the "FAW Stream Classes Package".
//  It provides some iostream, streambuf and buffer classes.
//  See file README for more information.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------
 
#ifndef FAW_INFLATING_INPUT_STREAM_HH
#define FAW_INFLATING_INPUT_STREAM_HH

#if (__GNUC__ < 3)
#include "fawInflatingInputStream.gcc2.hh"
#else

#include "fawInflatingInputStreamBuf.hh"

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch>, class Octet = char>
class BasicInflatingInputStream: public std::basic_istream<Ch,Tr>
{
public:
  BasicInflatingInputStream(std::basic_streambuf<Octet> *sb = 0) 
  : std::basic_istream<Ch,Tr>(0), _sb(sb) 
  {
    this->init(&_sb);
  }

  BasicInflatingInputStream(std::basic_istream<Octet> &is)
  : std::basic_istream<Ch,Tr>(0), _sb(is) 
  {
    this->init(&_sb);
  }

  inline void open(std::basic_streambuf<Octet> *sb) 
  {
    _sb.open(sb); 
  }

  inline void open(std::basic_istream<Octet> &is)
  { 
    _sb.open(is); 
  }

  inline bool is_open() const 
  {
    return _sb.is_open(); 
  }

  inline void close() 
  {
    _sb.close();
  }

private:
  BasicInflatingInputStreamBuf<Ch,Tr,Octet> _sb;
};

typedef BasicInflatingInputStream<char> InflatingInputStream;

} // namespace Faw

#endif

#endif
