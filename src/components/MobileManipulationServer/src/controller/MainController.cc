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

#include "controller/MainController.hh"

#include "object_recognition/ObjectRecognition.hh"
#include "object_recognition/ObjectDatabase.hh"
#include "gen/MobileManipulationServer.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

namespace mmp
{

	MainController* MainController::instance = 0;

	MainController& MainController::getInstance()
	{
		if (instance == 0)
		{
			instance = new MainController();
		}

		return *instance;
	}

	const int MainController::learnObjectCount = 5;
	const double MainController::mahalanobisDistance = 12;
	const std::string MainController::OBSTACLE = "Obstacle";

	void openRaveGUIHandle(void* param)
	{
		OpenRave* rave = (OpenRave*) param;
		//Init OpenRave Environment
		rave->run();
		sem.release(Sem::INITOPENRAVE);
		while (true)
		{
			sem.acquire(Sem::STARTGUI);
			rave->startGUI();
		}
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	MainController::MainController()
	{
		openRave = NULL;
	}

	/*************************************************

	 Public Methods

	 *************************************************/

	MainController::~MainController()
	{
		if (openRave != NULL)
		{
			delete openRave;
			openRave = NULL;
		}
	}

	void MainController::init()
	{
		//this->openRaveGUIThread = mrpt::system::createThread(openRaveGUIHandle, &this->openRave);
		//sem.acquire(Sem::INITOPENRAVE);

		// add listener
		hardwareController.addLaserListener(&mapper);
	}

	void MainController::scanEnvironment(uint16_t startAngle, uint16_t scanAngle)
	{
		// clear mapper
		mapper.clearMap();

		// move katana to default scan position
		hardwareController.getKatana().moveRobotTo_Deg(208.305, 5.43, 322.20, 91.49, 86.75, -92.46);
		hardwareController.getKatana().moveMotorToAngle_Deg(Katana::MotorOne, startAngle);
		hardwareController.scanRegion(scanAngle);
	}


	void MainController::getObjectList(std::vector<ConcreteObject>& objects)
	{
		ObjectRecognition recognition;

		// clear concrete object list
		concreteObjects.clear();

		mapper.clipOutOfRange(0.7, -0.6, 0.6);
		mapper.getObjectMap(recognition);

		recognition.doCalculations();
		const std::vector<Cluster>& clusters = recognition.getClusters();

		// open object database
		ObjectDatabase database;
		database.open(COMP->ini.general.objectDatabase);

		std::vector<ObjectClass> objClasses;
		database.getAllObjectClasses(objClasses);

		mrpt::math::TPoint3D minPos;
		mrpt::math::TPoint3D maxPos;
		double lastMahalanobisDistance = 1000;

		for (unsigned int i = 0; i < clusters.size(); ++i)
		{
			ConcreteObject cObj;
			cObj.setId(i);
			cObj.setObjectClass(OBSTACLE);	// default class is obstacle
			cObj.setPose(clusters[i].getPose());

			clusters[i].getBoundingBox(minPos, maxPos);
			cObj.setMinPoint(minPos);
			cObj.setMaxPoint(maxPos);

			// compore each cluster which every object class in the database
			lastMahalanobisDistance = 1000;
			for (unsigned int j=0; j < objClasses.size(); j++)
			{
				double distance = objClasses[j].distance(clusters[i].getFeatures());
				if (distance <= mahalanobisDistance && distance < lastMahalanobisDistance)
				{
					cObj.setObjectClass(objClasses[j].getName());
					lastMahalanobisDistance = distance;
				}
			}

			std::cout << cObj.getObjectClass() << " Mahalanobis: " << lastMahalanobisDistance << std::endl;
			concreteObjects.push_back(cObj);
		}

		// Get table
		const Cluster& table = recognition.getTableCluster();
		table.getBoundingBox(minPos, maxPos);

		concreteTable.setId(0);
		concreteTable.setObjectClass("Table");
		concreteTable.setPose(table.getPose());
		concreteTable.setMinPoint(minPos);
		concreteTable.setMaxPoint(maxPos);

		for (uint32_t i = 0; i < concreteObjects.size(); i++)
		{
			if (concreteObjects[i].getObjectClass() != OBSTACLE)
			{
				objects.push_back(concreteObjects[i]);
			}
		}
	}


	void MainController::beginManipulation(const std::vector<uint32_t>& ids)
	{
		cout << ">> reset OpenRave" << endl;

		vector<double> realAngles;
		vector<double> orAngles;

		openRave = new OpenRave();
		openRave->run();

		// Get the current Jointangles from the Katana
		this->hardwareController.getKatana().getAllMotorAngles_Rad(realAngles);

		// Manually add one more Joint for dummyJoint otherwise it doesn't work
		orAngles = realAngles;
		orAngles.push_back(0);
		orAngles[5] = orAngles[4];
		orAngles[4] = orAngles[3];
		orAngles[3] = 0;

		// Make conversion from Katana Jointangles to OpenRave Jointangles
		realAngles[0] = M_PI - realAngles[0];
		realAngles[1] = M_PI_2 - realAngles[1];
		realAngles[2] = M_PI - realAngles[2];
		realAngles[3] = M_PI - realAngles[3];
		realAngles[4] = M_PI - realAngles[4];

		// Make conversion from Katana Jointangles to OpenRave Jointangles
		orAngles[0] = M_PI - orAngles[0];
		orAngles[1] = M_PI_2 - orAngles[1];
		orAngles[2] = M_PI - orAngles[2];
		orAngles[4] = M_PI - orAngles[4];
		orAngles[5] = M_PI - orAngles[5];

		// Move the Katana in OpenRave to the position the real Katana is
		this->openRave->moveRobotToPosition(orAngles);

		// bring objects into openrave
		cout << ">> add objects to OpenRave" << endl;
		this->addObjectsToOpenRave(ids);
		cout << ">> objects added to OpenRave" << endl;
	}

	std::string MainController::manipulateObject(uint32_t id, double motor01, double motor02, double motor03, double motor04, double motor05, uint32_t command)
	{
		std::string state = "Manipulation done";

		// Hardcoded Endposition which the Objects will be set to

		vector<dReal> endPosition;
		endPosition.push_back(M_PI - motor01);
		endPosition.push_back(M_PI_2 - motor02);
		endPosition.push_back(M_PI - motor03);
		endPosition.push_back(M_PI - motor04);
		endPosition.push_back(M_PI - motor05);

		KinBody* obj = this->openRave->getKinBody(id);

		Trajectory* traj = NULL;
		Trajectory* trajUp = NULL;
		vector<Trajectory::TPOINT> path;
		if (obj != NULL)
		{
			this->openRave->openGripper();
			this->hardwareController.getKatana().openGripper();

			// Move to given Object
			cout << ">> move to object " << id << endl;
			traj = this->openRave->planPath(obj);
			if (traj == NULL)
			{
				state = "NoPlanFound to Object";
				std::cout << state<< std::endl;
				return state;
			}

			this->openRave->moveRobotToPosition(traj);
			path = traj->GetPoints();

			for (unsigned int j = 0; j < path.size(); ++j)
			{
				path[j].q[0] = M_PI - path[j].q[0];
				path[j].q[1] = M_PI_2 - path[j].q[1];
				path[j].q[2] = M_PI - path[j].q[2];
				path[j].q[3] = M_PI - path[j].q[3];
				path[j].q[4] = M_PI - path[j].q[4];
				this->hardwareController.getKatana().moveMotorsToAngles_Rad(path[j].q);
			}

			this->hardwareController.getKatana().closeGripper();

			// Move Object up a little;
			cout << ">> lift up object" << endl;
			trajUp = this->openRave->planPath(obj, this->openRave->getCurrentIteration(), 0.02);
			if (trajUp == NULL)
			{
				state = "NoPlanFound Up";
				std::cout << state << std::endl;
				return state;
			}

			std::cout << "close gripper" << std::endl;
			this->openRave->closeGripper();
			this->openRave->grab(obj);

			this->openRave->moveRobotToPosition(trajUp);

			path = trajUp->GetPoints();
			for (unsigned int j = 0; j < path.size(); ++j)
			{
				path[j].q[0] = M_PI - path[j].q[0];
				path[j].q[1] = M_PI_2 - path[j].q[1];
				path[j].q[2] = M_PI - path[j].q[2];
				path[j].q[3] = M_PI - path[j].q[3];
				path[j].q[4] = M_PI - path[j].q[4];
				this->hardwareController.getKatana().moveMotorsToAngles_Rad(path[j].q);
			}

			delete trajUp;
			trajUp = NULL;

			delete traj;
			traj = NULL;

			// Move Object to new Position;
			cout << ">> move object " << id << " to new position" << endl;
			traj = this->openRave->planPath(endPosition);
			if (traj == NULL)
			{
				state = "NoPlanFound to EndPosition";
				std::cout << state << std::endl;
				return state;
			}
			this->openRave->moveRobotToPosition(traj);

			path = traj->GetPoints();
			for (unsigned int j = 0; j < path.size(); ++j)
			{
				path[j].q[0] = M_PI - path[j].q[0];
				path[j].q[1] = M_PI_2 - path[j].q[1];
				path[j].q[2] = M_PI - path[j].q[2];
				path[j].q[3] = M_PI - path[j].q[3];
				path[j].q[4] = M_PI - path[j].q[4];
				this->hardwareController.getKatana().moveMotorsToAngles_Rad(path[j].q);
			}

			if (command == 1)
			{
				std::cout << "open gripper" << std::endl;
				this->hardwareController.getKatana().openGripper();
				this->openRave->openGripper();
			}
			this->openRave->release(obj);
			this->openRave->deleteObject(obj);

			delete traj;
			traj = NULL;
		}

		return state;
	}

	void MainController::endManipulation()
	{

		if (openRave != NULL)
		{
			delete openRave;
			openRave = NULL;
		}

		cout << ">> manipulation finished" << endl;
	}


	void MainController::moveToPoint(KatanaAPI::MotorAngles motors, uint32_t command, bool waitUntilReached)
	{
		std::vector<double> angles;
		angles.push_back(motors.motor1);
		angles.push_back(motors.motor2);
		angles.push_back(motors.motor3);
		angles.push_back(motors.motor4);
		angles.push_back(motors.motor5);

		this->hardwareController.getKatana().moveMotorsToAngles_Rad(angles, waitUntilReached);

		if (command == 1)
		{
			std::cout << "open gripper" << std::endl;
			this->hardwareController.getKatana().openGripper();
		}
		else if(command == 2)
		{
			std::cout << "close gripper" << std::endl;
			this->hardwareController.getKatana().closeGripper();
		}
	}


	KatanaAPI::MotorAngles MainController::getNewPoint(KatanaAPI::MotorAngles startAngle)
	{
		KatanaAPI::MotorAngles newAngles;

		moveToPoint(startAngle, true);

		this->hardwareController.getKatana().robotOff();
		std::string tmp;
		std::cout << ">> When Katana in new Position type a char and press enter";
		std::cin >> tmp;
		this->hardwareController.getKatana().robotOn();

		vector<double> angles;
		this->hardwareController.getKatana().getAllMotorAngles_Rad(angles);

		newAngles.motor1 = angles[0];
		newAngles.motor2 = angles[1];
		newAngles.motor3 = angles[2];
		newAngles.motor4 = angles[3];
		newAngles.motor5 = angles[4];

		return newAngles;
	}


	void MainController::calculateIK(double x, double y, double z, double phi, double theta, double psi, std::vector<double>& angles)
	{
		mrpt::math::CPose3D pose;
		std::vector<double> raveAngles;

		pose.setFromValues(x, y, z, phi, theta, psi);
		this->openRave->calculateIKSolution(pose, raveAngles);
		if (raveAngles.size() > 0)
		{
			this->openRave->convertAnglesOpenRaveToKatana(raveAngles, angles);
		}
	}


	void MainController::addObjectsToOpenRave(const std::vector<uint32_t>& ids)
	{
		mrpt::math::TPoint3D minPos;
		mrpt::math::TPoint3D maxPos;

		double obstacleGrowing = 0.02;
		double obstacleShrinking = 0.012;

		std::ofstream coordFile;
		std::string coordFileName = COMP->ini.general.tempData + "coords.txt";
		coordFile.open(coordFileName.c_str(), std::ios_base::out | std::ios_base::trunc);

		// Objects into OpenRave
		for (uint32_t i = 0; i < concreteObjects.size(); ++i)
		{
			minPos = concreteObjects[i].getMinPoint();
			maxPos = concreteObjects[i].getMaxPoint();

			stringstream stream;
			stream << COMP->ini.general.tempData << "object" << i << ".kinbody.xml";

			if (idIsToManipulate(concreteObjects[i].getId(), ids))
			{
				stringstream temp;
				temp << concreteObjects[i].getId();


				// Write XML File
				this->xmlWriter.writeKinBodyBox(stream.str(), i, maxPos.x - minPos.x - obstacleShrinking,
						maxPos.y - minPos.y - obstacleShrinking, maxPos.z - minPos.z + 0.01);
				this ->openRave->readKinBody(stream.str(), temp.str(), concreteObjects[i].getPose().getHomogeneousMatrixVal().m_Val, false, true);
			} else
			{
				// Write XML File
				this->xmlWriter.writeKinBodyBox(stream.str(), i, maxPos.x - minPos.x + obstacleGrowing, maxPos.y - minPos.y + obstacleGrowing,
						maxPos.z - minPos.z + obstacleGrowing);
				this ->openRave->readKinBody(stream.str(), OBSTACLE, concreteObjects[i].getPose().getHomogeneousMatrixVal().m_Val, true, true);
			}
			coordFile << COMP->ini.general.tempData << "object" << i << ".kinbody.xml";
			coordFile << concreteObjects[i].getPose().getHomogeneousMatrixVal();
			coordFile << endl;

			stream.clear();
		}

		// Get Table
		minPos = concreteTable.getMinPoint();
		maxPos = concreteTable.getMaxPoint();

		this->xmlWriter.writeKinBodyBox(COMP->ini.general.tempData + "table.kinbody.xml", 0, maxPos.x - minPos.x- 0.075, maxPos.y - minPos.y- 0.075, maxPos.z - minPos.z
				- 0.05);
		this ->openRave->readKinBody(COMP->ini.general.tempData + "table.kinbody.xml", "Table", concreteTable.getPose().getHomogeneousMatrixVal().m_Val, true, true);

		coordFile << COMP->ini.general.tempData << "table.kinbody.xml";
		coordFile << concreteTable.getPose().getHomogeneousMatrixVal();
		coordFile << endl;
		coordFile.close();
	}


	bool MainController::idIsToManipulate(uint32_t id, const std::vector<uint32_t>& ids)
	{
		for(uint32_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] == id)
			{
				return true;
			}
		}

		return false;
	}



}
