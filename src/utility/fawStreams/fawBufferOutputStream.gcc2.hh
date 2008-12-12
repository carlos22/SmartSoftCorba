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
 
#include <iostream>
#include <iterator>

#include "fawBufferOutputStreamBuf.hh"

namespace Faw {

template<class Iterator>
class BufferOutputStream: public std::ostream
{
public:

  typedef typename std::iterator_traits<Iterator>::value_type char_type;

  BufferOutputStream()
  : std::ostream(0)
  {
    this->init(&_sb);
  }

  BufferOutputStream(const Iterator &begin, const Iterator &end)
  : std::ostream(0), _sb(begin, end) 
  {
    this->init(&_sb);
  }

  inline void set_buffer(const Iterator &begin, const Iterator &end)
  {
    this->flush();
    _sb.set_buffer(begin, end);
  }

  inline Iterator begin() const { return _sb.begin(); }
  inline Iterator end() const { return _sb.end(); }

private:
  BufferOutputStreamBuf<Iterator> _sb;
};

} // namespace Faw
