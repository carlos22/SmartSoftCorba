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
 
#ifndef FAW_FILE_DES_OUTPUT_STREAM_BUF_HH
#define FAW_FILE_DES_OUTPUT_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawFileDesOutputStreamBuf.gcc2.hh"
#else

#include <unistd.h>
#include <ios>

#include <streambuf>

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch> >
class BasicFileDesOutputStreamBuf: public std::basic_streambuf<Ch,Tr>
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BasicFileDesOutputStreamBuf(int fd) 
    : _fd(fd) 
  {
    setp(_buffer, _buffer + BUFFER_SIZE);
  }

protected:
  
  virtual int_type sync()
  {
    write(_fd, std::streambuf::pbase(), std::streambuf::pptr() - std::streambuf::pbase());
    setp(_buffer, _buffer + BUFFER_SIZE);
    return 0;
  }

  virtual int_type overflow(int_type c = traits_type::eof())
  {
    if(c!=traits_type::eof())
    {
      sync();
      setp(_buffer, _buffer + BUFFER_SIZE);
      _buffer[0] = traits_type::to_char_type(c);
      std::streambuf::pbump(1);
    }
    return c;
  }

private:
  int _fd;

  static const unsigned int BUFFER_SIZE = 512;
  Ch _buffer[BUFFER_SIZE];
};

typedef BasicFileDesOutputStreamBuf<char> FileDesOutputStreamBuf;

} // namespace Faw

#endif

#endif
