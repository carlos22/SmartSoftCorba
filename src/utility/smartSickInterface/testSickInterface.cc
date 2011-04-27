//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge, Andreas Steck
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License 
//  as published by the Free Software Foundation; either version 2.1 
//  of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along 
//  with this library; if not, write to the Free Software Foundation, Inc., 
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//  This work is based on previous work by Christian Schlegel, FAW,
//  and on the work by the folks from PlayerStage.
//
//--------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// CREDITS:
//
// The laser code was taken from the Playerstage Project,
// which is distributed under GPL, and you can find at 
// http://playerstage.sourceforge.net/
//
// The PlayerStage Project:
//   Copyright (C) 2000  
//      Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
//                   
//----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "smartSickInterface.hh"

using namespace std;
using namespace Smart;

int main(int argc, char **argv)
{
  if(argc<6)
  {
    cerr << "usage:" << endl
         << argv[0] << endl
         << "\tdevice: {z.B. /dev/ttyUSB0}" << endl
         << "\tlength unit [mm]: {1|10|100}" << endl
         << "\tresolution [0.01deg]: {25|50|100}" << endl
         << "\twidth [deg]: {100|180}" << endl
         << "\tbitrate [bps]: {9600|38400|500000}" << endl
         << "\tnum scans [int]" << endl;
    return 1;
  }

  Smart::SickInterface sick;

  sick.verbose = false;
  std::string device = argv[1];
  sick.length_unit = atoi(argv[2]);
  sick.resolution = atoi(argv[3]);
  sick.width = atoi(argv[4]);
  sick.bitrate = atoi(argv[5]);
  sick.term_bitrate = sick.bitrate;
  unsigned int num_scans = atoi(argv[6]);

  std::cout << "device: " << device << std::endl;

  unsigned long long start = 0, stop = 0;
  
  //if(sick.open_device("/dev/ttyS0"))
  //if(sick.open_device("/dev/ttyUSB0"))
  if(sick.open_device(device.c_str()))
  {
    cerr << "open_device failed." << endl;
    return 1;
  }

  while(true)
  {
    unsigned int sick_speed = 0;
    string sick_type;
    if(sick.probe_sick_speed(sick_speed, sick_type))
    {
      //cerr << "probe_sick_speed failed." << endl;
      continue;
    }

    cout << endl << "Connected to \"" << sick_type << "\" at " << sick_speed << "bps" << endl << endl;

    if(sick.set_laser_config())
    {
      cerr << "set_laser_config failed." << endl;
      continue;
    }

    cout << "Laser configured." << endl;

    if(sick.request_laser_data()==0) break;
  }

  unsigned int count = 0;
  while(count < num_scans)
  {
    if (sick.read_laser_data()==0)
    {
      timeval ts = sick.get_receive_timestamp();
      if(count == 0) start = ts.tv_sec * 1000000 + ts.tv_usec;
      if(count == num_scans-1) stop = ts.tv_sec * 1000000 + ts.tv_usec;

      const unsigned int n = sick.extract_num_points();
      cout << "id = " << count << "  ; num points = " << n << endl;
      ++count;
    }
    else
    {
      cerr << "received unexpected packet type" << endl;
    }
  }

  std::cout << "Frequency is " << (double)num_scans / (double)(stop-start) * 1000000.0 << " Hz.\n";

  sick.close_device();

  return 0;
}
