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
//---------------------------------------------------------------------

#include "ManipulatorFactory.hh"
#include "gen/SmartOpenRave.hh"
#include "Katana.hh"
#include "UniversalRobot.hh"
#include "util/ErrorHandler.hh"

namespace SpecificManipulator {

ManipulatorFactory* ManipulatorFactory::_instance = NULL;

//////////////////////////////////////////////////
//
//	public methods
//
//////////////////////////////////////////////////

ManipulatorFactory* ManipulatorFactory::instance() {
	if (_instance == NULL) {
		CHS::SmartGuard guard(COMP->ManipulatorFactoryMutex);
		if (_instance == NULL) {
			_instance = new ManipulatorFactory();
		}
		guard.release();
	}
	return _instance;
}

ManipulatorFactory::~ManipulatorFactory() {
}

Manipulator* ManipulatorFactory::createManipulatorClass(const std::string& name) {
	if (name == "Katana") {
		return new Katana();
	} else if (name == "UniversalRobot") {
		return new UniversalRobot();
	}
	ErrorHandler::handleMessage(
			"Specific Manipulator class could not be created. [createManipulatorClass in ManipulatorFactory]\n",
			ErrorHandler::ERROR);
	throw new std::exception();
}

//////////////////////////////////////////////////
//
//	private methods
//
//////////////////////////////////////////////////

ManipulatorFactory::ManipulatorFactory() {
}

}
