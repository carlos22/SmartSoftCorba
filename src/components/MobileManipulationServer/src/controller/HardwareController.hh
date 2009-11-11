//--------------------------------------------------------------------------
//  Copyright (C) 2009 Jonas Brich, Manuel Wopfner
//
//        brich@mail.hs-ulm.de
//        wopfner@hs-ulm.de
//
//	Contact:
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "Katana Mobile Manipulation component".
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
//--------------------------------------------------------------------------

#ifndef HARDWARECONTROLLER_H_
#define HARDWARECONTROLLER_H_

#include "object_recognition/LaserListener.hh"
#include "hardware/Katana.hh"
#include "object_recognition/Point6D.hh"

#include <mrpt/hwdrivers/CHokuyoURG.h>
#include <mrpt/slam/CObservation2DRangeScan.h>

#include <vector>

using namespace KatanaAPI;
using namespace mrpt::hwdrivers;
using mrpt::slam::CObservation2DRangeScan;

namespace mmp
{
	/**
	 * This class is the interface to all hardware devices
	 */
	class HardwareController
	{
		private:
			/**
			 * This vector holds all listeners which are informed when a new laser scan object is available.
			 * The listeners are not deleted form this class
			 */
			std::vector<LaserListener*> laserListeners;

			CHokuyoURG* laser;
			Katana* katana;

		public:
			/**
			 * Default Constructor
			 */
			HardwareController();

			/**
			 * Denstructor
			 */
			virtual ~HardwareController();

			/**
			 * Adds a new Listener to the list.
			 * It doesn't check if the listener is already in the list.
			 * @param listener A new listener which should be added
			 */
			void addLaserListener(LaserListener* listener);

			/**
			 * Scans a give region.
			 * @param scanAngle The angle to scan in deg
			 * @param appendScan Append to the previous scan
			 */
			void scanRegion(double scanAngle, bool appendScan=false);

			/**
			 * Moves to a point an grasp an object
			 * @param point Point which should be grasped
			 */
			void moveToPoint(Point6D& point);

			/**
			 * Turns off the katana arm so you can move it.
			 * After pressing a key the tcp point is return and the arm
			 * moves to standby position
			 * @return
			 */
			Point6D getPoint();

			/**
			 * Returns the katana class
			 * TODO: evtl noch entvernen
			 * @return katana class
			 */
			Katana& getKatana()
			{
				return *katana;
			}

		private:

			/**
			 * Read a laser sweep from the hokuyo
			 * @param obs Contains the read laser sweep
			 * @param tcp The position of the tool center point
			 */
			void makeLaserSweep(CObservation2DRangeScan& obs, const Point6D& tcp);

			/**
			 * Sends a laser scan to all laser listeners
			 * @param scan Scan which should be send to all listeners
			 */
			void sendLaserObject(const CObservation2DRangeScan& scan) const;

			/**
			 * Initializes the different hardware devices.
			 * Hokuyo, Katana
			 */
			void initDevices();

			/**
			 * Tears the devices down.
			 */
			void teardownDevices();

	};
}

#endif /* HARDWARECONTROLLER_H_ */
