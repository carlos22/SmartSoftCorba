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
 
#include <zlib.h>
#include <streambuf.h>
#include <iostream>

namespace Faw {

class InflatingInputStreamBuf: public std::streambuf
{
public:

  InflatingInputStreamBuf(std::streambuf *sb = 0) 
    : _sb(0) 
  { 
    if(sb) this->open(sb); 
  }

  InflatingInputStreamBuf(std::istream &is) 
    : _sb(0) 
  { 
    this->open(is); 
  }

  void open(std::istream &is) 
  { 
    this->open(is.rdbuf()); 
  }

  void open(std::streambuf *sb)
  {
    if(_sb) this->close();

    _zlib_stream.next_in   = Z_NULL;
    _zlib_stream.avail_in  = 0;
    _zlib_stream.next_out  = Z_NULL;
    _zlib_stream.avail_out = 0;
    _zlib_stream.zalloc    = Z_NULL;
    _zlib_stream.zfree     = Z_NULL;
    _zlib_stream.opaque    = Z_NULL;

    const int result = inflateInit(&_zlib_stream);
    if(result==Z_OK) _sb = sb;
  }

  virtual ~InflatingInputStreamBuf()
  {
    if(_sb) this->close();
  }

  inline bool is_open() const { return _sb; }

  void close()
  {
    if(_sb)
    {
      inflateEnd(&_zlib_stream);
      _sb = 0;
    }
  }

  virtual int underflow()
  {
    int c = EOF;
    if(_sb)
    {
      int result = Z_OK;

      if(_zlib_stream.avail_out==0)
      {
        _zlib_stream.next_out = (Bytef*)_obuf;
        _zlib_stream.avail_out = BUFFER_LENGTH;
      }

      unsigned int bytes_in_obuf = BUFFER_LENGTH - _zlib_stream.avail_out;
      char *obuf_read_begin = _obuf + bytes_in_obuf;
      char *obuf_read_end   = obuf_read_begin;

      while((obuf_read_begin==obuf_read_end) && (result==Z_OK))
      {
        if(_zlib_stream.avail_in==0)
        {
          _zlib_stream.next_in  = (Bytef*)_ibuf;
          _zlib_stream.avail_in = _sb->sgetn(_ibuf, BUFFER_LENGTH);
        }
        
        result = inflate(&_zlib_stream, Z_SYNC_FLUSH);
        bytes_in_obuf = BUFFER_LENGTH - _zlib_stream.avail_out;
        obuf_read_end = _obuf + bytes_in_obuf;
      }

      if(result==Z_OK)
      {
        c = *((unsigned char*)obuf_read_begin);
        setg(obuf_read_begin, obuf_read_begin, obuf_read_end);
      }
      else if(result==Z_STREAM_END)
      {
        if(obuf_read_begin!=obuf_read_end)
        {
          c = *((unsigned char*)obuf_read_begin);
          setg(obuf_read_begin, obuf_read_begin, obuf_read_end);
        }
        else 
        {
          this->close();
        }
      }
      else
      {
        this->close();
      }
    }
    return c;
  }

  virtual int uflow()
  {
    const int c = underflow();
    if(c!=EOF) sbumpc();
    return c;
  }

private:

  std::streambuf *_sb;

  static const unsigned int BUFFER_LENGTH = 4096;
  char _ibuf[BUFFER_LENGTH];
  char _obuf[BUFFER_LENGTH];

  z_stream _zlib_stream;
};

} // namespace Faw
