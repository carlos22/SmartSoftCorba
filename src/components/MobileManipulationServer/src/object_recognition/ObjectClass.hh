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


#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <mrpt/core.h>

#include <vector>
#include <string>

using namespace mrpt::math;

namespace mmp
{
	/**
	 * This class represents a special object class
	 */
	class ObjectClass
	{
		public:
			static const unsigned int featureSpaceSize;

		private:
			/**
			 * Name of the object to which the histogram belongs
			 */
			std::string name;
			std::vector<std::vector<double> > featureVectors;

		public:
			/**
			 * Default Constructor
			 */
			ObjectClass();

			/**
			 * Destructor
			 */
			virtual ~ObjectClass();

			/**
			 * Prints the histogram to std::out
			 */
			void print() const;

			void addFeatureVector(const std::vector<double>& features);

			/**
			 * Calculates the Mahalanobis distance.
			 * @param compare vector to which the distance should be calculated.
			 * @return Mahalanobis distance
			 */
			double distance(const std::vector<double>& compare) const;

			void getExpectedVector(std::vector<double>& vector) const;

			std::string getName() const
			{
				return name;
			}

			void setName(const std::string& name)
			{
				this->name = name;
			}

			size_t size() const
			{
				return this->featureVectors.size();
			}

			std::vector<double>& operator[](size_t i)
			{
				return this->featureVectors[i];
			}

			const std::vector<double>& operator[](size_t i) const
			{
				return this->featureVectors[i];
			}
	};

}

#endif /* HISTOGRAM_H_ */
