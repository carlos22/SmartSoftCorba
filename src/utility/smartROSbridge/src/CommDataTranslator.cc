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
#include "CommDataTranslator.hh"

#include <math.h>

#include <tf/tf.h>

/////////////////////////////////////////////////////////////
// conversion between the two Time objects
/////////////////////////////////////////////////////////////
void CommDataTranslator::convert(const ros::Time &ros_time, CommBasicObjects::CommTimeStamp &time)
{
	time.set(ros_time.sec, ros_time.nsec);
}
void CommDataTranslator::convert(const CommBasicObjects::CommTimeStamp &time, ros::Time &ros_time)
{
	ros_time.sec = time.get_seconds();
	ros_time.nsec = time.get_microseconds();
}

/////////////////////////////////////////////////////////////
// conversion between Odometry/BaseState objects
/////////////////////////////////////////////////////////////
void CommDataTranslator::convert(const geometry_msgs::Twist &twist, CommBasicObjects::CommNavigationVelocity &velocity)
{
	velocity.set_v(twist.linear.x);
	//twist.linear.y;
	//twist.linear.z;
	//twist.angular.x;
	//twist.angular.y;
	velocity.set_omega(twist.angular.z);
}

void CommDataTranslator::convert(const CommBasicObjects::CommNavigationVelocity &velocity, geometry_msgs::Twist &twist)
{
	// read the received CommNavigationVelocity object
	const double vx = velocity.get_v()/1000; // velocity to the front of the robot
	const double vy = 0.0;                   // velocity to the left/right side of the robot
	const double vz = 0.0;                   // velocity to the top/bottom direction
	const double ax = 0.0;
	const double ay = 0.0;
	const double az = velocity.get_omega();

	twist.linear.x = vx;
	twist.linear.y = vy;
	twist.linear.z = vz;
	twist.angular.x = ax;
	twist.angular.y = ay;
	twist.angular.z = az;
}

/////////////////////////////////////////////////////////////////
// conversion between Odometry/BasePosition+BaseVelocity objects
/////////////////////////////////////////////////////////////////
void CommDataTranslator::convert(const nav_msgs::Odometry &odom, const unsigned int &update_count, CommBasicObjects::CommBasePosition &base_position, CommBasicObjects::CommBaseVelocity &base_velocity)
{
	// set 3D base position
	base_position.set_x(odom.pose.pose.position.x, 1.0);
	base_position.set_y(odom.pose.pose.position.y, 1.0);
	base_position.set_z(odom.pose.pose.position.z, 1.0);

	base_position.set_update_count(update_count);

	// set base orientation calculated from Quaternion (using tf converters)
	tf::Quaternion bt(odom.pose.pose.orientation.x,
		odom.pose.pose.orientation.y,
		odom.pose.pose.orientation.z,
		odom.pose.pose.orientation.w);
	bt.normalize();
	base_position.set_base_alpha(tf::getYaw(bt));

	// covariance matrix in a 3D pose of base_position is:
	// xx xy xz
	// yx yy yz
	// zx zy zz

	// in ROS covariance matrix of a 6D pose must be read as:
	//------------------------
	//j=     0  1  2  3  4  5
	//------------------------
	//i= 0 | xx xy .. .. .. xz
	//   1 | yx yy .. .. .. yz
	//   2 | .. .. .. .. .. ..
	//   3 | .. .. .. .. .. ..
	//   4 | .. .. .. .. .. ..
	//   5 | zx zy .. .. .. zz
	base_position.set_cov(0,0, odom.pose.covariance[0*6+0]);
	base_position.set_cov(0,1, odom.pose.covariance[0*6+1]);
	base_position.set_cov(0,2, odom.pose.covariance[0*6+5]);
	base_position.set_cov(1,0, odom.pose.covariance[1*6+0]);
	base_position.set_cov(1,1, odom.pose.covariance[1*6+1]);
	base_position.set_cov(1,2, odom.pose.covariance[1*6+5]);
	base_position.set_cov(2,0, odom.pose.covariance[5*6+0]);
	base_position.set_cov(2,1, odom.pose.covariance[5*6+1]);
	base_position.set_cov(2,2, odom.pose.covariance[5*6+5]);

	// set data for base velocity
	double vx = odom.twist.twist.linear.x;
	double vy = odom.twist.twist.linear.y;
	double vz = odom.twist.twist.linear.z;
	// calculate vector length
	double v = sqrt(vx*vx + vy*vy + vz*vz);
	base_velocity.set_v(v, 1.0);
	// set current omega velocity of the base
	base_velocity.set_omega_base(odom.twist.twist.angular.z);
}


