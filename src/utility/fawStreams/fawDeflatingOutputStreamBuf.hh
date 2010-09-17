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
 
#ifndef FAW_DEFLATING_OUTPUT_STREAM_BUF_HH
#define FAW_DEFLATING_OUTPUT_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawDeflatingOutputStreamBuf.gcc2.hh"
#else

#include <zlib.h>

#include <bits/char_traits.h>
#include <streambuf>
#include <iostream>

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch>, class Octet = char>
class BasicDeflatingOutputStreamBuf: public std::basic_streambuf<Ch,Tr>
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BasicDeflatingOutputStreamBuf(std::basic_streambuf<Octet> *sb = 0, int level = Z_DEFAULT_COMPRESSION) 
  : _sb(0)
  {
    if(sb) this->open(sb, level);
    _zlib_stream.total_in  = 0;
    _zlib_stream.total_out = 0;
  }

  BasicDeflatingOutputStreamBuf(std::basic_ostream<Octet> &os, int level = Z_DEFAULT_COMPRESSION) 
  : _sb(0)
  {
    this->open(os, level);
    _zlib_stream.total_in  = 0;
    _zlib_stream.total_out = 0;
  }

  void open(std::basic_ostream<Octet> &os, int level = Z_DEFAULT_COMPRESSION)
  { 
    this->open(os.rdbuf(), level); 
  }

  void open(std::basic_streambuf<Octet> *sb, int level = Z_DEFAULT_COMPRESSION)
  {
    if(_sb) this->close();

    assert(sizeof(Octet)==sizeof(Bytef));

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

  virtual ~BasicDeflatingOutputStreamBuf()
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
      _sb->pubsync();
    }
  }

protected:
  
  virtual int sync()
  {
    // do nothing: calling "sync" on every "endl" is terribly bad for compression!
    return 0;
  }

  virtual int_type overflow(int_type c = traits_type::eof())
  {
    if(_sb)
    {
      _flush_input_buffer();
      if(c!=traits_type::eof())
      {
        *(std::streambuf::pptr()) = c; 
        std::streambuf::pbump(1);
      }
      else
      {
        this->close();
      }
    }
    return c;
  }

private:

  inline int _flush_input_buffer()
  {
    _zlib_stream.next_in = (Bytef*)std::streambuf::pbase();
    _zlib_stream.avail_in = (std::streambuf::pptr() - std::streambuf::pbase()) * sizeof(Ch);
    while(_zlib_stream.avail_in>0)
    {
      if(_zlib_stream.avail_out==0)
      {
        _sb->sputn((const Octet*)_obuf, BUFFER_LENGTH);
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
        _sb->sputn((const Octet*)_obuf, BUFFER_LENGTH);
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
      _sb->sputn((const Octet*)_obuf, BUFFER_LENGTH - _zlib_stream.avail_out);
      _zlib_stream.next_out = _obuf;
      _zlib_stream.avail_out = BUFFER_LENGTH;
    }
    return 0;
  }
  
  std::basic_streambuf<Ch,Tr> *_sb;

  static const unsigned int BUFFER_LENGTH = 4096; /* Ch characters in ibuf, bytes in obuf */
  Ch    _ibuf[BUFFER_LENGTH];
  Bytef _obuf[BUFFER_LENGTH];

  z_stream _zlib_stream;
};

typedef BasicDeflatingOutputStreamBuf<char> DeflatingOutputStreamBuf;

} // namespace Faw

#endif

#endif
