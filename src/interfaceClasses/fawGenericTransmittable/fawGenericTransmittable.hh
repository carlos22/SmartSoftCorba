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
 
#ifndef FAW_GENERIC_TRANSMITTABLE_HH
#define FAW_GENERIC_TRANSMITTABLE_HH

#include <string>
#include <iterator>

#include "fawDynamicBuffer.hh"
#include "fawMemoryStreamBuf.hh"
#include "fawBufferInputStreamBuf.hh"
#include "fawDeflatingOutputStream.hh"
#include "fawInflatingInputStream.hh"

#include "fawStreamOctetSequenceC.hh"

namespace Faw {

/**
  Wrapper to make classes transmittable if they provide \c save and \c load
  methods for serialization.
 */
template<class T, bool compress = true>
class GenericTransmittable: public T
{
public:

  template<class Array, class Element>
  class ArrayInputIterator
  {
  public:

    typedef std::input_iterator_tag iterator_category;
    typedef Element value_type;
    typedef size_t difference_type;
    typedef const Element *pointer;
    typedef const Element &reference;

    inline ArrayInputIterator()
    : _array(0), _index(0)
    {}

    inline ArrayInputIterator(const Array &array, unsigned int index = 0)
    : _array(&array), _index(index)
    {}
    
    inline bool operator==(const ArrayInputIterator<Array,Element> &other) const
    {
      return (other._array==_array) && (other._index==_index);
    }

    inline bool operator!=(const ArrayInputIterator<Array,Element> &other) const
    {
      return !(*this==other);
    }

    inline ArrayInputIterator<Array,Element> operator++(int)
    {
      ArrayInputIterator<Array,Element> iter = *this;
      ++_index;
      return iter;
    }
    
    inline ArrayInputIterator<Array,Element> &operator++()
    {
      ++_index;
      return *this;
    }
    
    inline Element operator*() const
    {
      return (*_array)[_index];
    }
    
    inline void print(std::ostream &os = std::cout) const
    {
      os << "ArrayInputIterator(" << (void*)_array << ", " << _index << ")";
    }

  private:
    const Array *_array;
    unsigned int _index;
  };

  inline GenericTransmittable() : T() {}

  inline GenericTransmittable(const T &data) 
  : T(data) {}

  inline GenericTransmittable<T,compress> &operator=(const T &data)
  {
    T::operator=(data);
    return *this;
  }

  inline void get(CORBA::Any &a) const
  {
    FawStreamOctetSequence seq;
    DynamicBuffer buf;
    if(compress)
    {
      MemoryStreamBuf sb(buf);
      DeflatingOutputStream os(&sb);
      this->save(os);
      os.close();
    }
    else
    {
      MemoryStreamBuf sb(buf);
      std::ostream os(&sb);
      this->save(os);
    }
    const unsigned int len = buf.size();
    seq.data.length(len);
    DynamicBuffer::const_iterator iter = buf.begin();
    for(unsigned int i=0; i<len; ++i)
    {
      seq.data[i] = *iter;
      ++iter;
    }
    a <<= seq;
  }

  inline void set(const CORBA::Any &a)
  {
    FawStreamOctetSequence *pseq = 0;
    if(a >>= pseq)
    {
      const unsigned int len = pseq->data.length();
      ArrayInputIterator<FawStreamOctetSequence::_data_seq,char> begin(pseq->data, 0);
      ArrayInputIterator<FawStreamOctetSequence::_data_seq,char> end(pseq->data, len);
      BufferInputStreamBuf<ArrayInputIterator<FawStreamOctetSequence::_data_seq,char> > sb(begin,end);
      if(compress)
      {
        InflatingInputStream is(&sb);
        this->load(is);
      }
      else
      {
        std::istream is(&sb);
        this->load(is);
      }
    }
  }

  static inline std::string identifier(void) 
  {
    return std::string("FAW::GenericTransmittable<") + T::identifier() + std::string(">");
  };

};

} // namespace Faw

#endif
