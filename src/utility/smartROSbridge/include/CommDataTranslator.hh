/*
 * CommDataTranslator.h
 *
 *  Created on: May 11, 2011
 *      Author: alexej
 */

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
