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


#ifndef OBJECTDATABASE_H_
#define OBJECTDATABASE_H_

#include "object_recognition/ObjectClass.hh"

#include <mrpt/utils/CSimpleDatabase.h>
#include <string>

using namespace mrpt::utils;

namespace mmp
{
	/**
	 * This class gives access to the object database in which
	 * all object classes are contained.
	 */
	class ObjectDatabase
	{
		private:
			std::string fileName;
			CSimpleDatabase database;

		public:
			/**
			 * Default Constructor
			 */
			ObjectDatabase();

			/**
			 * Denstructor
			 */
			virtual ~ObjectDatabase();

			/**
			 * Loads the database from the file
			 * @param fileName
			 */
			void open(const std::string& fileName);

			/**
			 * Saves the database to the opened file
			 */
			void save();


			void addObjectClass(const ObjectClass& objectClass);


			void getAllObjectClasses(std::vector<ObjectClass>& objectClasses);

			/**
			 * Returns the histogram with the given name. If there are more histograms
			 * with the same name, the first is returned
			 * @param objectName Name of the histogram
			 * @param histogram Histogram that should hold the values
			 * @return true = histogram found / false = no histogram found
			 */
			bool getObjectClass(const std::string& className, ObjectClass& objectClass);

		private:

			void readObjectClass(CSimpleDatabaseTablePtr table, size_t index, ObjectClass& cl);

			std::string asString(int value);
			std::string asString(size_t value);
			std::string asString(double value);

			int asInt(const std::string& str);
			double asDouble(const std::string& str);

			int newObjectId();
	};

}

#endif /* OBJECTDATABASE_H_ */
