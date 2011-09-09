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
//  This file is part of the "SmartSoft Mapper/GridMapper component".
//  It provides mapping services based on grid maps. The current map
//  represents the latest snapshot of the local surrounding based on
//  laserscans. The current map can be preoccupied by the longterm map.
//  The longterm map is a simple occupancy grid map.
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


#ifndef COMMDATATRANSLATOR_H_
#define COMMDATATRANSLATOR_H_

#include <smartSoft.hh>

// smartsoft communication objects
#include <CommBasicObjects/commBasePositionUpdate.hh>
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commMobileLaserScan.hh>
#include <CommBasicObjects/commNavigationVelocity.hh>

// ROS includes
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <tf/transform_datatypes.h>

class CommDataTranslator {
protected:
	// don't create an instance of this class, instead use the static methods directly
	CommDataTranslator() {  };

public:
	// conversion between the two Time objects
	static void convert(const ros::Time &ros_time, CommBasicObjects::CommTimeStamp &time);
	static void convert(const CommBasicObjects::CommTimeStamp &time, ros::Time &ros_time);

	// conversion between Twist/NavigationVelocity objects
	static void convert(const geometry_msgs::Twist &twist, CommBasicObjects::CommNavigationVelocity &velocity);
	static void convert(const CommBasicObjects::CommNavigationVelocity &velocity, geometry_msgs::Twist &twist);

	// conversion between Odometry/BasePosition+BaseVelocity objects
	static void convert(const nav_msgs::Odometry &odom, const unsigned int &update_count,
			CommBasicObjects::CommBasePosition &base_position, CommBasicObjects::CommBaseVelocity &velocity);
	static void convert(const CommBasicObjects::CommBasePosition &base_position, const CommBasicObjects::CommBaseVelocity &velocity,
			nav_msgs::Odometry &odom);

	// conversion between Odometry/BaseState objects (using corrected gps_pos from ros)
	static void convert(const nav_msgs::Odometry &odom_raw, const nav_msgs::Odometry &odom_gps, const unsigned int &update_count,
			CommBasicObjects::CommBaseState &base_state);
	static void convert(const CommBasicObjects::CommBaseState &base_state, nav_msgs::Odometry &odom_raw, nav_msgs::Odometry &odom_gps);

	// special helper for the case of position update
	static void convert(const CommBasicObjects::CommBasePositionUpdate &pos_update, CommBasicObjects::CommBasePosition &base_pos_diff);
	static void convert(const nav_msgs::Odometry &odom_raw, const unsigned int &update_count, const CommBasicObjects::CommBasePosition &base_pos_diff,
			CommBasicObjects::CommBaseState &base_state);

	// conversion between LaserScan objects
	static void convert(const sensor_msgs::LaserScan &scan, const tf::StampedTransform &tf_offset, const CommBasicObjects::CommBaseState &base_state,
			const unsigned int &update_count, CommBasicObjects::CommMobileLaserScan &mobile_scan, const double &max_range_cut=0.1);
	static void convert(const CommBasicObjects::CommMobileLaserScan &mobile_scan, sensor_msgs::LaserScan &scan,
			tf::StampedTransform &tf_offset, CommBasicObjects::CommBaseState &base_state, unsigned int &update_count);
};

#endif /* COMMDATATRANSLATOR_H_ */
