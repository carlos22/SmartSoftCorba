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
 
#include <streambuf.h>
#include <iterator.h>

namespace Faw {

/**
  Write to a buffer of fixed size.
 */
template<class Iterator>
class BufferOutputStreamBuf: public std::streambuf
{
public:

  typedef typename std::iterator_traits<Iterator>::value_type char_type;

  typedef Iterator iterator;

  BufferOutputStreamBuf() 
  : _target_begin(), _target_end(_target_begin), _target_iter(_target_begin), _target_length(0)
  {
    setp(0,0);
  }

  BufferOutputStreamBuf(const Iterator &begin, const Iterator &end)
  : _target_begin(), _target_end(_target_begin), _target_iter(_target_begin), _target_length(0)
  {
    set_buffer(begin,end);
  }

  virtual ~BufferOutputStreamBuf()
  {
    sync();
  }

  void set_buffer(const Iterator &begin, const Iterator &end)
  {
    _target_begin = begin;
    _target_end = end;
    _target_iter = begin;
    _target_length = distance(begin, end);
    setp(_buffer, _buffer + _min(BUFFER_LENGTH, _target_length));
  }

  inline Iterator begin() const
  {
    return _target_begin;
  }
  
  inline Iterator end() const
  {
    return _target_iter;
  }

// gcc 2.95.2 expects this to be public, ANSI-C++ says this should be protected
  
  virtual int sync()
  {
    char_type *pb = pbase();
    const char_type *pp = pptr();
    while(pb!=pp) 
    {
      *_target_iter = *pb;
      ++_target_iter;
      ++pb;
      --_target_length;
    }
    setp(_buffer, _buffer + _min(BUFFER_LENGTH, _target_length));
    return 0;
  }

  virtual int overflow(int c = EOF)
  {
    sync();
    if(_target_length>0)
    {
      _buffer[0] = c;
      pbump(1);
      return c;
    }
    // else we're done
    _target_end = _target_iter;
    _target_length = 0;
    return EOF;
  }

private:
  Iterator _target_begin;
  Iterator _target_end;
  Iterator _target_iter;

  unsigned int _target_length;

  static const unsigned int BUFFER_LENGTH = 32;
  char_type _buffer[BUFFER_LENGTH];

  inline static unsigned int _min(const unsigned int u1, const unsigned int u2)
  {
    return (u1<u2)?u1:u2;
  }
};

} // namespace Faw
