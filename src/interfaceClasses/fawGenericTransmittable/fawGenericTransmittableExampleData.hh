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


#ifndef FAW_EXAMPLE_DATA_HH
#define FAW_EXAMPLE_DATA_HH

#include <string>
#include <iostream>

namespace Faw {

/**
  Dummy class with \c load and \c save serialization methods.
 */
class ExampleData
{
public:
  ExampleData() {}
  
  inline void set_string(const std::string &s) { _s = s; }
  inline const std::string &get_string() const { return _s; }
  
  inline void set_double(double d) { _d = d; }
  inline double get_double() const { return _d; }

  inline void set_int(int i) { _i = i; }
  inline int get_int() const { return _i; }
  
  inline void save(std::ostream &os) const
  {
    os << _s.length() << " " << _s << std::endl;
    os << _d << "\t" << _i << std::endl;
  }

  inline void load(std::istream &is)
  {
    unsigned int n;
    char space;
    is >> n;
    is.get(space);
    _s.resize(n);
    for(unsigned int i=0; i<n; ++i) is.get(_s[i]);
    is >> _d >> _i;
  }

  inline static std::string identifier()
  {
    return "Faw::GenericTransmittableExampleData";
  }

  inline void print(std::ostream &os = cout) const
  {
    os << "ExampleData(s=\"" << _s << "\",d=" << _d << ",i=" << _i << ")";
  }

private:
  std::string _s;
  double _d;
  int _i;
};

inline std::ostream &operator<<(std::ostream &os, const ExampleData &d)
{
  d.print(os);
  return os;
}

} // namespace Faw

#endif
