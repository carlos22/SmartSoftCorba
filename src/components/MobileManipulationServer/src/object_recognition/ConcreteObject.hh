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


#ifndef CONCRETEOBJECT_H_
#define CONCRETEOBJECT_H_

#include <mrpt/core.h>
#include <string>

using namespace mrpt::math;

namespace mmp {

	class ConcreteObject {

		private:
			uint32_t id;
			std::string objectClass;
			CPose3D pose;

			mrpt::math::TPoint3D minPoint;
			mrpt::math::TPoint3D maxPoint;

		public:
			ConcreteObject();
			virtual ~ConcreteObject();

		public:

			void setId(uint32_t id)
			{
				this->id = id;
			}

			uint32_t getId() const
			{
				return id;
			}

			void setObjectClass(const std::string& objClass)
			{
				this->objectClass = objClass;
			}

			const std::string& getObjectClass() const
			{
				return objectClass;
			}

			void setPose(const CPose3D& pose)
			{
				this->pose = pose;
			}

			CPose3D& getPose()
			{
				return pose;
			}

			const CPose3D& getPose() const
			{
				return pose;
			}

			void setMinPoint(const mrpt::math::TPoint3D& point)
			{
				minPoint = point;
			}

			const mrpt::math::TPoint3D& getMinPoint()
			{
				return minPoint;
			}

			void setMaxPoint(const mrpt::math::TPoint3D& point)
			{
				maxPoint = point;
			}

			const mrpt::math::TPoint3D& getMaxPoint()
			{
				return maxPoint;
			}

	};

}

#endif /* CONCRETEOBJECT_H_ */
