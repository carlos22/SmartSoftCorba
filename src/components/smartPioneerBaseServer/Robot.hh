//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
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
//  This work is based on previous work by the folks from PlayerStage.
//
//--------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// CREDITS:
//
// The code for the communication with the pioneer robot was taken from the 
// Playerstage Project, which is distributed under GPL, and you can find at 
// http://playerstage.sourceforge.net/
//
// Player - One Hell of a Robot Server
// Copyright (C) 2000  
//    Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
//
//----------------------------------------------------------------------------
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//                   
//----------------------------------------------------------------------------


// header protection
#ifndef ROBOT_HH
#define ROBOT_HH

#include <string>
#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

// boost-Library for Matrix
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/operation.hpp>


#include "smartSoft.hh"
#include "commBasePosition.hh"
#include "commBasePositionUpdate.hh"

#include "packet.hh"

// different pioneer robot types
#define  ROBOT_TYPE_P3DX_SH 0
#define  ROBOT_TYPE_P3DX 1
#define  ROBOT_TYPE_P3AT_SH 2


// defines taken from player (p2os.h) some additional commands added by Andreas Steck
#define P2OS_CYCLETIME_USEC 200000 // 20 ms
#define SYNC0 0
#define SYNC1 1
#define SYNC2 2

#define PULSE 0
#define OPEN 1
#define CLOSE 2
#define ENABLE 4
#define SETA 5
#define SETV 6
#define SETO 7
#define SETRV 10
#define VEL 11
#define RVEL 21
#define SETRA 23
#define SONAR 28
#define STOP 29
#define VEL2 32
#define GRIPPER 33
#define GRIPPERVAL 36
#define TTY2 42         // Added in AmigOS 1.2
#define GETAUX 43       // Added in AmigOS 1.2
#define BUMP_STALL 44
#define JOYDRIVE 47
#define GYRO 58         // Added in AROS 1.8
#define ROTKP 82        // Added in P2OS1.M
#define ROTKV 83        // Added in P2OS1.M
#define ROTKI 84        // Added in P2OS1.M
#define TRANSKP 85      // Added in P2OS1.M
#define TRANSKV 86      // Added in P2OS1.M
#define TRANSKI 87      // Added in P2OS1.M
#define TTY3 66         // Added in AmigOS 1.3
#define GETAUX2 67      // Added in AmigOS 1.3
#define ARM_INFO 70
#define ARM_STATUS 71
#define ARM_INIT 72
#define ARM_CHECK 73
#define ARM_POWER 74
#define ARM_HOME 75
#define ARM_PARK 76
#define ARM_POS 77
#define ARM_SPEED 78
#define ARM_STOP 79
#define ARM_AUTOPARK 80
#define ARM_GRIPPARK 81
#define SOUND 90
#define PLAYLIST 91
  
/* Argument types */
#define ARGINT          0x3B    // Positive int (LSB, MSB)
#define ARGNINT         0x1B    // Negative int (LSB, MSB)
#define ARGSTR          0x2B    // String (Note: 1st byte is length!!)

#define PLAYER_SONAR_MAX_SAMPLES 64

/**
 * \class Robot
 * Encapsulates the Robot
 */
class Robot : public CHS::SmartTask
{
public:

  /**
   * @brief Default constructor
   *
   */
  Robot(int robotType);

  /**
   * @brief Default destructor
   */
  ~Robot(void);

  /**
   * @brief Open serial port
   * 
   * @param serialPort    serial port of robot base (use smartPioneerBaseServer.ini to configure)
   * @param enableMotors  enable motors on startup (use smartPioneerBaseServer.ini to configure)
   * @param enableSonar   enable sonar on startupe (use smartPioneerBaseServer.ini to configure)
   * 
   * @return 0 for connected 
   */
  int openSerial(std::string serialPort, bool enableMotors, bool enableSonar );

  /**
   * @brief Close serial port
   */
  void closeSerial();

  
  /**
   * @brief Returns the CommBasePosition object (Position of the robot)
   *
   * @return Position of the Robot
   */
  Smart::CommBasePosition getBasePosition();

  /**
   * @brief Updates the current position of the robot
   *
   * @return ??
   */
  int updatePosition( Smart::CommBasePositionUpdate update );

  /**
   * @brief Returns the voltage of the battery
   *
   * @return Battery charge in volt
   */
  double getBatteryVoltage();

