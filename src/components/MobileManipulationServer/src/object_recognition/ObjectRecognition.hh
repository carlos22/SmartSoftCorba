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


#ifndef OBJECTRECOGNITION_H_
#define OBJECTRECOGNITION_H_

#include "object_recognition/Point6D.hh"
#include "object_recognition/Cluster.hh"

#include <mrpt/core.h>

#include <vector>
#include <string>

using namespace mrpt::math;
using namespace mrpt::slam;

using namespace KatanaAPI;

namespace mmp
{
	/**
	 * This class does the object recognition and returns a list of clusters
	 * which can be compared with the object database.
	 */
	class ObjectRecognition
	{
		private:
			/**
			 * Map of all objects
			 */
			CSimplePointsMap map;

			std::vector<Cluster> clusters;
			std::vector<mrpt::math::TPoint3D> noise;

			TPlane tablePlane;
			Cluster table;

		public:
			/**
			 * Constructor which takes a map of objects
			 * @param map Map which contains a point cloud
			 */
			ObjectRecognition();

			/**
			 * Sets the table plane
			 * @param plane
			 */
			void setTablePlane(TPlane plane)
			{
				this->tablePlane = plane;
			}

			/**
			 * Adds a point to the intern map
			 * @param point
			 */
			void addPoint(const mrpt::math::TPoint3D& point);

			/**
			 * Adds a point to the intern table cluster
			 * @param point
			 */
			void addTablePoint(const mrpt::math::TPoint3D& point);

			/**
			 * Performs the object recognition in the object map
			 */
			void doCalculations();

			/**
			 * Returns a vector of the found clusters
			 * @return
			 */
			const std::vector<Cluster>& getClusters() const
			{
				return this->clusters;
			}

			const std::vector<mrpt::math::TPoint3D>& getNoise() const
			{
				return this->noise;
			}

			const Cluster& getTableCluster() const
			{
				return this->table;
			}

			/**
			 * TODO remove later
			 * @param mug1
			 * @param mug2
			 */
			void getMugPositions(Point6D& mug1, Point6D& mug2);

		private:
			/**
			 * Performs the DBSCAN
			 */
			void dbscan();
	};
}
#endif /* OBJECTRECOGNITION_H_ */
