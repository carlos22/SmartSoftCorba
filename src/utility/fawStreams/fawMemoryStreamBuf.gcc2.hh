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

#include "fawDynamicBuffer.hh"

namespace Faw {

class MemoryStreamBuf: public std::streambuf
{
public:

  MemoryStreamBuf(DynamicBuffer &buffer) 
    : _buffer(buffer), _gbuf_avail(0)
  {
    setp(_pbuf, _pbuf + BUFFER_SIZE);
    setg(0,0,0);
    _giter = _buffer.begin();
  }

  ~MemoryStreamBuf() { sync(); }

  /**
    Reset for writing.
    Implies reset for reading.
   */
  void clear()
  {
    _buffer.clear();
    setp(_pbuf, _pbuf + BUFFER_SIZE);
    setg(0,0,0);
    _giter = _buffer.begin();
  }

  /**
    Reset for reading.
   */
  void reset()
  {
    _giter = _buffer.begin();
    setg(0,0,0);
  }

  virtual void print(std::ostream &os = std::cout) const
  {
    os << "MemoryStreamBuf()" << std::endl;
  }

  virtual int sync()
  {
    _buffer.append(pbase(), pptr() - pbase());
    setp(_pbuf, _pbuf + BUFFER_SIZE);
    return 0;
  }

  virtual int overflow(int c = EOF)
  {
    sync();
    if(c!=EOF)
    {
      *(pptr()) = c;
      pbump(1);
    }
    return c;
  }

  virtual int underflow()
  {
    _gbuf_avail = _giter.get(_gbuf, BUFFER_SIZE);
    if(_gbuf_avail>0)
    {
      setg(_gbuf, _gbuf, _gbuf + _gbuf_avail);
      return ((unsigned char*)_gbuf)[0];
    }
    return EOF;
  }

  virtual int uflow()
  {
    const int c = underflow();
    if(c!=EOF) sbumpc();
    return c;
  }

private:

  DynamicBuffer &_buffer;

  static const unsigned int BUFFER_SIZE = 256;
  char _pbuf[BUFFER_SIZE];
  char _gbuf[BUFFER_SIZE];
  unsigned int _gbuf_avail;
  DynamicBuffer::const_iterator _giter;
};

} // namespace Faw
