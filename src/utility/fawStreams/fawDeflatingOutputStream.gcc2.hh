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
 
#include "fawDeflatingOutputStreamBuf.hh"

namespace Faw {

class DeflatingOutputStream: public std::ostream
{
public:
  DeflatingOutputStream(std::streambuf *sb = 0, int level = Z_DEFAULT_COMPRESSION)
  : std::ostream(0), _sb(sb, level) 
  {
    this->init(&_sb);
  }
    
  DeflatingOutputStream(std::ostream &os, int level = Z_DEFAULT_COMPRESSION)
  : std::ostream(0), _sb(os, level) 
  {
    this->init(&_sb);
  }

  inline void open(std::streambuf *sb, int level = Z_DEFAULT_COMPRESSION) 
  { 
    _sb.open(sb, level); 
  }

  inline void open(std::ostream &os, int level = Z_DEFAULT_COMPRESSION) 
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
  DeflatingOutputStreamBuf _sb;
};

} // namespace Faw
