// --------------------------------------------------------------------------
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
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
//  Small test program to see if your joystick interface is working.
//  This program is in the public domain.
//
// --------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/joystick.h>

int main(int argc, char **argv)
{
  int fd;
  int n;
  struct js_event e;
  
  fd = open(argv[1], O_RDONLY);
  
  if(fd<0)
  {
    perror("open()");
    return 1;
  }

  while(1)
  {
    n = read(fd, &e, sizeof(js_event));
    if(n>0)
    {
      printf("%d %d %d %d\n", e.time, e.value, e.type, e.number);
    }
  }
  
  return 0;
};