void CommDataTranslator::convert(const CommBasicObjects::CommBasePosition &base_position, const CommBasicObjects::CommBaseVelocity &base_velocity, nav_msgs::Odometry &odom)
{
	// set 3D base position
	odom.pose.pose.position.x = base_position.get_x(1.0);
	odom.pose.pose.position.y = base_position.get_y(1.0);
	odom.pose.pose.position.z = base_position.get_z(1.0);

	// set base orientation calculated from Quaternion (using tf converters)
	geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(base_position.get_base_alpha());
	odom.pose.pose.orientation = odom_quat;

	// covariance matrix in a 3D pose of base_position is:
	// xx xy xz
	// yx yy yz
	// zx zy zz

	// set all values to 0
	for(unsigned i=0; i<6; ++i)
	{
		for(unsigned j=0; j<6; ++j)
		{
			odom.pose.covariance[i*6+j] = 0.0;
		}
	}

	// in ROS covariance matrix of a 6D pose must be read as:
	//------------------------
	//j=     0  1  2  3  4  5
	//------------------------
	//i= 0 | xx xy .. .. .. xz
	//   1 | yx yy .. .. .. yz
	//   2 | .. .. .. .. .. ..
	//   3 | .. .. .. .. .. ..
	//   4 | .. .. .. .. .. ..
	//   5 | zx zy .. .. .. zz
	odom.pose.covariance[0*6+0] = base_position.get_cov(0,0);
	odom.pose.covariance[0*6+1] = base_position.get_cov(0,1);
	odom.pose.covariance[0*6+5] = base_position.get_cov(0,2);
	odom.pose.covariance[1*6+0] = base_position.get_cov(1,0);
	odom.pose.covariance[1*6+1] = base_position.get_cov(1,1);
	odom.pose.covariance[1*6+5] = base_position.get_cov(1,2);
	odom.pose.covariance[5*6+0] = base_position.get_cov(2,0);
	odom.pose.covariance[5*6+1] = base_position.get_cov(2,1);
	odom.pose.covariance[5*6+5] = base_position.get_cov(2,2);

	// set data for base velocity
	odom.twist.twist.linear.x = base_velocity.get_v(1.0);
	odom.twist.twist.linear.y = 0.0;
	odom.twist.twist.linear.z = 0.0;
	odom.twist.twist.angular.x = 0.0;
	odom.twist.twist.angular.y = 0.0;
	odom.twist.twist.angular.z = base_velocity.get_omega_base();
}

///////////////////////////////////////////////////////////////////////////////////
// conversion between Odometry/BaseState objects (using corrected gps_pos from ros)
///////////////////////////////////////////////////////////////////////////////////
void CommDataTranslator::convert(const nav_msgs::Odometry &odom_raw, const nav_msgs::Odometry &odom_gps, const unsigned int &update_count, CommBasicObjects::CommBaseState &base_state)
{
	CommBasicObjects::CommBasePosition base_raw_position;
	CommBasicObjects::CommBaseVelocity base_raw_velocity;
	convert(odom_raw, update_count, base_raw_position, base_raw_velocity);

	CommBasicObjects::CommTimeStamp raw_time;
	convert(odom_raw.header.stamp, raw_time);

	// write objects into base_state
	base_state.set_time_stamp(raw_time);
	base_state.set_base_raw_position(base_raw_position);
	base_state.set_base_velocity(base_raw_velocity);

	CommBasicObjects::CommBasePosition base_gps_position;
	// ignore gps velocity, because not needed in SmartSoft
	convert(odom_gps, update_count, base_gps_position, base_raw_velocity);
	base_state.set_base_position(base_gps_position);
}

