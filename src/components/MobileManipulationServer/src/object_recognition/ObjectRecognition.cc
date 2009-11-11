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


#include "object_recognition/ObjectRecognition.hh"

#include "object_recognition/DBScan.hh"

#include <math.h>

namespace mmp
{
	/*************************************************

	 Public Methods

	 *************************************************/

	ObjectRecognition::ObjectRecognition()
	{
	}

	void ObjectRecognition::addPoint(const mrpt::math::TPoint3D& point)
	{
		this->map.insertPoint(point);
	}

	void ObjectRecognition::addTablePoint(const mrpt::math::TPoint3D& point)
	{
		this->table.addPoint(point);
	}

	void ObjectRecognition::doCalculations()
	{
		CTicTac tictac;

		// first calculate table cluster
		mrpt::math::TPoint3D minPoint;
		mrpt::math::TPoint3D maxPoint;
		this->table.getBoundingBox(minPoint, maxPoint);

		// calculate center of point cloud
		CPose3D center(minPoint.x + (maxPoint.x - minPoint.x) / 2, minPoint.y + (maxPoint.y - minPoint.y) / 2,
				minPoint.z + (maxPoint.z - minPoint.z) / 2);

		// calcuate pose
		//tablePlane.getAsPose3D(this->pose);
		this->table.changeOrigin(center);
		this->table.calculateBoundingBox();

		// second perform clustering mit DBSCAN
		this->dbscan();

		cout << ">> Objekterkennung: " << tictac.Tac() << " s" << endl;
	}

	void ObjectRecognition::getMugPositions(Point6D& mug1, Point6D& mug2)
	{
		CTicTac tictac;

		// Bounding Boxen berechnen
		tictac.Tic();
		TPoint3D minPos;
		TPoint3D maxPos;

		int mugCounter = 1;
		for (unsigned int i = 0; i < this->clusters.size(); i++)
		{
			this->clusters[i].getBoundingBox(minPos, maxPos);
			double height = maxPos.z - minPos.z;

			if (height > 0.07 && height < 0.1)
			{
				CPoint3D minP(minPos);
				CPoint3D maxP(maxPos);

				minP = this->clusters[i].getPose() + minP;
				maxP = this->clusters[i].getPose() + maxP;

				double centerX = minP.x() + (maxP.x() - minP.x()) / 2;
				double centerY = minP.y() + (maxP.y() - minP.y()) / 2;
				double centerZ = minP.z() + (maxP.z() - minP.z()) / 2;

				// in mm umrechnen
				centerX *= 1000;
				centerY *= 1000;
				centerZ *= 1000;

				double angle = atan(centerY / centerX);
				double phi = angle + (M_PI / 2);
				double theta = M_PI / 2;
				double psi = 0.0;

				cout << ">> Becher <<" << endl;
				cout << "x: " << centerX << endl;
				cout << "y: " << centerY << endl;
				cout << "z: " << centerZ << endl;
				cout << "phi: " << phi << endl;
				cout << "theta: " << theta << endl;
				cout << "psi: " << psi << endl;
				cout << endl;

				if (mugCounter == 1)
				{
					// in mm umrechnen
					mug1.setX(centerX);
					mug1.setY(centerY);
					mug1.setZ(centerZ);

					mug1.setPhi(phi);
					mug1.setTheta(theta);
					mug1.setPsi(psi);
				}
				else if (mugCounter == 2)
				{
					// in mm umrechnen
					mug2.setX(centerX);
					mug2.setY(centerY);
					mug2.setZ(centerZ);

					mug2.setPhi(phi);
					mug2.setTheta(theta);
					mug2.setPsi(psi);
				}

				mugCounter++;
			}
		}

		cout << ">> Bounding Boxen berechnen: " << tictac.Tac() << " s" << endl << endl;
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	void ObjectRecognition::dbscan()
	{
		DBScan dbscan(map);
		dbscan.getClusters(this->clusters, this->noise);

		// calculate features for all clusters
		for (unsigned int i = 0; i < this->clusters.size(); i++)
		{
			this->clusters[i].calculateFeatures(this->tablePlane);

			cout << endl << ">>>> Cluster " << i << endl;
			this->clusters[i].printFeatures();
		}

	}

}
