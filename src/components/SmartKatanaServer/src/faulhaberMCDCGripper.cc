// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Andreas Steck
//
//        steck@hs-ulm.de
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
//
// --------------------------------------------------------------------------

#include "faulhaberMCDCGripper.hh"

/////////////////////////////////////////////
// PRIVATE


// open serial port
void FaulhaberMCDCGripper::open_port(std::string port)
{
        struct termios term;

        if((fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY )) < 0 )
        {
           printf("FaulhaberMCDCGripper::open_port -- can not open serial port: %s \n", port.c_str());
           exit(1);
        }

        if(fcntl(fd, F_SETFL, 0) < 0)
        {
          printf("FaulhaberMCDCGripper::open_port -- blocking\n");
          ::close(fd);
          fd = -1;
          exit(1);
        }

        if(tcgetattr( fd, &term ) < 0 )
        {
          printf("FaulhaberMCDCGripper::open_port -- tcgetattr()\n");
          ::close(fd);
          fd = -1;
          exit(1);
        }

        cfmakeraw( &term );
        cfsetispeed(&term, B9600);
        cfsetospeed(&term, B9600);

        if(tcsetattr(fd, TCSAFLUSH, &term ) < 0)
        {
          printf("FaulhaberMCDCGripper::open_port -- tcsetattr() \n");
          ::close(fd);
          fd = -1;
          exit(1);
        }

        if(tcflush(fd, TCIOFLUSH ) < 0)
        {
          perror("FaulhaberMCDCGripper::open_port -- tcflush()\n");
          ::close(fd);
          fd = -1;
          exit(1);
        }

        printf("Connected to Faulhaber MCDC 3006\n");

}



// close serial port
void FaulhaberMCDCGripper::close_port()
{
  ::close(fd);
  fd = -1;
  printf("Connection to Faulhaber MCDC 3006 has been closed\n");
}


// send string on serial port
int FaulhaberMCDCGripper::serial_send(std::string str)
{
  int rd;
  char buff[16]; 
  const char* data = str.c_str();

  write(fd, data, strlen(data));

  char rtrn[3];
  int i = 0;
  do
  {
    rd = read(fd, buff, 16);

    strncpy(rtrn, buff, 2);
    rtrn[2] = '\0';
    //std::cout << "serial_send() " << i++ << "  -- " << rtrn << std::endl;
  } while( !strcasecmp(rtrn, "OK") == 0 );
  
  return 0;
}

// receive string from serial port
std::string FaulhaberMCDCGripper::serial_read()
{
  int rd;
  char buff[16];

  rd = read(fd, buff, 16);

  char rtrn[rd+1];
  strncpy(rtrn, buff, rd);
  rtrn[rd] = '\0';

  std::string str(rtrn);

  return str;
}




/////////////////////////////////////////////
// PUBLIC

// constructor
FaulhaberMCDCGripper::FaulhaberMCDCGripper() 
{ }

// destructor
FaulhaberMCDCGripper::~FaulhaberMCDCGripper() 
{
  disable();
  close_port();
}

// connect to controller
void FaulhaberMCDCGripper::connect(std::string port)
{
  open_port(port);  
}

// wait for position reached || v = 0
int FaulhaberMCDCGripper::wait_pv(int v)
{
  std::stringstream s;

  // notify v
  s.str("");
  s << "NV" << v << "\n";
  serial_send(s.str());

  std::string rtrn;;

  do
  {
    rtrn = serial_read();
    //std::cout << "wait_pv() -- " << rtrn << std::endl;
  } while( !strcasecmp(rtrn.c_str(), "V") );

  return 0;
}


// read current gripper position
int FaulhaberMCDCGripper::read_pos()
{
  int rd;
  char buff[16];
  const char* data = "POS\n";

  write(fd, data, strlen(data));

  rd = read(fd, buff, 16);

  char rtrn[rd+1];
  strncpy(rtrn, buff, rd);
  rtrn[rd] = '\0';

  int i;
  sscanf(rtrn, "%d", &i);
  return i;
}


// calibrate gripper -> set encoder for opened gripper to 31000
void FaulhaberMCDCGripper::calibrate(int max_speed, int max_current)
{
  std::stringstream s;

  std::cout << "gripper_calibrate...\n";
  serial_send("ENCRES128\n");
  s.str("");
  s << "SP" << max_speed << "\n";
  serial_send(s.str()); 
 
  s.str("");
  s << "LPC" << max_current << "\n";
  serial_send(s.str());  

  serial_send("EN\n");
  serial_send("V30000\n");
  wait_pv(10);
  serial_send("V0\n");
  serial_send("HO31000\n");

  serial_send("LA15000\n");
  serial_send("M\n");
  wait_pv(-10);
  serial_send("V0\n");

  std::cout << "calibrated\n";
}

// open gripper
void FaulhaberMCDCGripper::open()
{
  serial_send("EN\n");
  serial_send("V30000\n");
  wait_pv(10);
  serial_send("V0\n");
}


// close gripper
void FaulhaberMCDCGripper::close()
{
  serial_send("EN\n");
  serial_send("V-30000\n");
  wait_pv(-10);
  serial_send("V0\n");
}


// gripper unblocked -> motor released
void FaulhaberMCDCGripper::disable()
{
  serial_send("DI\n");
}


// gripper unblocked -> motor released
void FaulhaberMCDCGripper::enable()
{
  serial_send("EN\n");
}


// move gripper to pos (encoder)
void FaulhaberMCDCGripper::move_to_pos(int pos)
{
  std::stringstream s;

  int currpos = read_pos();
  int v=0;
  if( pos - currpos > 0 ) v = 10; else v = -10;

  serial_send("EN\n");

  s.str("");
  s << "LA" << pos << "\n";
  serial_send(s.str());

  serial_send("M\n");
  wait_pv(v);
  serial_send("V0\n");
}

