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
#include <iostream>

int main(int argc, char **argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " <file1> <file2>" << std::endl;
    return -1;
  }
  
  std::ifstream ifs1(argv[1]);
  std::ifstream ifs2(argv[2]);
  
  unsigned int n = 0;
  bool are_different = false;
  char ch1,ch2;
  std::cout << std::hex;
  while(true)
  {
    ifs1.get(ch1);
    ifs2.get(ch2);
    
    if(ifs1.eof() || ifs2.eof()) break;
    
    if(ch1!=ch2)
    {
      std::cout << "byte 0x" << n << ": 0x" << static_cast<int>(ch1) << " vs. 0x" << static_cast<int>(ch2) << std::endl;
      are_different = true;
    }

    ++n;
  }
  if(!ifs1.eof())
  {
    std::cout << argv[2] << " shorter than " << argv[1] << std::endl;
    are_different = true;
  }
  else if(!ifs2.eof())
  {
    std::cout << argv[1] << " shorter than " << argv[2] << std::endl;
    are_different = true;
  }
  return are_different;
}
