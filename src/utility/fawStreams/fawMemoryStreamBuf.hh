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

//--------------------------------------------------------------------------
// changelog:
// Andreas Steck 04.08.2008:
// changes in usage of streambuf
// -------------------------------
// Steck; Lutz 26.07.2010:
// update to GCC 4.5
//--------------------------------------------------------------------------
 
#ifndef FAW_MEMORY_STREAM_BUF_HH
#define FAW_MEMORY_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawMemoryStreamBuf.gcc2.hh"
#else

#include <ios>
#include <streambuf>
#include <iterator>

#include "fawDynamicBuffer.hh"

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch> >
class BasicMemoryStreamBuf: public std::basic_streambuf<Ch,Tr>
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BasicMemoryStreamBuf(BasicDynamicBuffer<Ch,Tr> &buffer) 
    : _buffer(buffer), _gbuf_avail(0)
  {
    std::streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    std::streambuf::setg(0,0,0);
    _giter = _buffer.begin();
  }

  ~BasicMemoryStreamBuf() { sync(); }

  /**
    Reset for writing.
    Implies reset for reading.
   */
  void clear()
  {
    _buffer.clear();
    std::streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    std::streambuf::setg(0,0,0);
    _giter = _buffer.begin();
  }

  /**
    Reset for reading.
   */
  void reset()
  {
    _giter = _buffer.begin();
    std::streambuf::setg(0,0,0);
  }

  virtual void print(std::ostream &os = std::cout) const
  {
    os << "BasicMemoryStreamBuf()" << std::endl;
  }

protected:
  
  virtual int sync()
  {
    _buffer.append(std::streambuf::pbase(), std::streambuf::pptr() - std::streambuf::pbase());
    std::streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    return 0;
  }

  virtual int_type overflow(int_type c = traits_type::eof())
  {
    sync();
    if(c!=traits_type::eof())
    {
      *(std::streambuf::pptr()) = c;
      std::streambuf::pbump(1);
    }
    return c;
  }

  virtual int_type underflow()
  {
    _gbuf_avail = _giter.get(_gbuf, BUFFER_SIZE);
    if(_gbuf_avail>0)
    {
      setg(_gbuf, _gbuf, _gbuf + _gbuf_avail);
      return traits_type::to_int_type(_gbuf[0]);
    }
    return traits_type::eof();
  }

  virtual int_type uflow()
  {
    const int_type c = std::streambuf::underflow();
    if(c!=traits_type::eof()) std::streambuf::sbumpc();
    return c;
  }

private:

  BasicDynamicBuffer<Ch,Tr> &_buffer;

  static const unsigned int BUFFER_SIZE = 256;
  Ch _pbuf[BUFFER_SIZE];
  Ch _gbuf[BUFFER_SIZE];
  unsigned int _gbuf_avail;
  typename BasicDynamicBuffer<Ch,Tr>::const_iterator _giter;
};

typedef BasicMemoryStreamBuf<char> MemoryStreamBuf;

} // namespace Faw

#endif

#endif
