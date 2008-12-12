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
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#include "fawFileDesInputStream.hh"
#include "fawFileDesOutputStream.hh"

int main(int argc, char **argv)
{
  if(argc<2)
  {
    std::cerr << "Send a file to /dev/null" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  int in_fd = open(argv[1], O_RDONLY);
  int out_fd = open("/dev/null", O_WRONLY);
  if((in_fd>0) && (out_fd>0))
  {
    Faw::FileDesInputStream is(in_fd);
    Faw::FileDesOutputStream os(out_fd);
    unsigned int n = 0;
    char ch;
    while(true)
    {
      if(is.get(ch).eof()) break;
      os.put(ch);
      ++n;
    }
    std::cout << "Sent " << n << " bytes to /dev/null" << std::endl;
  }

  return 0;
}
