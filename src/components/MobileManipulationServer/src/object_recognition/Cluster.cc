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

#include "object_recognition/Cluster.hh"

#include "object_recognition/ObjectClass.hh"
#include <math.h>

using namespace std;

namespace mmp
{
	const double Cluster::binSize = 0.01;
	const int Cluster::minPointCount = 2;

	/*************************************************

	 Public Methods

	 *************************************************/
	Cluster::Cluster() :
		minPoint(800, 800, 800), maxPoint(-800, -800, -800)
	{
		this->features.resize(ObjectClass::featureSpaceSize);
	}

	Cluster::~Cluster()
	{
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	void Cluster::addPoint(const mrpt::math::TPoint3D& point)
	{
		this->points.push_back(point);

		// minPoint
		if (point.x < this->minPoint.x)
			this->minPoint.x = point.x;

		if (point.y < this->minPoint.y)
			this->minPoint.y = point.y;

		if (point.z < this->minPoint.z)
			this->minPoint.z = point.z;

		// maxPoint
		if (point.x > this->maxPoint.x)
			this->maxPoint.x = point.x;

		if (point.y > this->maxPoint.y)
			this->maxPoint.y = point.y;

		if (point.z > this->maxPoint.z)
			this->maxPoint.z = point.z;

	}

	void Cluster::calculateFeatures(TPlane tablePlane)
	{
		// calculate center of point cloud
		CPose3D center(minPoint.x + (maxPoint.x - minPoint.x) / 2, minPoint.y + (maxPoint.y - minPoint.y) / 2,
				minPoint.z + (maxPoint.z - minPoint.z) / 2);

		// calcuate pose
		//tablePlane.getAsPose3D(this->pose);
		changeOrigin(center);

		// calculate eigenvectors and set largest eigenvector as x axis
		TPoint3D xAxis;
		TPoint3D yAxis;
		TPoint3D zAxis;
		calculateEigenvectors(xAxis, yAxis, zAxis);

		double yaw = atan(xAxis.y / xAxis.x);
		CPose3D dir(0, 0, 0, yaw, 0, 0);
		changeOrigin(dir);

		// calculate bounding box
		calculateBoundingBox();

		// calculate centroid
		//calculateCentroid();

		// calculate expectation
		//calculateExpectation();

		// calculate z-histogram
		//calculateZHistogram();

		// calculate relative volume
		//calculateRelativeVolume();
	}

	void Cluster::calculateEigenvectors(TPoint3D& xAxis, TPoint3D& yAxis, TPoint3D& zAxis)
	{
		vector<double> means;
		CMatrixDouble33 covars, eigenVal, eigenVec;

		covariancesAndMean(this->points, covars, means);
		covars.eigenVectors(eigenVec, eigenVal);

		zAxis.x = eigenVec(0, 0);
		zAxis.y = eigenVec(1, 0);
		zAxis.z = eigenVec(2, 0);

		yAxis.x = eigenVec(0, 1);
		yAxis.y = eigenVec(1, 1);
		yAxis.z = eigenVec(2, 1);

		xAxis.x = eigenVec(0, 2);
		xAxis.y = eigenVec(1, 2);
		xAxis.z = eigenVec(2, 2);

		// set eigenvalues to histogram and multiply them by 100
		// TODO remove multiply by 100?
		//		this->histogram.setBiggestEigenvalue(eigenVal(2, 2) * 100);
		//		this->histogram.setMiddleEigenvalue(eigenVal(1, 1) * 100);
		//		this->histogram.setSmallestEigenvalue(eigenVal(0, 0) * 100);
	}

	void Cluster::calculateBoundingBox()
	{
		this->minPoint.x = this->minPoint.y = this->minPoint.z = 800;
		this->maxPoint.x = this->maxPoint.y = this->maxPoint.z = -800;

		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			TPoint3D& point = points[i];

			// minPoint
			if (point.x < this->minPoint.x)
				this->minPoint.x = point.x;

			if (point.y < this->minPoint.y)
				this->minPoint.y = point.y;

			if (point.z < this->minPoint.z)
				this->minPoint.z = point.z;

			// maxPoint
			if (point.x > this->maxPoint.x)
				this->maxPoint.x = point.x;

			if (point.y > this->maxPoint.y)
				this->maxPoint.y = point.y;

			if (point.z > this->maxPoint.z)
				this->maxPoint.z = point.z;
		}

		// set histogram values
		this->features[0] = (maxPoint.x - minPoint.x);
		this->features[1] = (maxPoint.y - minPoint.y);
		this->features[2] = (maxPoint.z - minPoint.z);

	}