void CommDataTranslator::convert(const CommBasicObjects::CommBaseState &base_state, nav_msgs::Odometry &odom_raw, nav_msgs::Odometry &odom_gps)
{
	CommBasicObjects::CommBaseVelocity base_velocity = base_state.get_base_velocity();
	convert(base_state.get_base_raw_position(), base_velocity, odom_raw);
	convert(base_state.get_time_stamp(), odom_raw.header.stamp);

	convert(base_state.get_base_position(), base_velocity, odom_gps);
	convert(base_state.get_time_stamp(), odom_gps.header.stamp);
}

////////////////////////////////////////////////////////////////
// special helper for the case of position update in SmartSoft
////////////////////////////////////////////////////////////////
void CommDataTranslator::convert(const CommBasicObjects::CommBasePositionUpdate &pos_update, CommBasicObjects::CommBasePosition &base_pos_diff)
{
	CommBasicObjects::CommBasePosition corr_pos = pos_update.get_corrected_position();
	CommBasicObjects::CommBasePosition old_pos = pos_update.get_old_position();

	// set general parameters
	base_pos_diff.set_update_count(corr_pos.get_update_count());
	base_pos_diff.set_cov_invalid(true);

	// set difference
	base_pos_diff.set_x(corr_pos.get_x() - old_pos.get_x());
	base_pos_diff.set_y(corr_pos.get_y() - old_pos.get_y());
	base_pos_diff.set_z(corr_pos.get_z() - old_pos.get_z());
	base_pos_diff.set_base_alpha(corr_pos.get_base_alpha() - old_pos.get_base_alpha());
}
void CommDataTranslator::convert(const nav_msgs::Odometry &odom_raw, const unsigned int &update_count, const CommBasicObjects::CommBasePosition &base_pos_diff, CommBasicObjects::CommBaseState &base_state)
{
	CommBasicObjects::CommBasePosition raw_position;
	CommBasicObjects::CommBaseVelocity velocity;
	convert(odom_raw, update_count, raw_position, velocity);

	CommBasicObjects::CommTimeStamp time;
	convert(odom_raw.header.stamp, time);

	// calculate corrected position
	CommBasicObjects::CommBasePosition corr_pos;
	corr_pos.set_update_count(update_count);
	corr_pos.set_x(raw_position.get_x() + base_pos_diff.get_x());
	corr_pos.set_y(raw_position.get_y() + base_pos_diff.get_y());
	corr_pos.set_z(raw_position.get_z() + base_pos_diff.get_z());
	corr_pos.set_base_alpha(raw_position.get_base_alpha() + base_pos_diff.get_base_alpha());

	// set time-stamp, both positions and velocity
	base_state.set_time_stamp(time);
	base_state.set_base_raw_position(raw_position);
	base_state.set_base_position(corr_pos);
	base_state.set_base_velocity(velocity);
}

