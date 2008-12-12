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
 
#ifndef FAW_FILE_DES_OUTPUT_STREAM_HH
#define FAW_FILE_DES_OUTPUT_STREAM_HH

#if (__GNUC__ < 3)
#include "fawFileDesOutputStream.gcc2.hh"
#else

#include <iostream>

#include "fawFileDesOutputStreamBuf.hh"

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch> >
class BasicFileDesOutputStream: public std::basic_ostream<Ch,Tr>
{
public:
  BasicFileDesOutputStream(int fd) 
  : std::basic_ostream<Ch,Tr>(0), _sb(fd) 
  {
    this->init(&_sb);
  }
private:
  BasicFileDesOutputStreamBuf<Ch,Tr> _sb;
};

typedef BasicFileDesOutputStream<char> FileDesOutputStream;

} // namespace Faw

#endif

#endif
