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

#ifndef ROS_ODOMETRY_H_
#define ROS_ODOMETRY_H_

#include "gen/SmartROSBaseGateway.hh"

class ROS_Odomentry
{
public:
	/*
	static void odometryGpsCallback(const nav_msgs::Odometry &odom)
	{
		COMP->GlobalLock.acquire();
			COMP->gps_odometry = odom;
		COMP->GlobalLock.release();
	}
	*/

	static void odometryRawCallback(const nav_msgs::Odometry &odom)
	{
		COMP->GlobalLock.acquire();
			COMP->raw_odometry = odom;
		COMP->GlobalLock.release();
	}
};

#endif /* ROS_ODOMETRY_H_ */
