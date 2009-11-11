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


#include "object_recognition/Mapper.hh"

#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

namespace mmp
{

	void ransac3Dplane_fit(const CMatrixDouble &allData, const vector_size_t &useIndices,
			vector<CMatrixDouble> &fitModels);
	void ransac3Dplane_distance(const CMatrixDouble &allData, const vector<CMatrixDouble> & testModels,
			const double distanceThreshold, unsigned int & out_bestModelIndex, vector_size_t & out_inlierIndices);
	bool ransac3Dplane_degenerate(const CMatrixDouble &allData, const mrpt::vector_size_t &useIndices);

	/*************************************************

	 Public Methods

	 *************************************************/
	Mapper::Mapper()
	{
		this->tableTrashhold = 0.015;

		// minimum distance between to Laser Points
		this->map.insertionOptions.minDistBetweenLaserPoints = 0.003;
	}

	Mapper::~Mapper()
	{
	}

	void Mapper::handleLaserScan(const CObservation2DRangeScan& scan)
	{
		this->map.loadFromRangeScan(scan);
	}

	void Mapper::clipOutOfRange(float maxRangeXY, float minZ, float maxZ)
	{
		CPoint2D origin(0, 0);

		map.clipOutOfRange(origin, maxRangeXY);
		map.clipOutOfRangeInZ(minZ, maxZ);
	}

	const CSimplePointsMap& Mapper::getMap() const
	{
		return this->map;
	}

	void Mapper::getObjectMap(ObjectRecognition& obj)
	{
		TPlane plane = this->calculateTablePlane();
		obj.setTablePlane(plane);

		double tableNormalVec[3];
		plane.getUnitaryNormalVector(tableNormalVec);

		// checks the direction of the z value and correct it if it points downwards
		int upPositiveZ = (tableNormalVec[2] < 0) ? -1 : 1;

		for (unsigned int i = 0; i < this->map.size(); i++)
		{
			TPoint3D point;
			this->map.getPoint(i, point);

			double dist = plane.distance(point);
			double dir = plane.evaluatePoint(point);

			// objects on the table
			if (dist > this->tableTrashhold && upPositiveZ * dir > 0)
			{
				obj.addPoint(point);
			}
			// table plane
			else if (dist <= this->tableTrashhold)
			{
				obj.addTablePoint(point);
			}
		}
	}

	void Mapper::clearMap()
	{
		this->map.clear();
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	TPlane Mapper::calculateTablePlane()
	{
		CMatrixDouble data(3, this->map.size());

		float x;
		float y;
		float z;

		cout << "Map size: " << this->map.size();

		for (unsigned int i = 0; i < this->map.size(); i++)
		{
			map.getPoint(i, x, y, z);
			data(0, i) = x;
			data(1, i) = y;
			data(2, i) = z;
		}

		// Run RANSAC
		// ------------------------------------
		CMatrixDouble best_model;
		vector_size_t best_inliers;

		CTicTac tictac;
		const size_t TIMES = 50;

		for (size_t iters = 0; iters < TIMES; iters++)
			math::RANSAC::execute(data, ransac3Dplane_fit, ransac3Dplane_distance, ransac3Dplane_degenerate,
					this->tableTrashhold, 3, // Minimum set of points
					best_inliers, best_model, false//iters == 0 // Verbose
			);

		cout << "Computation time: " << tictac.Tac() << " s" << endl;

		ASSERT_(size(best_model,1)==1 && size(best_model,2)==4)

		cout << "RANSAC finished: Best model: " << best_model << endl;
		cout.flush();
		//	cout << "Best inliers: " << best_inliers << endl;

		TPlane temp(best_model(0, 0), best_model(0, 1), best_model(0, 2), best_model(0, 3));

		return temp;

	}

	/*************************************************

	 RANSAC Helper Methods

	 *************************************************/
	void ransac3Dplane_fit(const CMatrixDouble &allData, const vector_size_t &useIndices,
			vector<CMatrixDouble> &fitModels)
	{
		ASSERT_(useIndices.size() == 3);

		TPoint3D p1(allData(0, useIndices[0]), allData(1, useIndices[0]), allData(2, useIndices[0]));
		TPoint3D p2(allData(0, useIndices[1]), allData(1, useIndices[1]), allData(2, useIndices[1]));
		TPoint3D p3(allData(0, useIndices[2]), allData(1, useIndices[2]), allData(2, useIndices[2]));

		try
		{
			TPlane plane(p1, p2, p3);
			fitModels.resize(1);
			CMatrixDouble &M = fitModels[0];

			M.setSize(1, 4);
			for (size_t i = 0; i < 4; i++)
				M(0, i) = plane.coefs[i];
		}
		catch (exception &)
		{
			fitModels.clear();
			return;
		}

	}

	void ransac3Dplane_distance(const CMatrixDouble &allData, const vector<CMatrixDouble> & testModels,
			const double distanceThreshold, unsigned int & out_bestModelIndex, vector_size_t &out_inlierIndices)
	{
		ASSERT_( testModels.size()==1 )
		out_bestModelIndex = 0;
		const CMatrixDouble &M = testModels[0];

		ASSERT_( size(M,1)==1 && size(M,2)==4 )

		TPlane plane;
		plane.coefs[0] = M(0, 0);
		plane.coefs[1] = M(0, 1);
		plane.coefs[2] = M(0, 2);
		plane.coefs[3] = M(0, 3);

		const size_t N = size(allData, 2);
		out_inlierIndices.clear();
		out_inlierIndices.reserve(100);
		for (size_t i = 0; i < N; i++)
		{
			const double d = plane.distance(TPoint3D(allData.get_unsafe(0, i), allData.get_unsafe(1, i),
					allData.get_unsafe(2, i)));
			if (d < distanceThreshold)
				out_inlierIndices.push_back(i);
		}
	}

	/** Return "true" if the selected points are a degenerate (invalid) case.
	 */
	bool ransac3Dplane_degenerate(const CMatrixDouble &allData, const mrpt::vector_size_t &useIndices)
	{
		return false;
	}
}
