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
 
//--------------------------------------------------------------------------
// changelog:
// Andreas Steck 04.08.2008:
// changes in usage of streambuf
//--------------------------------------------------------------------------

#ifndef FAW_BUFFER_INPUT_STREAM_BUF_HH
#define FAW_BUFFER_INPUT_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawBufferInputStreamBuf.gcc2.hh"
#else

#include <ios>
// <asteck date="04.08.2008">
//#include <streambuf>
#include <streambuf.h>
// </asteck>
#include <iterator>

namespace Faw {

template<class Iterator>
class BufferInputStreamBuf: public std::basic_streambuf<typename std::iterator_traits<Iterator>::value_type>
{
public:

  typedef typename std::iterator_traits<Iterator>::value_type char_type;
  typedef typename std::char_traits<char_type> traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BufferInputStreamBuf() : _end(_begin), _current(_begin) {}

  BufferInputStreamBuf(const Iterator &begin, const Iterator &end)
  {
    set_buffer(begin, end);
  }

  inline void set_buffer(const Iterator &begin, const Iterator &end)
  {
    _begin = begin; _end = end; _current = _begin;
  }

protected:

  virtual int_type underflow()
  {
    if(_current!=_end)
    {
      unsigned int i = 0;
      while((_current!=_end) && (i<BUFFER_SIZE))
      {
        _buffer[i] = *_current;
        ++_current;
        ++i;
      }
      setg(_buffer, _buffer, _buffer + i);
      return traits_type::to_int_type(_buffer[0]);
    }
    else
    {
      return traits_type::eof();
    }
  }
  
  virtual int_type uflow()
  {
    const int_type c = underflow();
    // <asteck date="04.08.2008">
    //if(c!=traits_type::eof()) sbumpc();
    if(c!=traits_type::eof()) streambuf::sbumpc();
    // </asteck>
    return c;
  }

private:
  Iterator _begin;
  Iterator _end;
  Iterator _current;

  static const unsigned int BUFFER_SIZE = 250;
  char_type _buffer[BUFFER_SIZE];
};

} // namespace Faw

#endif

#endif