	void Cluster::calculateCentroid()
	{
		TPoint3D centroid;

		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			centroid.x += this->points[i].x;
			centroid.y += this->points[i].y;
			centroid.z += this->points[i].z;
		}

		// calculate mean
		centroid.x /= this->points.size();
		centroid.y /= this->points.size();
		centroid.z /= this->points.size();

		// set centroid to histogram
		//		this->histogram.setCentroidX(centroid.x);
		//		this->histogram.setCentroidY(centroid.y);
		//		this->histogram.setCentroidZ(centroid.z);
	}

	void Cluster::calculateExpectation()
	{
		double q1Expectation = 0;
		double q1Variance = 0;
		size_t q1PointCount = 0;

		double q2Expectation = 0;
		double q2Variance = 0;
		size_t q2PointCount = 0;

		double q3Expectation = 0;
		double q3Variance = 0;
		size_t q3PointCount = 0;

		double q4Expectation = 0;
		double q4Variance = 0;
		size_t q4PointCount = 0;

		// Calculate expectation value
		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			// 1. Quadrant
			if (this->points[i].x > 0 && this->points[i].y > 0)
			{
				q1Expectation += sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2));
				q1PointCount++;
			}
			// 2. Quadrant
			else if (this->points[i].x < 0 && this->points[i].y > 0)
			{
				q2Expectation += sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2));
				q2PointCount++;
			}
			// 3. Quadrant
			else if (this->points[i].x < 0 && this->points[i].y < 0)
			{
				q3Expectation += sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2));
				q3PointCount++;
			}
			// 4. Quadrant
			else if (this->points[i].x > 0 && this->points[i].y < 0)
			{
				q4Expectation += sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2));
				q4PointCount++;
			}
		}

		//		this->histogram[3] = q1Expectation /= q1PointCount;
		//		this->histogram[4] = q2Expectation /= q2PointCount;
		//		this->histogram[5] = q3Expectation /= q3PointCount;
		//		this->histogram[6] = q4Expectation /= q4PointCount;

		// Calculate variance
		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			// 1. Quadrant
			if (this->points[i].x > 0 && this->points[i].y > 0)
			{
				q1Variance += pow(q1Expectation - sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2)), 2);
			}
			// 2. Quadrant
			else if (this->points[i].x < 0 && this->points[i].y > 0)
			{
				q2Variance += pow(q2Expectation - sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2)), 2);
			}
			// 3. Quadrant
			else if (this->points[i].x < 0 && this->points[i].y < 0)
			{
				q3Variance += pow(q3Expectation - sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2)), 2);
			}
			// 4. Quadrant
			else if (this->points[i].x > 0 && this->points[i].y < 0)
			{
				q4Variance += pow(q4Expectation - sqrt(pow(this->points[i].x, 2) + pow(this->points[i].y, 2)), 2);
			}
		}

		//		this->histogram[7] = q1Variance /= q1PointCount;
		//		this->histogram[8] = q2Variance /= q2PointCount;
		//		this->histogram[9] = q3Variance /= q3PointCount;
		//		this->histogram[10] = q4Variance /= q4PointCount;
	}

	void Cluster::calculateZHistogram()
	{
		double binSize = (this->maxPoint.x - this->minPoint.x) / 4;
		double halfHeight = (this->maxPoint.z - this->minPoint.z) / 2;

		cout << "Bin Size: " << binSize << endl;
		cout << "Half Height: " << halfHeight << endl;

		double bin1 = 0;
		double bin2 = 0;
		double bin3 = 0;
		double bin4 = 0;

		size_t bin1Count = 0;
		size_t bin2Count = 0;
		size_t bin3Count = 0;
		size_t bin4Count = 0;

		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			// only points near the x axis
			if (abs(this->points[i].y) < 0.01)
			{
				if (this->points[i].x > -2 * binSize && this->points[i].x < -1 * binSize)
				{
					bin1 += this->points[i].z + halfHeight;
					bin1Count++;
				}
				else if (this->points[i].x > -1 * binSize && this->points[i].x < 0)
				{
					bin2 += this->points[i].z + halfHeight;
					bin2Count++;
				}
				else if (this->points[i].x > 0 && this->points[i].x < binSize)
				{
					bin3 += this->points[i].z + halfHeight;
					bin3Count++;
				}
				else if (this->points[i].x > binSize && this->points[i].x < 2 * binSize)
				{
					bin4 += this->points[i].z + halfHeight;
					bin4Count++;
				}
			}

		}

		size_t minBinSize = 10;
		this->features[3] = (bin1Count < minBinSize) ? 0 : bin1 / bin1Count;
		this->features[4] = (bin2Count < minBinSize) ? 0 : bin2 / bin2Count;
		this->features[5] = (bin3Count < minBinSize) ? 0 : bin3 / bin3Count;
		this->features[6] = (bin4Count < minBinSize) ? 0 : bin4 / bin4Count;
	}

	void Cluster::changeOrigin(const CPose3D& p)
	{
		this->pose = this->pose + p;

		// move bounding box
		CPoint3D p1(this->minPoint);
		this->minPoint = p1 - p;
		CPoint3D p2(this->maxPoint);
		this->maxPoint = p2 - p;

		// move origin of the point cloud to it's center
		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			CPoint3D point(this->points[i]);
			this->points[i] = point - p;
		}
	}

	void Cluster::printFeatures() const
	{
		std::cout << "Features: ";
		for (size_t i = 0; i < this->features.size(); i++)
		{
			std::cout << this->features[i] << ", ";
		}
		std::cout << std::endl;
	}

	void Cluster::calculateRelativeVolume()
	{
		int binCountX = floor((this->maxPoint.x - this->minPoint.x) / binSize);
		int binCountY = floor((this->maxPoint.y - this->minPoint.y) / binSize);

		double binSizeX = (this->maxPoint.x - this->minPoint.x) / binCountX;
		double binSizeY = (this->maxPoint.y - this->minPoint.y) / binCountY;

		double halfHeight = (this->maxPoint.z - this->minPoint.z) / 2;

		int pointCount[binCountX][binCountY];
		double maxZValue[binCountX][binCountY];
		for (int i = 0; i < binCountX; i++)
		{
			for (int j = 0; j < binCountY; j++)
			{
				pointCount[i][j] = 0;
				maxZValue[i][j] = 0;
			}
		}

		//		cout << "BinCountX: " << binCountX << "; BinCountY: " << binCountY << endl;
		//		cout << "BinSizeX: " << binSizeX << "; BinSizeY: " << binSizeY << endl;
		//		cout << "Half Height: " << halfHeight << endl;

		for (unsigned int i = 0; i < this->points.size(); i++)
		{
			int xIndex = floor((this->points[i].x - this->minPoint.x) / binSizeX);
			int yIndex = floor((this->points[i].y - this->minPoint.y) / binSizeY);

			//			cout << "xIndex: " << xIndex << "; yIndex: " << yIndex << endl;

			pointCount[xIndex][yIndex] += 1;

			if (this->points[i].z > maxZValue[xIndex][yIndex])
			{
				maxZValue[xIndex][yIndex] = this->points[i].z;
			}
		}

		double volume = 0;
		for (int i = 0; i < binCountX; i++)
		{
			for (int j = 0; j < binCountY; j++)
			{
				if (pointCount[i][j] > minPointCount)
				{
					volume += binSizeX * binSizeY * (maxZValue[i][j] + halfHeight);
				}
			}
		}

		this->features[3] = volume / (this->features[0] * this->features[1] * this->features[2]);
	}
}