/////////////////////////////////////////////////////////////
// conversion between LaserScan objects
/////////////////////////////////////////////////////////////
void CommDataTranslator::convert(
		const sensor_msgs::LaserScan &scan,
		const tf::StampedTransform &tf_offset,
		const CommBasicObjects::CommBaseState &base_state,
		const unsigned int &update_count,
		CommBasicObjects::CommMobileLaserScan &mobile_scan,
		const double &max_range_cut)
{
    //ROS_WARN("CommDataTranslator::convert(in sensor_msgs::LaserScan, in tf::StampedTransform, in nav_msgs::Odometry, out CommBasicObjects::CommMobileLaserScan) is not implemented yet!");

    // 1) set current update_count and base_state in laser-scan
	mobile_scan.set_scan_update_count(update_count);
    mobile_scan.set_base_state(base_state);


    // 2) set timestamp where the scan was made
    CommBasicObjects::CommTimeStamp scan_time;
    convert(scan.header.stamp, scan_time);
    mobile_scan.set_scan_time_stamp(scan_time);

    // 3) set the offset of the lrf-device to the base

    // 3.1) set 3D relative pose offset between base and laser
    double offset_x = tf_offset.getOrigin().getX();
    double offset_y = tf_offset.getOrigin().getY();
    double offset_z = tf_offset.getOrigin().getZ();
    double yaw, pitch, roll;
    tf_offset.getBasis().getEulerYPR(yaw, pitch, roll);

    CommBasicObjects::CommPose3d pose;
    pose.set_x(offset_x, 1.0);
    pose.set_y(offset_y, 1.0);
    pose.set_z(offset_z, 1.0);
    pose.set_azimuth(yaw);
    pose.set_elevation(pitch);
    pose.set_roll(roll);

    mobile_scan.set_sensor_pose(pose);

    // 3.2) calculate new world pose of the current laser-scan by rotating the coordinate system counterclockwise around the z axis
    double curr_x, curr_y, curr_z, curr_a;
    curr_x = base_state.get_base_position().get_x(1.0);
    curr_y = base_state.get_base_position().get_y(1.0);
    curr_z = base_state.get_base_position().get_z(1.0);
    curr_a = base_state.get_base_position().get_base_alpha();

    // see http://en.wikipedia.org/wiki/Cartesian_coordinate_system for more details for coordinate system transformations
    double new_x, new_y, new_z;
    new_x = offset_x * cos(curr_a) - offset_y * sin(curr_a);
    new_y = offset_x * sin(curr_a) + offset_y * cos(curr_a);
    new_z = offset_z; // z is the same, because we rotate around z axis

    mobile_scan.set_scanner_x(curr_x + new_x, 1.0);
    mobile_scan.set_scanner_y(curr_y + new_y, 1.0);
    mobile_scan.set_scanner_z(curr_y + new_z, 1.0);
    mobile_scan.set_scanner_azimuth(curr_a + yaw);
    mobile_scan.set_scanner_elevation(pitch); // curr_elevation is ignored because is always NULL
    mobile_scan.set_scanner_roll(roll); // curr_roll is ignored because is always NULL

    // 4) set scan parameters
	mobile_scan.set_scan_length_unit(1);

	// 4.1) calculate the starting angle and resolution in [1] deg
	double starting_angle = (scan.angle_min / M_PI * 180);
	double resolution = (scan.angle_increment / M_PI * 180);
	mobile_scan.set_scan_double_field_of_view(starting_angle, resolution);

	// 5) copy all valid scan rays
    size_t num_points = scan.ranges.size();

    double max_range = scan.range_max - (scan.range_max*max_range_cut);
    double min_range = scan.range_min;

    if( num_points == 0)
    {
      mobile_scan.set_scan_valid(false);
    }
    else
    {
    	mobile_scan.set_max_distance(max_range, 1.0);
    	mobile_scan.set_min_distance(min_range, 1.0);

    	size_t num_valid_points = 0;
    	for(size_t i=0; i<num_points; ++i)
    	{
    		if(min_range < scan.ranges[i] && scan.ranges[i] < max_range) ++num_valid_points;
    	}

    	mobile_scan.set_scan_size(num_valid_points);

    	size_t valid_point_index = 0;
    	for(size_t i=0; i<num_points; ++i)
    	{
    		const double dist = scan.ranges[i];

    		// add scan ray only if it is within a range
			if(min_range < dist && dist < max_range)
			{
				mobile_scan.set_scan_index(valid_point_index, i);
				mobile_scan.set_scan_distance(valid_point_index, dist, 1.0);

				// set intensity to a value between 0 and 7
				if(i < scan.intensities.size())
				{
					if(scan.intensities[i] < 0) {
						mobile_scan.set_scan_intensity(valid_point_index, 0);
					} else if(scan.intensities[i] > 6) {
						mobile_scan.set_scan_intensity(valid_point_index, 6);
					} else {
						mobile_scan.set_scan_intensity(valid_point_index, scan.intensities[i]);
					}
				}

				valid_point_index++;
			}
    	}

    	mobile_scan.set_scan_valid(true);
    }
}

void CommDataTranslator::convert(const CommBasicObjects::CommMobileLaserScan &mobile_scan,
		sensor_msgs::LaserScan &scan,
		tf::StampedTransform &tf_offset,
		CommBasicObjects::CommBaseState &base_state,
		unsigned int &update_count)
{
    ROS_WARN("CommDataTranslator::convert(in CommBasicObjects::CommMobileLaserScan, out sensor_msgs::LaserScan, out tf::StampedTransform, out CommBasicObjects::CommBaseState) is not implemented yet!");
}

