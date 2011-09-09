// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Christian Schlegel, Alex Lotz
//
//        schlegel@hs-ulm.de
//        lotz@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------


#include "ROS_LaserScan.hh"

// initialize static members...
CHS::SmartMutex ROS_LaserScan::mutex;
CommBasicObjects::CommMobileLaserScan ROS_LaserScan::curr_scan;
unsigned long ROS_LaserScan::scan_count = 0;
//nav_msgs::Odometry ROS_LaserScan::raw_odometry;
//nav_msgs::Odometry ROS_LaserScan::gps_odometry;
