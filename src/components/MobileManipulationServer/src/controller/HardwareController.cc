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

#include <mrpt/utils/CFileOutputStream.h>

#include "controller/HardwareController.hh"
#include "misc/Defines.hh"
#include "gen/MobileManipulationServer.hh"

#include <string>
#include <iostream>

using mrpt::utils::CFileOutputStream;
using namespace std;

namespace mmp
{
	/*************************************************

	 Public Methods

	 *************************************************/

	HardwareController::HardwareController()
	{
#ifdef KATANA_ATTACHED
		initDevices();
#endif
	}

	HardwareController::~HardwareController()
	{
#ifdef KATANA_ATTACHED
		teardownDevices();
#endif
	}

	void HardwareController::addLaserListener(LaserListener* listener)
	{
		this->laserListeners.push_back(listener);
	}

	void HardwareController::scanRegion(double scanAngle, bool appendScan)
	{

#ifdef DUMP_LASER
		CFileOutputStream file(COMP->ini.general.tempData + "laserdump.txt", appendScan);
#endif

		float degPerScan = 0.76;
		int scanNumber = (int) (scanAngle / degPerScan);

		double x = 0;
		double y = 0;
		double z = 0;
		double phi = 0;
		double theta = 0;
		double psi = 0;

		// perform scan
		laser->turnOn();
		for (int i = 0; i < scanNumber; ++i)
		{
			katana->getCoordinates_Rad(x, y, z, phi, theta, psi);
			Point6D tcp(x / 1000, y / 1000, z / 1000, phi, theta, psi);

			// make sweep and sent it to the listeners
			CObservation2DRangeScan obs;
			makeLaserSweep(obs, tcp);

#ifdef DUMP_LASER
			file << obs;
#endif
			// send scan to all listeners
			sendLaserObject(obs);

			katana->moveMotorByAngle_Deg(Katana::MotorOne, degPerScan);
		}
		laser->turnOff();

		// move katana to standby position
		katana->moveMotorToAngle_Deg(Katana::MotorOne, 90);
		katana->moveRobotTo_Rad(-49.3045, 0.08134, 457.442, -1.5724, 0.2975, -0.271);

#ifdef DUMP_LASER
		file.close();
#endif
	}

	void HardwareController::moveToPoint(Point6D& point)
	{
		try
		{
			katana->closeGripper();
			double tmpZ = point.getZ() + 50.0;
			katana->moveRobotTo_Deg(point.getX(), point.getY(), tmpZ, point.getPhi(), point.getTheta(), point.getPsi());
			katana->openGripper();
			katana->moveRobotLinearTo_Deg(point.getX(), point.getY(), point.getZ(), point.getPhi(), point.getTheta(), point.getPsi());
			katana->closeGripper();
			katana->moveRobotLinearTo_Deg(point.getX(), point.getY(), tmpZ, point.getPhi(), point.getTheta(), point.getPsi());
		} catch (MotorCrashException e)
		{
			cout << "MotorCrashException" << endl;
			katana->unBlockRobot();
		} catch (KNI::NoSolutionException e)
		{
			cout << "NoSolutionException" << endl;
		}

		// In Ruheposition fahren
		katana->moveMotorToAngle_Deg(Katana::MotorTwo, 120);
		katana->moveRobotTo_Deg(-70.17, 0.0, 449.40, -90.79, 21.27, 177.43);
	}

