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

#include "object_recognition/DBScan.hh"

#include <time.h>
#include <iostream>

using namespace std;

namespace mmp
{
	/*************************************************

	 Public Methods

	 *************************************************/

	DBScan::DBScan(const CSimplePointsMap& map) :
		map(map)
	{
		this->epsilon = 0.015;
		this->fuseEpsilon = 0.04;
		this->minPoints = 8;

		this->processed_InstanceId = 0;
		this->numberOfGeneratedClusters = 0;
		this->clusterId = 1;

		this->label.resize(map.size());
	}

	DBScan::~DBScan()
	{
	}

	void DBScan::getClusters(std::vector<Cluster>& clusters, std::vector<mrpt::math::TPoint3D>& noise)
	{
		// build clusters
		buildClusters();

		// fuse clusters
		fuseClusters();

		std::vector<unsigned int> index(numberOfGeneratedClusters + 1);

		for (unsigned int i = 0; i < index.size(); i++)
		{
			index[i] = 0;
		}

		for (unsigned int i = 0; i < map.size(); i++)
		{
			if (this->label[i] > 0)
			{
				index[this->label[i]]++;
			}
			else
			{
				index[0]++;
			}
		}

		unsigned int counter = 0;
		for (unsigned int i = 0; i < index.size(); i++)
		{
			if (index[i] > 0)
			{
				index[i] = counter;
				//cout << ">> " << i << " | " << index[i] << endl;
				counter++;
			}
		}

		// add built clusters to the cluster list
		clusters.resize(counter - 1);

		for (unsigned int i = 0; i < map.size(); i++)
		{
			mrpt::math::TPoint3D point;
			map.getPoint(i, point);

			int lindex = (this->label[i] < 0) ? 0 : this->label[i];
			unsigned int cindex = index[lindex];
			//cout << "I: " << i << " | Lindex " << lindex << " | Cindex: " << cindex << endl;

			if (cindex == 0)
			{
				noise.push_back(point);
			}
			else
			{
				clusters[cindex-1].addPoint(point);
			}
		}
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	void DBScan::buildClusters()
	{
		// label all points as unclassified
		for (unsigned int i = 0; i < this->label.size(); i++)
		{
			this->label[i] = DBScan::UNCLASSIFIED;
		}

		// build clusters
		for (unsigned int i = 0; i < map.size(); i++)
		{
			// if point is unclassified
			if (this->label[i] == DBScan::UNCLASSIFIED)
			{
				// build a new cluster
				if (expandCluster(i))
				{
					clusterId++;
					numberOfGeneratedClusters++;
				}
			}
		}
	}

	bool DBScan::expandCluster(int index)
	{
		std::list<int> seedList;
		std::vector<int> neighbourList;

		TPoint3D p;
		map.getPoint(index, p);
		getNeighbours(neighbourList, p, this->epsilon);

		if (neighbourList.size() < this->minPoints)
		{
			this->label[index] = DBScan::NOISE;
			return false;
		}

		for (unsigned int i = 0; i < neighbourList.size(); i++)
		{
			// label this point with the current clusterId, because it is in epsilon-range
			this->label[neighbourList[i]] = clusterId;

			// if point isn't start point add it to seed list
			if (neighbourList[i] != index)
			{
				seedList.push_back(neighbourList[i]);
			}
		}

		// Iterate the seedList
		for (std::list<int>::iterator iter = seedList.begin(); iter != seedList.end(); iter++)
		{
			int pointIndex = *iter;

			map.getPoint(pointIndex, p);
			getNeighbours(neighbourList, p, this->epsilon);

			if (neighbourList.size() >= this->minPoints)
			{
				for (unsigned int i = 0; i < neighbourList.size(); i++)
				{
					int neighbourIndex = neighbourList[i];

					int label = this->label[neighbourIndex];
					if (label == DBScan::UNCLASSIFIED || label == DBScan::NOISE)
					{
						if (label == DBScan::UNCLASSIFIED)
						{
							iter = seedList.insert(iter, neighbourIndex);
						}
						this->label[neighbourIndex] = clusterId;
					}
				}
			}

			iter = seedList.erase(iter);
		}

		return true;
	}

	void DBScan::fuseClusters()
	{
		std::vector<int> neighbourList;

		// fuse clusters
		for (unsigned int i = 0; i < map.size(); i++)
		{
			// if point is in a cluster
			if (this->label[i] > 0)
			{
				TPoint3D p;
				map.getPoint(i, p);
				getNeighbours(neighbourList, p, this->fuseEpsilon);

				for (unsigned int j = 0; j < neighbourList.size(); j++)
				{
					if (this->label[neighbourList[j]] != this->label[i] && this->label[neighbourList[j]] > 0)
					{
						changeLabel(this->label[neighbourList[j]], this->label[i]);
					}
				}
			}
		}
	}

	void DBScan::changeLabel(int oldLabel, int newLabel)
	{
		for (unsigned int i = 0; i < map.size(); i++)
		{
			if (this->label[i] == oldLabel)
			{
				this->label[i] = newLabel;
			}
		}
	}

	void DBScan::getNeighbours(std::vector<int>& list, const TPoint3D point, double epsilon)
	{
		list.clear();

		TPoint3D p;
		for (unsigned int i = 0; i < map.size(); i++)
		{
			map.getPoint(i, p);
			if (p.distanceTo(point) <= epsilon)
			{
				list.push_back(i);
			}
		}
	}
}
