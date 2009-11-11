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


#include "object_recognition/ObjectDatabase.hh"

#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

namespace mmp
{
	/*************************************************

	 Public Methods

	 *************************************************/
	ObjectDatabase::ObjectDatabase()
	{
	}

	ObjectDatabase::~ObjectDatabase()
	{
	}

	void ObjectDatabase::open(const std::string& fileName)
	{
		this->fileName = fileName;
		this->database.loadFromXML(this->fileName);
	}

	void ObjectDatabase::save()
	{
		this->database.saveAsXML(this->fileName);
	}

	void ObjectDatabase::addObjectClass(const ObjectClass& objectClass)
	{
		CSimpleDatabaseTablePtr table = this->database.getTable("Objects");
		if (table->query("name", objectClass.getName()) < 0)
		{
			int objectId = newObjectId();
			size_t index = table->appendRecord();
			table->set(index, "id", asString(objectId));
			table->set(index, "name", objectClass.getName());

			for (size_t i = 0; i < objectClass.size(); i++)
			{
				CSimpleDatabaseTablePtr instanceTable = this->database.getTable("Instances");
				size_t instanceIndex = instanceTable->appendRecord();
				instanceTable->set(instanceIndex, "objectId", asString(objectId));
				instanceTable->set(instanceIndex, "instanceId", asString(i));

				for (size_t j = 0; j < objectClass[i].size(); j++)
				{
					CSimpleDatabaseTablePtr featureTable = this->database.getTable("Features");
					size_t featureIndex = featureTable->appendRecord();
					featureTable->set(featureIndex, "objectId", asString(objectId));
					featureTable->set(featureIndex, "instanceId", asString(i));
					featureTable->set(featureIndex, "dimension", asString(j));
					featureTable->set(featureIndex, "value", asString(objectClass[i][j]));
				}
			}
		}
		else
		{
			std::cout << "DB ERROR: object name already exists!" << std::endl;
		}
	}

	void ObjectDatabase::getAllObjectClasses(std::vector<ObjectClass>& objectClasses)
	{
		CSimpleDatabaseTablePtr table = this->database.getTable("Objects");
		for (size_t i = 0; i < table->getRecordCount(); i++)
		{
			ObjectClass obj;
			readObjectClass(table, i, obj);
			objectClasses.push_back(obj);
		}
	}

	bool ObjectDatabase::getObjectClass(const std::string& objectName, ObjectClass& objectClass)
	{
		CSimpleDatabaseTablePtr table = this->database.getTable("Objects");
		int index = table->query("name", objectName);

		if (index >= 0)
		{
			readObjectClass(table, index, objectClass);
			return true;
		}

		return false;
	}

	/*************************************************

	 Private Methods

	 *************************************************/
	void ObjectDatabase::readObjectClass(CSimpleDatabaseTablePtr table, size_t index, ObjectClass& cl)
	{
		int id = asInt(table->get(index, "id"));
		cl.setName(table->get(index, "name"));

		CSimpleDatabaseTablePtr instanceTable = this->database.getTable("Instances");
		for (size_t i = 0; i < instanceTable->getRecordCount(); i++)
		{
			if (asInt(instanceTable->get(i, "objectId")) == id)
			{
				std::vector<double> features(ObjectClass::featureSpaceSize);
				CSimpleDatabaseTablePtr featureTable = this->database.getTable("Features");
				for (size_t j = 0; j < featureTable->getRecordCount(); j++)
				{
					if (featureTable->get(j, "instanceId") == instanceTable->get(i, "instanceId") && asInt(featureTable->get(
							j, "objectId")) == id)
					{
						int fIndex = asInt(featureTable->get(j, "dimension"));
						features[fIndex] = asDouble(featureTable->get(j, "value"));
					}
				}
				cl.addFeatureVector(features);
			}
		}
	}

	std::string ObjectDatabase::asString(int value)
	{
		std::stringstream temp;
		temp << value;
		return temp.str();
	}

	std::string ObjectDatabase::asString(size_t value)
	{
		std::stringstream temp;
		temp << value;
		return temp.str();
	}

	std::string ObjectDatabase::asString(double value)
	{
		std::stringstream temp;
		temp << value;
		return temp.str();
	}

	int ObjectDatabase::asInt(const std::string& str)
	{
		int value;
		std::stringstream temp(str);
		temp >> value;

		return value;
	}

	double ObjectDatabase::asDouble(const std::string& str)
	{
		double value;
		std::stringstream temp(str);
		temp >> value;

		return value;
	}

	int ObjectDatabase::newObjectId()
	{
		int maxId = 0;

		CSimpleDatabaseTablePtr table = this->database.getTable("Objects");
		for (size_t i = 0; i < table->getRecordCount(); i++)
		{
			int id = asInt(table->get(i, "id"));
			if (id > maxId)
				maxId = id;
		}

		return maxId + 1;
	}
}