	Point6D HardwareController::getPoint()
	{
		Point6D point;
		try
		{
			katana->robotOff();

			string tmp;
			cin >> tmp;

			katana->robotOn();

			vector<double> angles;
			katana->getAllMotorAngles_Rad(angles);
			for (unsigned int i = 0; i < angles.size(); ++i) {
				std::cout << angles[i] << " ";
			}

			double x = 0;
			double y = 0;
			double z = 0;
			double phi = 0;
			double theta = 0;
			double psi = 0;

			katana->getCoordinates_Deg(x, y, z, phi, theta, psi);

			cout << "x: " << x << endl;
			cout << "y: " << y << endl;
			cout << "z: " << z << endl;
			cout << "phi: " << phi << endl;
			cout << "theta: " << theta << endl;
			cout << "psi: " << psi << endl;

			point.setValues(x, y, z, phi, theta, psi);

		} catch (MotorCrashException e)
		{
			cout << "Exception" << endl;
			katana->unBlockRobot();
		}
		katana->moveMotorToAngle_Deg(Katana::MotorTwo, 120);
		katana->moveRobotTo_Deg(-70.17, 0.0, 449.40, -90.79, 21.27, 177.43);

		return point;
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	void HardwareController::makeLaserSweep(CObservation2DRangeScan& obs, const Point6D& tcp)
	{

		// TODO: read laser offset from file
		CPose3D laserOffset(0.0, 0.09, -0.163);
		bool thereIsObservation, hardError;

		// get laser sweep from device
		this->laser->doProcessSimple(thereIsObservation, obs, hardError);

		if (hardError)
		{
			printf("[TEST] Hardware error=true!!\n");
		}

		if (thereIsObservation)
		{
			// create a new pose from the tcp
			CPose3D toolCenterPoint(tcp.getX(), tcp.getY(), tcp.getZ());

			// get angles which are zxz-euler angles; see katana documentation
			double phi = tcp.getPhi();
			double theta = tcp.getTheta();
			double psi = tcp.getPsi();

			// rotation matrix to transform the zxz-euler angle in angles for the CPose3D
			// so that the laser scan lies in the xy axis and not in the xz axis as it would be
			// without this matrix
			const double zxzCorrection[] = { cos(-M_PI / 2) * cos(-M_PI / 2), -sin(-M_PI / 2), cos(-M_PI / 2) * sin(-M_PI / 2), 0,

			sin(-M_PI / 2) * cos(-M_PI / 2), cos(-M_PI / 2), sin(-M_PI / 2) * sin(-M_PI / 2), 0,

			-sin(-M_PI / 2), 0, cos(-M_PI / 2), 0,

			0, 0, 0, 1 };

			CMatrixDouble44 zxzCorrectionMatrix(zxzCorrection);
			CPose3D zxzCorrectionPose(zxzCorrectionMatrix);

			// the rotation matrix for the zxz euler angle of the tcp
			const double zxzEuler[] = { cos(phi) * cos(psi) - sin(phi) * cos(theta) * sin(psi), -cos(phi) * sin(psi) - sin(phi) * cos(theta) * cos(
					psi), sin(phi) * sin(theta), 0,

			sin(phi) * cos(psi) + cos(phi) * cos(theta) * sin(psi), -sin(phi) * sin(psi) + cos(phi) * cos(theta) * cos(psi), -cos(phi) * sin(theta),
					0,

					sin(theta) * sin(psi), sin(theta) * cos(psi), cos(theta), 0,

					0, 0, 0, 1 };

			CMatrixDouble44 zxzEulerMatrix(zxzEuler);
			CPose3D zxzEulerPose(zxzEulerMatrix);

			// calculate the new pose
			CPose3D newPose = toolCenterPoint + zxzEulerPose + laserOffset + zxzCorrectionPose;

			// set the pose of the laser for this scan
			obs.setSensorPose(newPose);

			// add exclusion area for the robot
			CPolygon p1;
			p1.AddVertex(0.1, 0.3);
			p1.AddVertex(-0.3, 0.3);
			p1.AddVertex(-0.3, -0.3);
			p1.AddVertex(0.1, -0.3);

			std::vector<CPolygon> areas;
			areas.push_back(p1);
			obs.filterByExclusionAreas(areas);
		}
	}

	void HardwareController::sendLaserObject(const CObservation2DRangeScan& scan) const
	{
		std::vector<LaserListener*>::const_iterator iter;
		for (iter = this->laserListeners.begin(); iter != this->laserListeners.end(); iter++)
		{
			(*iter)->handleLaserScan(scan);
		}
	}

	void HardwareController::initDevices()
	{

		// TODO: read config file for hukoyo
		laser = new CHokuyoURG();
		laser->m_com_port = COMP->ini.general.laserPort;

		printf("[TEST] Turning laser ON...\n");
		if (laser->turnOn())
			printf("[TEST] Initialization OK!\n");
		else
			printf("[TEST] Initialization failed!\n");

		printf("Configure Katana\n");
		katana = new Katana(COMP->ini.general.katanaComPortNr, COMP->ini.general.katanaConfig);

	}

	void HardwareController::teardownDevices()
	{
		laser->turnOff();

		delete laser;
		delete katana;
	}
}
