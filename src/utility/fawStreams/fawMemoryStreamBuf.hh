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
//--------------------------------------------------------------------------
 
#ifndef FAW_MEMORY_STREAM_BUF_HH
#define FAW_MEMORY_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawMemoryStreamBuf.gcc2.hh"
#else

#include <ios>
// <asteck date="04.08.2008">
//#include <streambuf>
#include <streambuf.h>
// </asteck>
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
    // <asteck date="04.08.2008">
    //setp(_pbuf, _pbuf + BUFFER_SIZE);
    //setg(0,0,0);
    streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    streambuf::setg(0,0,0);
    // </asteck>
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
    // <asteck date="04.08.2008">
    //setp(_pbuf, _pbuf + BUFFER_SIZE);
    //setg(0,0,0);
    streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    streambuf::setg(0,0,0);
    // </asteck>
    _giter = _buffer.begin();
  }

  /**
    Reset for reading.
   */
  void reset()
  {
    _giter = _buffer.begin();
    // <asteck date="04.08.2008">
    //setg(0,0,0);
    streambuf::setg(0,0,0);
    // </asteck>
  }

  virtual void print(std::ostream &os = std::cout) const
  {
    os << "BasicMemoryStreamBuf()" << std::endl;
  }

protected:
  
  virtual int sync()
  {
    // <asteck date="04.08.2008">
    //_buffer.append(pbase(), pptr() - pbase());
    //setp(_pbuf, _pbuf + BUFFER_SIZE);
    _buffer.append(streambuf::pbase(), streambuf::pptr() - streambuf::pbase());
    streambuf::setp(_pbuf, _pbuf + BUFFER_SIZE);
    // </asteck>
    return 0;
  }

  virtual int_type overflow(int_type c = traits_type::eof())
  {
    sync();
    if(c!=traits_type::eof())
    {
      // <asteck date="04.08.2008">
      //*(pptr()) = c;
      //pbump(1);
      *(streambuf::pptr()) = c;
      streambuf::pbump(1);
      // </asteck>
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
    // <asteck date="04.08.2008">
    //const int_type c = underflow();
    //if(c!=traits_type::eof()) sbumpc();
    const int_type c = streambuf::underflow();
    if(c!=traits_type::eof()) streambuf::sbumpc();
    // </asteck>
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
