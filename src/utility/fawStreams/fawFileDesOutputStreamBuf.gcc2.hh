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
 
#include <unistd.h>
#include <streambuf.h>

namespace Faw {

class FileDesOutputStreamBuf: public std::streambuf
{
public:

  FileDesOutputStreamBuf(int fd) 
    : _fd(fd) 
  {
    setp(_buffer, _buffer + BUFFER_SIZE);
  }

  ~FileDesOutputStreamBuf()
  {
    sync();
  }

// gcc 2.95.2 expects this to be public, ANSI-C++ says this should be protected
  
  virtual int sync()
  {
    write(_fd, pbase(), pptr() - pbase());
    setp(_buffer, _buffer + BUFFER_SIZE);
    return 0;
  }

  virtual int overflow(int c = EOF)
  {
    sync();
    setp(_buffer, _buffer + BUFFER_SIZE);
    _buffer[0] = c;
    pbump(1);
    return c;
  }

private:
  int _fd;
  static const unsigned int BUFFER_SIZE = 512;
  char _buffer[BUFFER_SIZE];
};

} // namespace Faw
