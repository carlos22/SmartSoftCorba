//--------------------------------------------------------------------------
//  Copyright (C) 2010 Jonas Brich
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartOpenRave component".
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

#include "ObjectDatabase.hh"

//////////////////////////////////////////////////
//
//	Constructors
//
//////////////////////////////////////////////////

ObjectDatabase::ObjectDatabase() {
	this->position = -1;
}

ObjectDatabase::ObjectDatabase(const std::string& filename) {
	this->position = -1;
	this->readObjectDatabase(filename);
}

ObjectDatabase::~ObjectDatabase() {
}

//////////////////////////////////////////////////
//
//	public methods
//
//////////////////////////////////////////////////

bool ObjectDatabase::isKnownObject(const std::string& type) {
	for (size_t i = 0; i < this->objects.size(); ++i) {
		if (this->objects[i].type == type) {
			this->position = i;
			return true;
		}
	}
	return false;
}

ObjectDatabase::ObjectShape ObjectDatabase::getObjectShape(const std::string& type) {
	return this->objects[this->position].shape;
}

void ObjectDatabase::getObjectBoundingBox(double& x, double&y, double& z) {
	x = this->objects[this->position].x;
	y = this->objects[this->position].y;
	z = this->objects[this->position].z;
}

void ObjectDatabase::getObjectCylinder(double& r, double& h) {
	r = this->objects[this->position].r;
	h = this->objects[this->position].h;
}

void ObjectDatabase::getObjectMeshFilename(std::string& filename) {
	filename = this->objects[this->position].meshFilename;
}

//////////////////////////////////////////////////
//
//	private methods
//
//////////////////////////////////////////////////

void ObjectDatabase::readObjectDatabase(const std::string& filename) {
	//TODO: Implement file read and file structure usw.
	ObjectContainer cup;
	cup.type = "ikea-cup-kalas";
	cup.shape = ObjectDatabase::MESH;
	cup.r = 0.040;
	cup.h = 0.080;
	cup.meshFilename = "zafh_models/zafh_ikea_cup_kalas_safety.wrl";
	this->objects.push_back(cup);
	ObjectContainer redbull;
	redbull.type = "redbull-250ml";
	redbull.shape = ObjectDatabase::MESH;
	redbull.r = 0.025;
	redbull.h = 0.140;
	redbull.meshFilename = "zafh_models/zafh_redbull_250ml_safety.wrl";
	this->objects.push_back(redbull);
	ObjectContainer chips;
	chips.type = "pringles-160g";
	chips.shape = ObjectDatabase::MESH;
	chips.r = 0.040;
	chips.h = 0.230;
	chips.meshFilename = "zafh_models/zafh_pringles_160g_safety.wrl";
	this->objects.push_back(chips);
	ObjectContainer chips2;
	chips2.type = "sun-snacks-120g";
	chips2.shape = ObjectDatabase::MESH;
	chips2.r = 0.0425;
	chips2.h = 0.150;
	chips2.meshFilename = "zafh_models/zafh_sunsnacks_120g_safety.wrl";
	this->objects.push_back(chips2);
}
