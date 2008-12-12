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
 
#include "fawInflatingInputStreamBuf.hh"

namespace Faw {

class InflatingInputStream: public std::istream
{
public:
  InflatingInputStream(std::streambuf *sb = 0) 
  : std::istream(0), _sb(sb) 
  {
    this->init(&_sb);
  }

  InflatingInputStream(std::istream &is)
  : std::istream(0), _sb(is) 
  {
    this->init(&_sb);
  }

  inline void open(std::streambuf *sb) 
  {
    _sb.open(sb); 
  }

  inline void open(std::istream &is)
  { 
    _sb.open(is); 
  }

  inline bool is_open() const 
  {
    return _sb.is_open(); 
  }

  inline void close() 
  {
    _sb.close();
  }

private:
  InflatingInputStreamBuf _sb;
};

} // namespace Faw
