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
 
#ifndef FAW_DYNAMIC_BUFFER_HH
#define FAW_DYNAMIC_BUFFER_HH

#if (__GNUC__ < 3)
#include "fawDynamicBuffer.gcc2.hh"
#else

#include <cctype>
#include <ios>
#include <streambuf>
#include <iterator>

namespace Faw {

template<class Ch, class Tr = std::char_traits<Ch> >
class BasicDynamicBuffer
{
public:

  typedef Ch char_type;
  typedef Tr traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  struct Buffer
  {
    inline Buffer() : len(0), succ(0) {}
    static const unsigned int _SIZE = 256; // 2^n
    static const unsigned int SIZE = (_SIZE - sizeof(unsigned int) - sizeof(Buffer*)) / sizeof(Ch);
    Ch data[SIZE];
    unsigned int len;
    Buffer *succ;
  };

  class ConstIterator
  {
  public:
    typedef std::input_iterator_tag iterator_category;
    typedef Ch value_type;
    typedef size_t difference_type;
    typedef const Ch *pointer;
    typedef const Ch &reference;
    
    ConstIterator(const Buffer *b = 0, unsigned int n = 0, unsigned int i = 0) : _b(b), _n(n), _i(i) {}
    ConstIterator operator++(int);
    inline ConstIterator &operator++();
    inline const Ch &operator*() const;
    /**
      Get up to \a max_count character starting from current iterator position,
      and move iterator to next unseen character.
     */
    inline unsigned int get(Ch *buffer, unsigned int max_count);
    inline bool operator==(const ConstIterator &other) const;
    inline bool operator!=(const ConstIterator &other) const { return !(*this == other); } 

  private:
    const Buffer *_b;      // current buffer
    unsigned int _n; // index of current buffer
    unsigned int _i; // index of current character in current buffer
  };

  typedef ConstIterator const_iterator;
  
  BasicDynamicBuffer() 
    : _first(), _last(&_first), _num_buffers(1)
  {
  }

  BasicDynamicBuffer(const BasicDynamicBuffer<Ch,Tr> &src)
    : _first(), _last(&_first), _num_buffers(1)
  {
    const Buffer *b = &(src._first);
    while(b)
    {
      append(b->data, b->len);
      b = b->succ;
    }
  }
  
  BasicDynamicBuffer &operator=(const BasicDynamicBuffer<Ch,Tr> &src) 
  {
    if(&src!=this)
    {
      clear();
      Buffer *b = src._first;
      while(b)
      {
        append(b->data, b->len);
        b = b->succ;
      }
    }
    return *this;
  }

  virtual ~BasicDynamicBuffer()
  {
    clear();
  }

  void clear()
  {
    Buffer *b = _first.succ;
    while(b)
    {
      Buffer *n = b->succ;
      delete b;
      b = n;
    }
    _first.len = 0;
    _last = &_first;
    _num_buffers = 1;
  }

  inline unsigned int size() const
  {
    return (_num_buffers-1) * Buffer::SIZE + _last->len;
  }

  inline ConstIterator begin() const 
  { 
    return ConstIterator(&_first, 0, 0);
  }

  inline ConstIterator end() const
  {
    return ConstIterator(_last, _num_buffers - 1, _last->len);
  }

  inline void append(Ch c)
  {
    const unsigned int l = _last->len++;
    _last->data[l] = c;
    if(_last->len==Buffer::SIZE)
    {
      Buffer *b = new Buffer;
      ++_num_buffers;
      _last->succ = b;
      _last = b;
    }
  }

  template<class Iterator>
  inline void append(const Iterator &begin, const Iterator &end)
  {
    Iterator iter = begin;
    while(iter!=end)
    {
      append(*iter);
      ++iter;
    }
  }

  inline void append(const Ch *data, unsigned int count)
  {
    unsigned int i = 0;
    while(i<count)
    {
      unsigned int l = _last->len;
      while((l<Buffer::SIZE) && (i<count)) _last->data[l++] = data[i++];
      _last->len = l;
      if(l==Buffer::SIZE)
      {
        Buffer *b = new Buffer;
        ++_num_buffers;
        _last->succ = b;
        _last = b;
        l = 0;
      }
    }
  }

  inline void dump(std::basic_ostream<Ch,Tr> &os) const
  {
    const Buffer *b = &_first;
    while(b)
    {
      os.write(b->data, b->len);
      b = b->succ;
    }
  }

  inline void save(std::basic_ostream<Ch,Tr> &os) const
  {
    unsigned int n = size();
    while(n>0)
    {
      if(n%2==0) os.put('0'); else os.put('1');
      n=n/2;
    }
    os.put(' ');
    dump(os);
  }

  inline void load(std::basic_istream<Ch,Tr> &is)
  {
    char_type ch;
    clear();

    unsigned int n = 0;
    unsigned int e = 1;
    while(true)
    {
      if(is.get(ch).eof())
      {
        n = 0; break;
      }
      else if(ch==' ')
      {
        break;
      }
      else 
      {
        if(ch=='1') n += e;
        e = 2*e;
      }
    }

    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    while((n>0) && is.good())
    {
      const unsigned int m = is.rdbuf()->sgetn(buffer, std::min(n, BUFFER_SIZE));
      append(buffer, m);
      n -= m;
    }
  }

  inline void print(std::ostream &os = std::cout) const
  {
    os << "BasicDynamicBuffer(size==" << size() << ")" << std::endl;
  }

private:
  Buffer _first;
  Buffer *_last;
  unsigned int _num_buffers;
};

typedef BasicDynamicBuffer<char> DynamicBuffer;

template<class Ch, class Tr>
inline typename BasicDynamicBuffer<Ch,Tr>::ConstIterator BasicDynamicBuffer<Ch,Tr>::ConstIterator::operator++(int) 
{
  ConstIterator iter = *this;
  if(++_i>=Buffer::SIZE)
  {
    _i = 0;
    ++_n;
    _b = _b->succ;
  }
  return iter;
}

template<class Ch, class Tr>
inline typename BasicDynamicBuffer<Ch,Tr>::ConstIterator &BasicDynamicBuffer<Ch,Tr>::ConstIterator::operator++()
{
  if(++_i>=Buffer::SIZE)
  {
    _i = 0;
    ++_n;
    _b = _b->succ;
  }
  return *this;
}

template<class Ch,class Tr>
inline const Ch &BasicDynamicBuffer<Ch,Tr>::ConstIterator::operator*() const 
{ 
  return _b->data[_i]; 
}

template<class Ch, class Tr>
inline unsigned int BasicDynamicBuffer<Ch,Tr>::ConstIterator::get(Ch *buffer, unsigned int max_count)
{
  unsigned int count = 0;
  while((count<max_count) && (_i<_b->len))
  {
    buffer[count++] = _b->data[_i++];
    if(_i==Buffer::SIZE)
    {
      _i = 0;
      ++_n;
      _b = _b->succ;
    }
  }
  return count;
}

template<class Ch, class Tr>
inline bool BasicDynamicBuffer<Ch,Tr>::ConstIterator::operator==(const BasicDynamicBuffer<Ch,Tr>::ConstIterator &other) const
{
  return (other._n == _n) && (other._i == _i);
}

} // namespace Faw

#endif

#endif
