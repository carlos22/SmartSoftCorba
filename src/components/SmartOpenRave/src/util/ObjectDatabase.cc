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
	ObjectContainer cup_blue;
	cup_blue.type = "IKEA-CUP-SOLBRAEND-BLUE";
	cup_blue.shape = ObjectDatabase::MESH;
	cup_blue.r = 0.040;
	cup_blue.h = 0.081;
	//cup.meshFilename = "data/openRaveModels/ikea_cup_solbraend_safety.wrl";
	cup_blue.meshFilename = "data/openRaveModels/ikea_cup_solbraend.wrl";
	this->objects.push_back(cup_blue);

	ObjectContainer cup_yellow;
	cup_yellow.type = "IKEA-CUP-SOLBRAEND-YELLOW";
	cup_yellow.shape = ObjectDatabase::MESH;
	cup_yellow.r = 0.040;
	cup_yellow.h = 0.081;
	//cup.meshFilename = "data/openRaveModels/ikea_cup_solbraend_safety.wrl";
	cup_yellow.meshFilename = "data/openRaveModels/ikea_cup_solbraend.wrl";
	this->objects.push_back(cup_yellow);

	ObjectContainer redbull;
	redbull.type = "REDBULL-250ML";
	redbull.shape = ObjectDatabase::MESH;
	redbull.r = 0.025;
	redbull.h = 0.140;
	//redbull.meshFilename = "data/openRaveModels/zafh_redbull_250ml_safety.wrl";
	redbull.meshFilename = "data/openRaveModels/zafh_redbull_250ml.wrl";
	this->objects.push_back(redbull);

	ObjectContainer redbull_powershot;
	redbull_powershot.type = "REDBULL-60ML";
	redbull_powershot.shape = ObjectDatabase::MESH;
	redbull_powershot.r = 0.0175;
	redbull_powershot.h = 0.095;
	//redbull.meshFilename = "data/openRaveModels/zafh_redbull_60ml_safety.wrl";
	redbull_powershot.meshFilename = "data/openRaveModels/zafh_redbull_60ml.wrl";
	this->objects.push_back(redbull_powershot);

	ObjectContainer chips;
	chips.type = "PRINGLES-160G";
	chips.shape = ObjectDatabase::MESH;
	chips.r = 0.040;
	chips.h = 0.230;
	//chips.meshFilename = "data/openRaveModels/zafh_pringles_160g_safety.wrl";
	chips.meshFilename = "data/openRaveModels/zafh_pringles_160g.wrl";
	this->objects.push_back(chips);

	ObjectContainer chips2;
	chips2.type = "SUN-SNACKS-120G";
	chips2.shape = ObjectDatabase::MESH;
	chips2.r = 0.0425;
	chips2.h = 0.150;
	chips2.meshFilename = "data/openRaveModels/zafh_sunsnacks_120g_safety.wrl";
	//chips2.meshFilename = "data/openRaveModels/zafh_sunsnacks_120g.wrl";
	this->objects.push_back(chips2);

	ObjectContainer kMilk;
	kMilk.type = "KCLASSIC-H-VOLLMILCH";
	kMilk.shape = ObjectDatabase::MESH;
	kMilk.r = 0.040;
	kMilk.h = 0.081;
	kMilk.meshFilename = "data/openRaveModels/TetraPackLong.wrl";
	this->objects.push_back(kMilk);

	ObjectContainer fairMilk;
	fairMilk.type = "FAIR-MILK";
	fairMilk.shape = ObjectDatabase::MESH;
	fairMilk.r = 0.040;
	fairMilk.h = 0.081;
	fairMilk.meshFilename = "data/openRaveModels/TetraPackLong.wrl";
	this->objects.push_back(fairMilk);

	ObjectContainer oroApple;
	oroApple.type = "RIODORO-APPLEJUICE";
	oroApple.shape = ObjectDatabase::MESH;
	oroApple.r = 0.040;
	oroApple.h = 0.081;
	oroApple.meshFilename = "data/openRaveModels/TetraPackSquare.wrl";
	this->objects.push_back(oroApple);

	ObjectContainer grandeApple;
	grandeApple.type = "RIOGRANDE-APPLEJUICE";
	grandeApple.shape = ObjectDatabase::MESH;
	grandeApple.r = 0.040;
	grandeApple.h = 0.081;
	grandeApple.meshFilename = "data/openRaveModels/TetraPackSquare.wrl";
	this->objects.push_back(grandeApple);

	ObjectContainer sweetKiss;
	sweetKiss.type = "SWEET-KISS-TEA";
	sweetKiss.shape = ObjectDatabase::MESH;
	sweetKiss.r = 0.040;
	sweetKiss.h = 0.081;
	sweetKiss.meshFilename = "data/openRaveModels/TeaTeekanne60G.wrl";
	this->objects.push_back(sweetKiss);

	ObjectContainer coffeeMachine;
	coffeeMachine.type = "COFFEEMACHINE-DELONGHI-ESAM-3500";
	coffeeMachine.shape = ObjectDatabase::MESH;
	coffeeMachine.r = 0.040;
	coffeeMachine.h = 0.081;
	//coffeeMachine.meshFilename = "data/openRaveModels/coffeemachine_delonghi.wrl";
	coffeeMachine.meshFilename = "data/openRaveModels/coffeemachine_delonghi_safety.wrl";
	this->objects.push_back(coffeeMachine);

	ObjectContainer sugarDispenser;
	sugarDispenser.type = "SUGAR-DISPENSER";
	sugarDispenser.shape = ObjectDatabase::MESH;
	sugarDispenser.r = 0.040;
	sugarDispenser.h = 0.081;
	sugarDispenser.meshFilename = "data/openRaveModels/sugar-dispenser_safety.wrl";
	this->objects.push_back(sugarDispenser);

	ObjectContainer cocaCola;
	cocaCola.type = "COCA-COLA-CAN-250ML";
	cocaCola.shape = ObjectDatabase::MESH;
	cocaCola.r = 0.040;
	cocaCola.h = 0.081;
	cocaCola.meshFilename = "data/openRaveModels/cocacolacan250ml.wrl";
	this->objects.push_back(cocaCola);
}
