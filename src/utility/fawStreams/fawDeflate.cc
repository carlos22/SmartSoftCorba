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
#include <string>

#include "fawDeflatingOutputStream.hh"

void deflate(std::istream &is, std::ostream &os)
{
  char ch;
  Faw::DeflatingOutputStream dos(os,9);
  while(true)
  {
    if(is.get(ch).eof()) break;
    dos.put(ch);
  }
}

int main(int argc, char **argv)
{
  if(argc<2)
  {
    deflate(std::cin,std::cout);
  }
  else if(argc==2)
  {
    std::string in_file = argv[1];
    std::string out_file = in_file + ".deflated";
    std::ifstream ifs(in_file.c_str());
    std::ofstream ofs(out_file.c_str());
    deflate(ifs, ofs);
  }
  else
  {
    std::cerr << "Usage: " << argv[0] << " [<filename>]" << std::endl;
    return 1;
  }
  return 0;
}
