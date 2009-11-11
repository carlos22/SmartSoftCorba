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


#include "object_recognition/ObjectClass.hh"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <math.h>

using std::cout;
using std::endl;

using namespace mrpt::math;

namespace mmp
{
	const unsigned int ObjectClass::featureSpaceSize = 3;

	/*************************************************

	 Public Methods

	 *************************************************/

	ObjectClass::ObjectClass()
	{
	}

	ObjectClass::~ObjectClass()
	{
	}

	void ObjectClass::addFeatureVector(const std::vector<double>& features)
	{
		this->featureVectors.push_back(features);
	}

	double ObjectClass::distance(const std::vector<double>& compare) const
	{
		double means[ObjectClass::featureSpaceSize];
		CMatrixD covars(ObjectClass::featureSpaceSize, ObjectClass::featureSpaceSize);
		covariancesAndMean(this->featureVectors, covars, means);

		std::vector<double> vMean(ObjectClass::featureSpaceSize, 1);
		for (size_t i = 0; i < ObjectClass::featureSpaceSize; i++)
		{
			vMean[i] = means[i];
		}

		return mahalanobisDistance(compare, vMean, covars.inv());
	}

	void ObjectClass::getExpectedVector(std::vector<double>& vector) const
	{
		vector.resize(ObjectClass::featureSpaceSize);

		for (unsigned int i = 0; i < vector.size(); i++)
		{
			vector[i] = 0;
		}

		for (unsigned int i = 0; i < this->featureVectors.size(); i++)
		{
			for (unsigned int j = 0; j < this->featureVectors[i].size(); j++)
			{
				vector[j] += this->featureVectors[i][j];
			}
		}

		for (unsigned int i = 0; i < vector.size(); i++)
		{
			vector[i] /= this->featureVectors.size();
		}
	}

	void ObjectClass::print() const
	{
		//std::ofstream file("temp");

		std::cout << "ObjectClass [" << this->name << "][" << this->featureVectors.size() << "]: " << std::endl;
		for (size_t j = 0; j < this->featureVectors.size(); j++)
		{
			std::cout << "Features[" << j << "]: ";
			for (size_t i = 0; i < this->featureVectors[j].size(); i++)
			{
				std::cout << this->featureVectors[j][i] << ", ";
				//file << this->featureVectors[j][i] << " ";
			}
			//file << ";";
			std::cout << std::endl;
		}
		//file.close();

		double means[ObjectClass::featureSpaceSize];
		CMatrixD covars(ObjectClass::featureSpaceSize, ObjectClass::featureSpaceSize);
		covariancesAndMean(this->featureVectors, covars, means);

		std::cout << "Expectation vector: ";
		for (size_t i = 0; i < ObjectClass::featureSpaceSize; i++)
		{
			std::cout << means[i] << ", ";
		}

		std::cout << std::endl << "Covariance matrix: " << std::endl << covars;
		std::cout << std::endl;
	}

/*************************************************

 Private Methods

 *************************************************/

}
