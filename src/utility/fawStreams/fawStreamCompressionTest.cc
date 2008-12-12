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
 
#include <stdlib.h>

#include "fawDeflatingOutputStream.hh"
#include "fawInflatingInputStream.hh"

/*
  Primitive compression/expansion program:
  acts as compessing filter when called with one (integer) parameter 
  (the compression level), and acts as expanding filter when called 
  without any paramter. 
  So
    "cat foo | fawDeflatingStreamsTest 1 | fawDeflatingStreamsTest"
  is equivalent to
    "cat foo"
  but only a bit slower ;-)
*/

int main(int argc, char **argv)
{
  char ch;
  if(argc>1)
  {
    const int level = atoi(argv[1]);
    Faw::DeflatingOutputStream dos(std::cout, level);
    while(true)
    {
      if(std::cin.get(ch).eof()) break;
      dos.put(ch);
    }
    dos.close();
  }
  else
  {
    Faw::InflatingInputStream iis(std::cin);
    while(true)
    {
      if(iis.get(ch).eof()) break;
      std::cout.put(ch);
    }
  }
  return 0;
}
