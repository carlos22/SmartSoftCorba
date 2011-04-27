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

#include <iostream>
#include <string>

#include "smartKnuthMorrisPratt.hh"

using namespace Smart;

int main(int argc, char **argv)
{
  if(argc<2) return -1;
  KnuthMorrisPratt kmp(argv[1]);
  if(kmp.search(std::cin))
  {
    std::cout << "pattern found." << std::endl;
    std::cout << "rest of stream:" << std::endl;
    char ch;
    while(true)
    {
      if(std::cin.get(ch).eof()) break;
      std::cout.put(ch);
    }
    std::cout << std::endl;
    return 0;
  }
  std::cout << "pattern not found." << std::endl;
  return -1;
}
