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
 
#ifndef FAW_FILE_DES_INPUT_STREAM_BUF_HH
#define FAW_FILE_DES_INPUT_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawFileDesInputStreamBuf.gcc2.hh"
#else

#include <unistd.h>
#include <ios>
#include <streambuf>

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch> >
class BasicFileDesInputStreamBuf: public std::basic_streambuf<Ch,Tr>
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BasicFileDesInputStreamBuf(int fd) 
    : _fd(fd) 
  {
  }

  virtual ~BasicFileDesInputStreamBuf()
  {
  }

protected:
  
  virtual int_type underflow()
  {
    int result = read(_fd, _buffer, BUFFER_SIZE * sizeof(Ch));
    if(result>0)
    {
      setg(_buffer, _buffer, _buffer + (result / sizeof(Ch)));
      return traits_type::to_int_type(_buffer[0]);
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
  int _fd;
  static const unsigned int BUFFER_SIZE = 512;
  Ch _buffer[BUFFER_SIZE];
};

typedef BasicFileDesInputStreamBuf<char> FileDesInputStreamBuf;

} // namespace Faw

#endif

#endif
