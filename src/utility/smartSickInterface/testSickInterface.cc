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

#include "smartSickInterface.hh"

using namespace std;
using namespace Smart;

int main(int argc, char **argv)
{
  if(argc<6)
  {
    cerr << "usage:" << endl
         << argv[0] << endl
         << "\tlength unit [mm]: {1|10|100}" << endl
         << "\tresolution [0.01deg]: {25|50|100}" << endl
         << "\twidth [deg]: {100|180}" << endl
         << "\tbitrate [bps]: {9600|19200|38400}" << endl
         << "\tnum scans [int]" << endl;
    return 1;
  }

  Smart::SickInterface sick;

  sick.verbose = true;
  sick.length_unit = atoi(argv[1]);
  sick.resolution = atoi(argv[2]);
  sick.width = atoi(argv[3]);
  sick.bitrate = atoi(argv[4]);
  sick.term_bitrate = sick.bitrate;
  unsigned int num_scans = atoi(argv[5]);
  
  //if(sick.open_device("/dev/ttyS0"))
  if(sick.open_device("/dev/ttyUSB0"))
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
      cerr << "probe_sick_speed failed." << endl;
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
      cout << "time = " << (ts.tv_sec%1000) << "sec, " << (ts.tv_usec/1000) << "msec" << endl;
      cout << "length unit = " << sick.extract_length_unit() << endl;
      const unsigned int n = sick.extract_num_points();
      cout << "num points = " << n << endl;
      for(unsigned int i=0; i<3; ++i)
      {
        cout << i << "\t" << sick.extract_distance(i) << "\t" << sick.extract_intensity(i) << endl;
      }
      ++count;
    }
    else
    {
      cerr << "received unexpected packet type" << endl;
    }
  }

  sick.close_device();

  return 0;
}
