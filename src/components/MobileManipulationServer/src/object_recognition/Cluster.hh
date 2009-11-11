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

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <mrpt/core.h>
#include <vector>

using namespace mrpt::math;

namespace mmp
{

	class Cluster
	{
		private:
			static const double binSize;
			static const int minPointCount;

			CPose3D pose;
			std::vector<mrpt::math::TPoint3D> points;

			mrpt::math::TPoint3D minPoint;
			mrpt::math::TPoint3D maxPoint;

			std::vector<double> features;

		public:
			/**
			 * Default Constructor
			 */
			Cluster();

			/**
			 * Destructor
			 */
			virtual ~Cluster();

			/**
			 * Returns the minPoint and maxPoint of the bounding box
			 * @param minPoint
			 * @param maxPoint
			 */
			void getBoundingBox(mrpt::math::TPoint3D& minPoint, mrpt::math::TPoint3D& maxPoint) const
			{
				minPoint = this->minPoint;
				maxPoint = this->maxPoint;
			}

			/**
			 * Returns the inner list of the Points.
			 * @return The list of points in the cluster.
			 */
			const std::vector<mrpt::math::TPoint3D>& getPoints() const
			{
				return this->points;
			}

			/**
			 * Returns the pose of the cluster
			 * @return Pose of the cluster
			 */
			const CPose3D& getPose() const
			{
				return this->pose;
			}

			const std::vector<double>& getFeatures() const
			{
				return this->features;
			}

			/**
			 * Prints the features to std::out
			 */
			void printFeatures() const;

			/**
			 * Returns the size/number of points in the cluster
			 * @return Number of points
			 */
			unsigned int size() const
			{
				return this->points.size();
			}

			/**
			 * Add a Point to a Cluster.
			 * Only allowed from the DBScan class
			 * @param point Point that should be added.
			 */
			void addPoint(const mrpt::math::TPoint3D& point);

			/**
			 * Calculates the features for the cloud to identify the object.
			 */
			void calculateFeatures(TPlane tablePlane);

			/**
			 * Calculates the eigenvectors for the point cloud
			 */
			void calculateEigenvectors(mrpt::math::TPoint3D& xAxis, mrpt::math::TPoint3D& yAxis,
					mrpt::math::TPoint3D& zAxis);

			/**
			 * Calculates the bounding box
			 */
			void calculateBoundingBox();

			/**
			 * Calculates the centroid of the cluster
			 */
			void calculateCentroid();

			/**
			 * Calculates the expectation and standard derivation of the 4 quadrants
			 */
			void calculateExpectation();

			/**
			 * calculates a histogram over the z axis
			 */
			void calculateZHistogram();

			/**
			 * Changes the origin of the cloud
			 * @param pose
			 */
			void changeOrigin(const CPose3D& pose);

			void calculateRelativeVolume();

	};

}

#endif /* CLUSTER_H_ */
