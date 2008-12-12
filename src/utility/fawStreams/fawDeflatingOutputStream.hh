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
 
#ifndef FAW_DEFLATING_OUTPUT_STREAM_HH
#define FAW_DEFLATING_OUTPUT_STREAM_HH

#if (__GNUC__ < 3)
#include "fawDeflatingOutputStream.gcc2.hh"
#else

#include "fawDeflatingOutputStreamBuf.hh"

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch>, class Octet = char>
class BasicDeflatingOutputStream: public std::basic_ostream<Ch,Tr>
{
public:
  BasicDeflatingOutputStream(std::basic_streambuf<Octet> *sb = 0, int level = Z_DEFAULT_COMPRESSION)
  : std::basic_ostream<Ch,Tr>(0), _sb(sb, level) 
  {
    this->init(&_sb);
  }
    
  BasicDeflatingOutputStream(std::basic_ostream<Octet> &os, int level = Z_DEFAULT_COMPRESSION)
  : std::basic_ostream<Ch,Tr>(0), _sb(os, level) 
  {
    this->init(&_sb);
  }

  inline void open(std::basic_streambuf<Octet> *sb, int level = Z_DEFAULT_COMPRESSION) 
  { 
    _sb.open(sb, level); 
  }

  inline void open(std::basic_ostream<Octet> &os, int level = Z_DEFAULT_COMPRESSION) 
  { 
    _sb.open(os, level); 
  }

  inline bool is_open() const
  { 
    return _sb.is_open(); 
  }
  
  inline void close() 
  { 
    _sb.close(); 
  }
  
  inline float ratio() const 
  { 
    return _sb.ratio(); 
  }

  inline unsigned int uncompressed_bytes_in() const 
  { 
    return _sb.uncompressed_bytes_in(); 
  }

  inline unsigned int compressed_bytes_out() const 
  { 
    return _sb.compressed_bytes_out(); 
  }

private:
  BasicDeflatingOutputStreamBuf<Ch,Tr,Octet> _sb;
};

typedef BasicDeflatingOutputStream<char> DeflatingOutputStream;

} // namespace Faw

#endif

#endif
