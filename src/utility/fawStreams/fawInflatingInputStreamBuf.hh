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
 
#ifndef FAW_INFLATING_INPUT_STREAM_BUF_HH
#define FAW_INFLATING_INPUT_STREAM_BUF_HH

#if (__GNUC__ < 3)
#include "fawInflatingInputStreamBuf.gcc2.hh"
#else

#include <zlib.h>
#include <bits/char_traits.h>
#include <streambuf>
#include <iostream>

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch>, class Octet = char>
class BasicInflatingInputStreamBuf: public std::basic_streambuf<Ch,Tr>
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  BasicInflatingInputStreamBuf(std::basic_streambuf<Octet> *sb = 0) 
    : _sb(0) 
  { 
    if(sb) this->open(sb); 
  }

  BasicInflatingInputStreamBuf(std::basic_istream<Octet> &is) 
    : _sb(0) 
  { 
    this->open(is); 
  }

  void open(std::basic_istream<Octet> &is) 
  { 
    this->open(is.rdbuf()); 
  }

  void open(std::basic_streambuf<Octet> *sb)
  {
    assert(sizeof(Octet)==sizeof(Bytef));

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

  virtual ~BasicInflatingInputStreamBuf()
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

protected:
  
  virtual int_type underflow()
  {
    int_type c = Tr::eof();
    if(_sb)
    {
      int result = Z_OK;

      if(_zlib_stream.avail_out==0)
      {
        _zlib_stream.next_out = (Bytef*)_obuf;
        _zlib_stream.avail_out = BUFFER_LENGTH * sizeof(Ch);
      }

      unsigned int bytes_in_obuf = BUFFER_LENGTH * sizeof(Ch) - _zlib_stream.avail_out;
      Ch *obuf_read_begin = _obuf + (bytes_in_obuf / sizeof(Ch));
      Ch *obuf_read_end   = obuf_read_begin;

      while((obuf_read_begin==obuf_read_end) && (result==Z_OK))
      {
        if(_zlib_stream.avail_in==0)
        {
          _zlib_stream.next_in  = (Bytef*)_ibuf;
          _zlib_stream.avail_in = _sb->sgetn(_ibuf, BUFFER_LENGTH);
        }
        
        result = inflate(&_zlib_stream, Z_SYNC_FLUSH);
        bytes_in_obuf = BUFFER_LENGTH * sizeof(Ch) - _zlib_stream.avail_out;
        obuf_read_end = _obuf + (bytes_in_obuf / sizeof(Ch));
      }

      if(result==Z_OK)
      {
        c = traits_type::to_int_type(*obuf_read_begin);
        setg(obuf_read_begin, obuf_read_begin, obuf_read_end);
      }
      else if(result==Z_STREAM_END)
      {
        if(obuf_read_begin!=obuf_read_end)
        {
          c = traits_type::to_int_type(*obuf_read_begin);
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

  virtual int_type uflow()
  {
    int_type c = underflow();
    if(c!=traits_type::eof()) std::streambuf::sbumpc();
    return c;
  }

private:

  std::basic_streambuf<Octet> *_sb;

  static const unsigned int BUFFER_LENGTH = 4096;
  Octet _ibuf[BUFFER_LENGTH];
  Ch    _obuf[BUFFER_LENGTH];

  z_stream _zlib_stream;
};

typedef BasicInflatingInputStreamBuf<char> InflatingInputStreamBuf;

} // namespace Faw

#endif

#endif
