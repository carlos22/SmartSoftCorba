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


#ifndef DBSCAN_H_
#define DBSCAN_H_

#include "object_recognition/Cluster.hh"

#include <mrpt/core.h>
#include <vector>
#include <list>

using namespace mrpt::math;
using namespace mrpt::slam;

namespace mmp
{
	/**
	 * Class that performs the density based clustering.
	 */
	class DBScan
	{
		private:
			enum Label
			{
				UNCLASSIFIED = -2, NOISE = -1
			};

			struct DataObject
			{
					CPoint3D point;
					int label;

					DataObject(const CPoint3D& point)
					{
						this->point = point;
						this->label = DBScan::UNCLASSIFIED;
					}
			};

		private:

			const CSimplePointsMap& map;

			std::vector<int> label;

			/**
			 * Specifies the radius for a range-query
			 */
			double epsilon;


			/**
			 * Specifies the radius for fusing to clusters
			 */
			double fuseEpsilon;

			/**
			 * Specifies the density (the range-query must contain at least minPoints Points)
			 */
			unsigned int minPoints;

			/**
			 * Holds the number of clusters generated
			 */
			int numberOfGeneratedClusters;

			/**
			 * Holds the current clusterId
			 */
			int clusterId;

			/**
			 * Counter for the processed instances
			 */
			int processed_InstanceId;


		public:
			DBScan(const CSimplePointsMap& map);

			~DBScan();

			void getClusters(std::vector<Cluster>& clusters, std::vector<mrpt::math::TPoint3D>& noise);

			double getEpsilon() const
			{
				return epsilon;
			}

			void setEpsilon(double epsilon)
			{
				this->epsilon = epsilon;
			}

			unsigned int getMinPoints() const
			{
				return minPoints;
			}

			void setMinPoints(unsigned int minPoints)
			{
				this->minPoints = minPoints;
			}

			int getClusterCount() const
			{
				return numberOfGeneratedClusters;
			}

		private:
			void buildClusters();

			bool expandCluster(int index);

			void fuseClusters();

			void changeLabel(int oldLabel, int newLabel);

			void getNeighbours(std::vector<int>& list, const TPoint3D point, double epsilon);
	};
}
#endif /* DBSCAN_H_ */