  /**
   * @brief Returns the velocity of the robot in mm/sec
   *
   * @return Velocity of the Robot in mm/sec
   */
  int getV();

  /**
   * @brief Sets the velocity of the robot in mm/sec
   *
   * @param v Velocity of the robot in mm/sec
   */
  void setV( int v );

  /**
   * @brief Returns the rotational velocity of the robot in rad/sec
   *
   * @return Rotational velocity of the Robot in radiant/sec
   */
  double getOmegaRad();

  /**
   * @brief Sets the rotational velocity of the robot in rad/sec
   *
   * @param omega Rotational velocity of the Robot in radiant/sec
   */
  void setOmegaRad( double omega );

  /**
   * @brief Returns the rotational velocity of the robot in degree/sec
   *
   * @return Rotational velocity of the Robot in degree/sec
   */
  double getOmegaDeg();

  /**
   * @brief Sets the rotational velocity of the robot in degree/sec
   *
   * @param omega Rotational velocity of the Robot in degree/sec
   */
  void setOmegaDeg( double omega );

  /**
   * @brief Returns the total distance the robot has travelled since startup in mm
   *
   * @return The distance the robot has travelled since startup in mm
   */
  double getDistance();
  
  /**
   * @brief Returns the total left rotation the robot has travelled since startup in radiant
   *
   * @return The total left rotation the robot has travelled since startup in radiant
   */
  double getTotalRotationLeft();
  
  /**
   * @brief Returns the total right rotation the robot has travelled since startup in radiant
   *
   * @return The total right rotation the robot has travelled since startup in radiant
   */
  double getTotalRotationRight();

  /**
   * @brief Returns whether the left wheel is stall (blocked)
   *
   * @return whether left wheel ist stall (blocked) 
   */
  bool isLeftWheelStall();


  /**
   * @brief Returns whether the right wheel is stall (blocked)
   *
   * @return whether right wheel ist stall (blocked)
   */
  bool isRightWheelStall();

  void setParameters( int maxVel, int maxRotVel, int maxVelAcc, int maxVelDecel, int maxRotVelAcc, int maxRotVelDecel );


/////////////////////////////// private
private:
  double DiffConvFactor;//  = 0.0056;
  double DistConvFactor;//  = 0.485;
  double AngleConvFactor;// = 0.001534;
  double VelConvFactor;//   = 1.0;
  double RangeConvFactor;// = 1.0;

  // thread
  int svc(void);
  
  // from player
  int positionChange( unsigned short, unsigned short );

  //from player
  void parse( unsigned char *buffer );


  //probabilistic
  bool updateCovMatrix( Smart::CommBasePosition pos0,
                        Smart::CommBasePosition &pos1 ); 

  double piToPiRad( double a );
 
  // serialPort filedescriptor
  int serial_fd;

  bool updateV, updateOmega;
  int newV, newOmega;

  int maxVel, maxRotVel;

  Smart::CommBasePosition robotPos, oldPos;
  CHS::SmartMutex mutexRobotPos;


  //
  // from player
  // variable for sip-package Infos
 
  // left-/right wheel stall (blocked) 
  bool lwstall, rwstall;

  // status
  unsigned char status;

  // battery charge in tenths of volts
  unsigned char battery;

  // number of new sonar readings
  unsigned char sonarreadings;
 
  // analog input
  unsigned char analog;

  // digital input
  unsigned char digin;

  // digital output
  unsigned char digout;

  // flags !?!
  unsigned short ptu;

  // compass heading
  unsigned short compass;

  // don't know 
  unsigned short timer;

  // old xpos, ypos and th -> necessary for calcutaling deltas
  unsigned short oldxpos, oldypos, oldth; // by Andreas Steck

  // bumpers
  unsigned short frontbumpers, rearbumpers;

  // velocity of left/right wheel in mm/sec
  short lvel, rvel;

  // ??
  short control;

  // sonar values
  unsigned short sonars[PLAYER_SONAR_MAX_SAMPLES];

  // total distance the robot is travelled
  double totalDistance;

  // absolut left rotation the robot is travelled
  double totalRotationLeft;
  
  // absolut right rotation the robot is travelled
  double totalRotationRight;

  // probabilistic
  double lamdaSigmaD;
  double lamdaSigmaDeltaAlpha;
  double lamdaSigmaDeltaBeta;
  
  

};

#endif // ROBOT_HH

