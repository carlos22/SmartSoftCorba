//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
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
 
#ifndef FAW_BUFFER_INPUT_STREAM_HH
#define FAW_BUFFER_INPUT_STREAM_HH

#if (__GNUC__ < 3)
#include "fawBufferInputStream.gcc2.hh"
#else

#include <ios>
#include <istream>
#include <iterator>

#include "fawBufferInputStreamBuf.hh"

namespace Faw {

template<class Iterator>
class BufferInputStream: public std::basic_istream<typename std::iterator_traits<Iterator>::value_type>
{
public:

  typedef typename std::iterator_traits<Iterator>::value_type char_type;
  typedef typename std::char_traits<char_type> traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BufferInputStream()
  : std::basic_istream<char_type>(0) 
  {
    this->init(&_sb);
  }

  BufferInputStream(const Iterator &begin, const Iterator &end)
  : std::basic_istream<char_type>(0), _sb(begin, end) 
  {
    this->init(&_sb);
  }

  inline void set_buffer(const Iterator &begin, const Iterator &end)
  {
    this->flush();
    _sb.set_buffer(begin, end);
  }

private:
  BufferInputStreamBuf<Iterator> _sb;
};

} // namespace Faw

#endif

#endif
