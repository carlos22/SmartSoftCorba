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
 
#include <iostream>
#include <fstream>

#include "fawMemoryStreamBuf.hh"
#include "fawInflatingInputStream.hh"
#include "fawDeflatingOutputStream.hh"

int main()
{
  char ch;
  Faw::DynamicBuffer buf1;
  Faw::MemoryStreamBuf sb1(buf1);

  Faw::DeflatingOutputStream os(&sb1);
  while(true)
  {
    if(std::cin.get(ch).eof()) break;
    os.put(ch);
  }
  os << std::flush;

  std::ofstream ofs("test.tmp");
  buf1.save(ofs);
  ofs.close();

  ////

  Faw::DynamicBuffer buf2;
  Faw::MemoryStreamBuf sb2(buf2);
  std::ifstream ifs("test.tmp");
  buf2.load(ifs);
  ifs.close();

  Faw::InflatingInputStream is(&sb2);
  while(true)
  {
    if(is.get(ch).eof()) break;
    std::cout.put(ch);
  }
  return 0;
}
