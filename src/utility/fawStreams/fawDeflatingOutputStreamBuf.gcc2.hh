//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
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
 
#include <zlib.h>

#include <streambuf.h>
#include <iostream>

namespace Faw {

class DeflatingOutputStreamBuf: public std::streambuf
{
public:

  DeflatingOutputStreamBuf(std::streambuf *sb = 0, int level = Z_DEFAULT_COMPRESSION) 
  : _sb(0)
  {
    if(sb) this->open(sb, level);
    _zlib_stream.total_in  = 0;
    _zlib_stream.total_out = 0;
  }

  DeflatingOutputStreamBuf(std::ostream &os, int level = Z_DEFAULT_COMPRESSION) 
  : _sb(0)
  {
    this->open(os, level);
    _zlib_stream.total_in  = 0;
    _zlib_stream.total_out = 0;
  }

  void open(std::ostream &os, int level = Z_DEFAULT_COMPRESSION)
  { 
    this->open(os.rdbuf(), level); 
  }

  void open(std::streambuf *sb, int level = Z_DEFAULT_COMPRESSION)
  {
    if(_sb) this->close();

    setp(_ibuf, _ibuf + BUFFER_LENGTH);

    _zlib_stream.next_out  = _obuf;
    _zlib_stream.avail_out = BUFFER_LENGTH;
    _zlib_stream.zalloc    = Z_NULL;
    _zlib_stream.zfree     = Z_NULL;
    _zlib_stream.opaque    = Z_NULL;
    _zlib_stream.total_in  = 0;
    _zlib_stream.total_out = 0;

    if(level!=Z_DEFAULT_COMPRESSION)
    {
      if(level<0) level = 0;
      if(level>9) level = 9;
    }

    const int result = deflateInit(&_zlib_stream, level);
    if(result==Z_OK) _sb = sb;
  }

  virtual ~DeflatingOutputStreamBuf()
  {
    if(_sb) close();
  }

  bool is_open() const { return _sb; }

  float ratio() const { return (_zlib_stream.total_in>0)?_zlib_stream.total_out/(float)_zlib_stream.total_in:1; }
  unsigned int uncompressed_bytes_in() const { return _zlib_stream.total_in; }
  unsigned int compressed_bytes_out() const { return _zlib_stream.total_out; }

  void close()
  {
    if(_sb)
    {
      full_sync();
      int result;
      do
      {
        result = deflate(&_zlib_stream, Z_FINISH);
        if(result==Z_OK) _flush_output_buffer();
      }
      while(result==Z_OK);
      deflateEnd(&_zlib_stream);
      _flush_output_buffer();
      _sb = 0;
    }
  }

  void full_sync()
  {
    if(_sb)
    {
      _flush_input_buffer();
      _flush_zlib_buffer();
      _flush_output_buffer();
      _sb->sync();
    }
  }

// gcc 2.95.2 expects this to be public, ANSI-C++ says this should be protected
  
  virtual int sync()
  {
    // do nothing: sync is called whenever "endl" is written!
    return 0;
  }

  virtual int overflow(int c = EOF)
  {
    if(_sb)
    {
      _flush_input_buffer();
      if(c!=EOF)
      {
        *(pptr()) = c; 
        pbump(1);
      }
    }
    return c;
  }

private:

  inline int _flush_input_buffer()
  {
    _zlib_stream.next_in = (Bytef*)pbase();
    _zlib_stream.avail_in = pptr() - pbase();
    while(_zlib_stream.avail_in>0)
    {
      if(_zlib_stream.avail_out==0)
      {
        _sb->sputn((const char*)_obuf, BUFFER_LENGTH);
        _zlib_stream.next_out = _obuf;
        _zlib_stream.avail_out = BUFFER_LENGTH;
      }
      deflate(&_zlib_stream, Z_NO_FLUSH);
    }
    setp(_ibuf, _ibuf + BUFFER_LENGTH);
    return 0;
  }

  inline int _flush_zlib_buffer()
  {
    do
    {
      if(_zlib_stream.avail_out==0)
      {
        _sb->sputn((const char*)_obuf, BUFFER_LENGTH);
        _zlib_stream.next_out = _obuf;
        _zlib_stream.avail_out = BUFFER_LENGTH;
      }
      deflate(&_zlib_stream, Z_FULL_FLUSH);
    }
    while(_zlib_stream.avail_out==0);
    return 0;
  }

  inline int _flush_output_buffer()
  {
    if(_zlib_stream.avail_out<BUFFER_LENGTH)
    {
      _sb->sputn((const char*)_obuf, BUFFER_LENGTH - _zlib_stream.avail_out);
      _zlib_stream.next_out = _obuf;
      _zlib_stream.avail_out = BUFFER_LENGTH;
    }
    return 0;
  }
  
  std::streambuf *_sb;

  static const unsigned int BUFFER_LENGTH = 4096;
  char _ibuf[BUFFER_LENGTH];
  Bytef _obuf[BUFFER_LENGTH];

  z_stream _zlib_stream;
};

} // namespace Faw


