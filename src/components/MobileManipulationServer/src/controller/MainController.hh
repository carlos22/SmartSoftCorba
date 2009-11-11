//--------------------------------------------------------------------------
//  Copyright (C) 2009 Jonas Brich, Manuel Wopfner
//
//        brich@mail.hs-ulm.de
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

#ifndef MAINCONTROLLER_H_
#define MAINCONTROLLER_H_

#include "controller/HardwareController.hh"
#include "object_recognition/Mapper.hh"
#include "object_recognition/ConcreteObject.hh"
#include "path_planning/OpenRave.hh"
#include "path_planning/ObjectXMLWriter.hh"
#include "misc/Sem.hh"
#include "misc/Defines.hh"

#include <mrpt/core.h>
#include <vector>

using mrpt::system::TThreadHandle;

namespace mmp
{
	/*************************************************

	 Globle Functions and Variables

	 *************************************************/

	/**
	 * This methode is for the GUI Thread
	 *
	 * It starts the OpenRave wrapper and inits the real OpenRAVE
	 */
	void openRaveGUIHandle(void* param);

	/**
	 * This class controls all actions in the program
	 */
	class MainController
	{
		private:
			/**
			 * Single instance of the main controller
			 */
			static MainController* instance;

		public:
			/**
			 * Method to get the contoller instance
			 */
			static MainController& getInstance();


		private:
			/**
			 * Count of the scans which are necessary to learn a new object.
			 */
			static const int learnObjectCount;

			/**
			 * Value of Mahalanobis distance which itentifies an object.
			 */
			static const double mahalanobisDistance;

			static const std::string OBSTACLE;

			/**
			 * Thread which controls the GUI of OpenRave
			 */
			TThreadHandle openRaveGUIThread;

			/**
			 * Wrapper for easy communication with OpenRave
			 */
			OpenRave* openRave;

			/**
			 * The hardware controller
			 */
			HardwareController hardwareController;

			/**
			 * The mapper which builds the 3d environment map
			 */
			Mapper mapper;

			/**
			 * Writes the Objects to XML which can be read by OpenRave
			 */
			ObjectXMLWriter xmlWriter;

			/**
			 * List of all objects in the scene
			 */
			std::vector<ConcreteObject> concreteObjects;

			/**
			 * Object which represents the table
			 */
			ConcreteObject concreteTable;


		private:
			/**
			 * Private default Constructor for Singleton
			 */
			MainController();

		public:
			/**
			 * Destructor
			 */
			virtual ~MainController();

			/**
			 * Initialize all objects
			 */
			void init();

			/**
			 * Scan environment beginning from the startAngle and scan the area specified
			 * by the scanAngle
			 * @param startAngle Start angle of the first joint (in degree)
			 * @param scanAngle Angle which should be clockwise scaned (in degree)
			 */
			void scanEnvironment(uint16_t startAngle, uint16_t scanAngle);

			/**
			 * Runs the object recognition and returns a list of recognized objects.
			 * Obstacles are not included in the list.
			 * @param objects All recognized objects are returned in this list.
			 */
			void getObjectList(std::vector<ConcreteObject>& objects);

			/**
			 * Starts the manipulation. It adds all objects which are specified in the ids vector
			 * to the OpenRave environment and all other objects as obstacles.
			 * @param ids Ids of all objects which should be manipulated
			 */
			void beginManipulation(const std::vector<uint32_t>& ids);

			/**
			 * Manipulates the object with the specified id an put it to the new position described by
			 * the motor angles.
			 * @param id Id of the object which sould be manipulated.
			 * @param motor01 Angle for the first motor (in rad)
			 * @param motor02 Angle for the second motor (in rad)
			 * @param motor03 Angle for the third motor (in rad)
			 * @param motor04 Angle for the fourth motor (in rad)
			 * @param motor05 Angle for the fifth motor (in rad)
			 * @param command Command what the gripper should do (1 = open gripper, 2 = close gripper)
			 * @return Status string for the manipulated object.
			 */
			std::string manipulateObject(uint32_t id, double motor01, double motor02, double motor03, double motor04, double motor05, uint32_t command);

			/**
			 * Manipulation is done and OpenRave is cleaned up.
			 */
			void endManipulation();

			/**
			 * Move the Katana to the specified motor angles and perform the specified command.
			 * @param motors Angles for all motors
			 * @param command Command what the gripper should do (1 = open gripper, 2 = close gripper)
			 */
			void moveToPoint(KatanaAPI::MotorAngles motors, uint32_t command = 0, bool waitUntilReached = false);

			/**
			 * Moves the Katana to the start position, then turn motors of so the Katana
			 * can be moved by hand. If you right a character and press enter in the server console
			 * the motors are turned on again and the new motor angles are returned.
			 * @param startAngle Motor angles for the start position
			 * @return Motor angles for the new position
			 */
			KatanaAPI::MotorAngles getNewPoint(KatanaAPI::MotorAngles startAngle);


			void calculateIK(double x, double y, double z, double phi,
							 double theta, double psi, std::vector<double>& angles);

		private:
			/**
			 * Adds all objects to OpenRave. The objects specified in the ids vector
			 * are normal objects all others are obstacles.
			 * @param ids Ids of the objects which should be manipulated.
			 */
			void addObjectsToOpenRave(const std::vector<uint32_t>& ids);

			/**
			 * Checks if an id is in the ids list.
			 * @param id Id which should be checked.
			 * @param ids Vector of ids in which should be searched.
			 * @return true = id was found, false = id was not found
			 */
			bool idIsToManipulate(uint32_t id, const std::vector<uint32_t>& ids);



	};
}

#endif /* MAINCONTROLLER_H_ */
