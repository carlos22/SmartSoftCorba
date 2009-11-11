//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
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


#ifndef MAPPER_H_
#define MAPPER_H_

#include "object_recognition/LaserListener.hh"
#include "object_recognition/ObjectRecognition.hh"

#include <mrpt/core.h>
#include <mrpt/slam/CPointsMap.h>
#include <mrpt/slam/CObservation2DRangeScan.h>

using namespace mrpt::math;
using namespace mrpt::slam;

namespace mmp
{
	/**
	 * This class generates a 3D map of the environment
	 */
	class Mapper: public LaserListener
	{
		private:
			/**
			 * The 3D environment map
			 */
			CSimplePointsMap map;

			/**
			 * The trashhold for the RANSAC algorithm which detects the table plane
			 */
			double tableTrashhold;

		public:
			/**
			 * Default Constructor
			 */
			Mapper();

			/**
			 * Destructor
			 */
			virtual ~Mapper();

			/**
			 * Handles new laser data. The table plane will be invalidated
			 * @param scan A new laser sweep
			 */
			virtual void handleLaserScan(const CObservation2DRangeScan& scan);

			/**
			 * Removes all points from the map which are farer from the origin than
			 * maxRangeXY and not between minZ and maxZ.
			 * Call this function before using the other functions
			 * @param maxRangeXY The maximum range in the xy plane
			 * @param minZ Minimal value for the z axis
			 * @param maxZ Maximal value for the z axis
			 */
			void clipOutOfRange(float maxRangeXY, float minZ, float maxZ);

			/**
			 * Returns a reference to the current map.
			 * @return The current map
			 */
			const CSimplePointsMap& getMap() const;

			/**
			 * First calculates the table plane and then returns a map containing
			 * all points which are on top of the table plane.
			 * IMPORTANT: First call clipOutOfRange to specify the right environment, otherwise
			 * a table will not be found
			 * @param obj The map of the objects on the table
			 */
			void getObjectMap(ObjectRecognition& obj);

			/**
			 * Removes all points form the map and invalidates the table plane
			 */
			void clearMap();

		private:

			/**
			 * Looks in the point cloud for the plane in which lies the most points.
			 * This is always the table plane, because of the specification of this approach.
			 * @return The calculated table plane
			 */
			TPlane calculateTablePlane();

	};

}

#endif /* MAPPER_H_ */
