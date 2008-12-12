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
 
#ifndef FAW_DYNAMIC_BUFFER_COMM_HH
#define FAW_DYNAMIC_BUFFER_COMM_HH

#include <string>

#include "fawDynamicBuffer.hh"
#include "fawMemoryStreamBuf.hh"
#include "fawBufferInputStreamBuf.hh"
#include "fawDeflatingOutputStream.hh"
#include "fawInflatingInputStream.hh"

#include "fawStreamOctetSequenceC.hh"

namespace Faw {

class DynamicBufferComm: public DynamicBuffer
{
public:

  inline void get(CORBA::Any &a) const
  {
    FawStreamOctetSequence seq;
    const unsigned int len = this->size();
    seq.data.length(len);
    DynamicBuffer::const_iterator iter = this->begin();
    for(unsigned int i=0; i<len; ++i)
    {
      seq.data[i] = *iter;
      ++iter;
    }
    a <<= seq;
  }

  inline void set(const CORBA::Any &a)
  {
    this->clear();
    FawStreamOctetSequence *pseq = 0;
    if(a >>= pseq)
    {
      const unsigned int len = pseq->data.length();
      for(unsigned int i=0; i<len; ++i) this->append(pseq->data[i]);
    }
  }

  inline static std::string identifier() 
  {
    return "FAW::DynamicBufferComm";
  };
};

} // namespace Faw

#endif
