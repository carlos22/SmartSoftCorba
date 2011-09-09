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

#ifndef ROS_LASERSCAN_HH_
#define ROS_LASERSCAN_HH_

#include <CommBasicObjects/commMobileLaserScan.hh>

#include "gen/SmartROSLaserGateway.hh"

// include utility: smartROSbridge
#include <CommDataTranslator.hh>

class ROS_LaserScan
{
private:
	static CHS::SmartMutex mutex;
	static CommBasicObjects::CommMobileLaserScan curr_scan;

//	static nav_msgs::Odometry raw_odometry;
//	static nav_msgs::Odometry gps_odometry;

	static unsigned long scan_count;
public:
	static CommBasicObjects::CommMobileLaserScan getCurrentScan()
	{
		CHS::SmartGuard g(mutex);
		return curr_scan;
	}


	static void lrfCallback(const sensor_msgs::LaserScan &ros_scan)
	{
		CommBasicObjects::CommMobileLaserScan mobile_scan;

		CommBasicObjects::CommBaseState base_state;
		if( COMP->baseStateClient->getUpdate(base_state) == CHS::SMART_OK )
		{
		    // get current base-laser offset from TF
			if(!COMP->ini.ROS.is_static_mounted_lrf)
			{
				// TODO: current offset from TF, according to the current laser scan
				//tf::StampedTransform transform;
				/*
				try {
					COMP->tf_listener->lookupTransform("/base_link", "/base_scan_link", ros::Time(0), transform);
				} catch (tf::TransformException &ex) {
					ROS_ERROR("%s", ex.what());
				}
				ROS_INFO("trans.x=%f; trans.y=%f;\n", transform.getOrigin().x(), transform.getOrigin().y());
				*/
				//TODO: remove valid=false and warning after full implementation...
				ROS_WARN("ROS_LaserScan::lrfCallback get TF for each scan is not implemented yet!\n");
				mobile_scan.set_scan_valid(false);
			} else {
				// fill mobile-scan with data
			    CommDataTranslator::convert(ros_scan, COMP->base_laser_offset, base_state, scan_count, mobile_scan);
			    /*
			    if(COMP->ini.services.verbose)
			    {
			    	//std::cout << mobile_scan << std::endl;
			    	std::cout << "scan 90: " << mobile_scan.get_scan_distance(90) << std::endl;
			    	std::cout << "scan 180: " << mobile_scan.get_scan_distance(180) << std::endl;
			    	std::cout << "scan 270: " << mobile_scan.get_scan_distance(270) << std::endl;
			    }
			    */
			}
		} else {
			mobile_scan.set_scan_valid(false);
		}

	    // store the current laser scan in the local member variable (used by query pattern to request for laser scans)
	    mutex.acquire();
	    curr_scan = mobile_scan;
	    mutex.release();

	    // publish current mobile scan
	    COMP->laserServer->put(mobile_scan);
	    ++scan_count;
	}

/*
	static void odometryRawCallback(const nav_msgs::Odometry &odom)
	{
		// no mutex necessary here, because lrfCallback and odometryCallback are ALWAYS called in sequence (and never concurrently)
		raw_odometry = odom;
	}

	static void odometryGpsCallback(const nav_msgs::Odometry &odom)
	{
		// no mutex necessary here, because lrfCallback and odometryCallback are ALWAYS called in sequence (and never concurrently)
		gps_odometry = odom;
	}
*/
};

#endif /* ROS_LASERSCAN_HH_ */
