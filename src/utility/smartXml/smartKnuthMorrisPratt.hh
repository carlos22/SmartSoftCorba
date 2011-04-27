//--------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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

#ifndef SMART_KNUTH_MORRIS_PRATT_HH
#define SMART_KNUTH_MORRIS_PRATT_HH

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Smart {

/**
  Knuth-Morris-Pratt text search.
 */ 
class KnuthMorrisPratt
{
public:
  KnuthMorrisPratt(const std::string &pattern)
  : _pattern(pattern),
    _next(pattern.length())
  {
    unsigned int i = 0;
    int j = -1;
    while(i<_pattern.length())
    {
      _next[i] = j;
      while((j>=0) && (_pattern[i]!=_pattern[j])) j = _next[j];
      ++i;
      ++j;
    }
  }

  inline bool search(std::istream &is, int &i) const
  {
    char ch;
    i = 0;
    int j = 0;
    while(j<(int)_pattern.length())
    {
      if(is.get(ch).eof()) return false;
      while((j>=0) && (ch!=_pattern[j])) j = _next[j];
      ++i;
      ++j;
    }
    return true;
  }

  inline bool search(std::istream &is) const
  {
    int dummy;
    return this->search(is, dummy);
  }
  
  inline bool search(const std::string &data, int &i) const
  {
    std::istringstream iss(data);
    return this->search(iss, i);
  }

  inline bool search(const std::string &data) const
  {
    std::istringstream iss(data);
    int dummy;
    return this->search(iss, dummy);
  }

private:
  const std::string _pattern;
  std::vector<int> _next;
};

} // namespace Smart

#endif
