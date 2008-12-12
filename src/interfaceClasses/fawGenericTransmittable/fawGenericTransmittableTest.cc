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
 
#include <fstream>

#include "fawDeflatingOutputStream.hh"
#include "fawInflatingInputStream.hh"
#include "fawMemoryStreamBuf.hh"
#include "fawDynamicBuffer.hh"
#include "fawGenericTransmittable.hh"

class FileContents
{
public:
  FileContents() {}
  FileContents(std::istream &is)
  {
    load(is);
  }

  void save(std::ostream &os) const
  {
    _text.save(os);
  }

  void load(std::istream &is)
  {
    _text.load(is);
  }

  void read(std::istream &is)
  {
    char ch;
    _text.clear();
    while(true)
    {
      if(is.get(ch).eof()) break;
      _text.append(ch);
    }
  }

  void print(std::ostream &os) const
  {
    Faw::DynamicBuffer::const_iterator iter = _text.begin();
    while(iter!=_text.end()) os.put(*iter++);
  }

  unsigned int size() const
  {
    return _text.size();
  }

private:
  Faw::DynamicBuffer _text;
};

int main()
{
  CORBA::Any a;

  Faw::GenericTransmittable<FileContents,true> fc1;
  fc1.read(std::cin);
  fc1.get(a);
  
  ///// transmit "a"

  ///// receive "a"

  Faw::GenericTransmittable<FileContents,true> fc2;
  fc2.set(a);
  fc2.print(std::cout);

  return 0;
}
