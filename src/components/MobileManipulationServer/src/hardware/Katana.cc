//--------------------------------------------------------------------------
//  Copyright (C) 2009 Jonas Brich
//
//        brich@mail.hs-ulm.de
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

#include <iostream>
#include <math.h>
#include "hardware/Katana.hh"
#include "common/MathHelperFunctions.h"

using KNI_MHF::rad2deg;
using KNI_MHF::deg2rad;

namespace KatanaAPI
{

	// -----------------------------------
	// Constructor / Destructor
	// -----------------------------------

	Katana::Katana(int port, const string& configFile)
	{
		m_katana = new CLMBase();
		init(port, configFile);
		calibrate();
		setDefaultParameters();
		enableCrashLimits();
	}

	Katana::~Katana()
	{
		delete m_katana;
	}

	// -----------------------------------
	// Private Methodes
	// -----------------------------------

	void Katana::init(int port, const string& configFile)
	{
		// Parameters for serial port device, do not change
		TCdlCOMDesc ccd = { port, 57600, 8, 'N', 1, 300, 0 };
		CCdlCOM* device = new CCdlCOM(ccd);
		CCplSerialCRC* protocol = new CCplSerialCRC;
		protocol->init(device);
		// Creates the Katanaarm
		m_katana->create(configFile.c_str(), protocol);
		// Init the LinearMovement
		m_katana->initLM();
	}

	void Katana::setDefaultParameters()
	{
		m_katana->setPositionCollisionLimit(Katana::MotorOne, 250);
		m_katana->setSpeedCollisionLimit(Katana::MotorOne, 20);
		m_katana->setPositionCollisionLimit(Katana::MotorTwo, 200);
		m_katana->setSpeedCollisionLimit(Katana::MotorTwo, 10);
		m_katana->setPositionCollisionLimit(Katana::MotorThree, 250);
		m_katana->setSpeedCollisionLimit(Katana::MotorThree, 40);
		m_katana->setPositionCollisionLimit(Katana::MotorFour, 200);
		m_katana->setSpeedCollisionLimit(Katana::MotorFour, 15);
		m_katana->setPositionCollisionLimit(Katana::MotorFive, 100);
		m_katana->setSpeedCollisionLimit(Katana::MotorFive, 10);
		m_katana->setPositionCollisionLimit(Katana::MotorGripper, 10);
		m_katana->setSpeedCollisionLimit(Katana::MotorGripper, 10);

		m_katana->setRobotVelocityLimit(80);
		m_katana->setRobotAccelerationLimit(1);
		m_katana->setMaximumLinearVelocity(20.0);
	}

	void Katana::enableCrashLimits()
	{
		m_katana->enableCrashLimits();
	}

	void Katana::disableCrashLimits()
	{
		m_katana->disableCrashLimits();
	}

	// -----------------------------------
	// Public Methodes
	// -----------------------------------

	void Katana::calibrate()
	{
		disableCrashLimits();
		m_katana->calibrate();
		enableCrashLimits();
	}

	void Katana::robotOn()
	{
		m_katana->switchRobotOn();
	}

	void Katana::robotOff()
	{
		m_katana->switchRobotOff();
	}

	void Katana::unBlockRobot()
	{
		m_katana->unBlock();
	}

	void Katana::openGripper()
	{
		m_katana->moveMotorToEnc(Katana::MotorGripper, Katana::ENCODER_GRIPPER_OPEN, true, Katana::ENCODER_TOLERANCE, Katana::TIMEOUT);
	}

	void Katana::closeGripper()
	{
		try
		{
			m_katana->moveMotorToEnc(Katana::MotorGripper, Katana::ENCODER_GRIPPER_CLOSED, true, Katana::ENCODER_TOLERANCE, Katana::GRIPPERTIMEOUT);
		} catch (MotorTimeoutException e)
		{
		}
	}

	void Katana::getCoordinates_Deg(double& x, double& y, double& z, double& phi, double& theta, double& psi)
	{
		m_katana->getCoordinates(x, y, z, phi, theta, psi, true);
		phi = rad2deg(phi);
		theta = rad2deg(theta);
		psi = rad2deg(psi);
	}

	void Katana::getCoordinates_Rad(double& x, double& y, double& z, double& phi, double& theta, double& psi)
	{
		m_katana->getCoordinates(x, y, z, phi, theta, psi, true);
	}

	void Katana::getAllMotorAngles_Deg(vector<double>& angles)
	{
		CMotBase* mArray = m_katana->GetBase()->GetMOT()->arr;
		const TMotInit* param = NULL;
		for (int i = 0; i < Katana::MOTORNUMBER; ++i)
		{
			param = mArray[i].GetInitialParameters();
			angles.push_back(rad2deg(param->angleOffset - ((m_katana->getMotorEncoders(i) - param->encoderOffset) * 2 * M_PI)
					/ (double) (param->encodersPerCycle * param->rotationDirection)));
		}
	}

	void Katana::getAllMotorAngles_Rad(vector<double>& angles)
	{
		CMotBase* mArray = m_katana->GetBase()->GetMOT()->arr;
		const TMotInit* param = NULL;
		for (int i = 0; i < Katana::MOTORNUMBER; ++i)
		{
			param = mArray[i].GetInitialParameters();
			angles.push_back(param->angleOffset - ((m_katana->getMotorEncoders(i) - param->encoderOffset) * 2 * M_PI)
					/ (double) (param->encodersPerCycle * param->rotationDirection));
		}
	}

	void Katana::moveMotorToAngle_Deg(Motor motor, double angle)
	{
		m_katana->moveMotorTo(motor, deg2rad(angle), true, Katana::TIMEOUT);
	}

	void Katana::moveMotorToAngle_Rad(Motor motor, double angle)
	{
		m_katana->moveMotorTo(motor, angle, true, Katana::TIMEOUT);
	}

	void Katana::moveMotorToEncoder(Motor motor, int encoder)
	{
		m_katana->moveMotorToEnc(motor, encoder, true, Katana::TIMEOUT);
	}

	void Katana::moveMotorByAngle_Deg(Motor motor, double angle)
	{
		m_katana->moveMotorBy(motor, deg2rad(angle), true, Katana::TIMEOUT);
	}

	void Katana::moveMotorByAngle_Rad(Motor motor, double angle)
	{
		m_katana->moveMotorBy(motor, angle, true, Katana::TIMEOUT);
	}

	void Katana::moveMotorByEncoder(Motor motor, int encoder)
	{
		m_katana->moveMotorByEnc(motor, encoder, true, Katana::TIMEOUT);
	}

	void Katana::moveMotorsToAngles_Deg(const vector<double>& angles)
	{
		for (unsigned int i = 0; i < angles.size(); ++i)
		{
			m_katana->moveMotorTo(i, deg2rad(angles[i]), false);
		}
	}

	void Katana::moveMotorsToAngles_Rad(const vector<double>& angles, bool waitUntilReached)
	{
		std::vector<int> encoders;

		CMotBase* mArray = m_katana->GetBase()->GetMOT()->arr;
		const TMotInit* param = NULL;
		for (unsigned int i = 0; i < angles.size(); ++i)
		{
			param = mArray[i].GetInitialParameters();
			encoders.push_back( ((param->angleOffset - angles[i]) * param->encodersPerCycle * param->rotationDirection) / (2 * M_PI) + param->encoderOffset );
		}

		m_katana->moveRobotToEnc(encoders.begin(), encoders.end(), waitUntilReached);
	}

	void Katana::moveRobotTo_Deg(double x, double y, double z, double phi, double theta, double psi)
	{
		m_katana->IKGoto(x, y, z, deg2rad(phi), deg2rad(theta), deg2rad(psi), true, 100, Katana::TIMEOUT);
	}

	void Katana::moveRobotTo_Rad(double x, double y, double z, double phi, double theta, double psi)
	{
		m_katana->IKGoto(x, y, z, phi, theta, psi, true, 100, Katana::TIMEOUT);

	}

	void Katana::calculateIK(double x, double y, double z, double phi, double theta, double psi, std::vector<double>& angles)
	{
		try
		{
			std::vector<int> encoders(5);
			std::vector<int> actualPosition;


			m_katana->IKCalculate(x * 1000, y* 1000, z* 1000 , rad2deg(phi), rad2deg(theta), rad2deg(psi), encoders.begin(), actualPosition);

			CMotBase* mArray = m_katana->GetBase()->GetMOT()->arr;
			const TMotInit* param = NULL;

			for (size_t i = 0; i < encoders.size() ; i++)
			{
				param = mArray[i].GetInitialParameters();
				angles.push_back(param->angleOffset - ((encoders[i] - param->encoderOffset) * 2 * M_PI)
						/ (double) (param->encodersPerCycle * param->rotationDirection));
			}
		}
		catch (KNI::NoSolutionException e)
		{
			std::cout << "[IK] No solution found!" << std::endl;
			angles.resize(5, 0);
		}
	}

	void Katana::moveRobotLinearTo_Deg(double x, double y, double z, double phi, double theta, double psi)
	{
		// refresh Encoders, so we use the right position
		m_katana->GetBase()->recvMPS();
		m_katana ->movLM(x, y, z, deg2rad(phi), deg2rad(theta), deg2rad(psi), m_katana->getActivatePositionController(),
				m_katana->getMaximumLinearVelocity(), true, 100, Katana::TIMEOUT);
	}

	void Katana::moveRobotLinearTo_Rad(double x, double y, double z, double phi, double theta, double psi)
	{
		// refresh Encoders, so we use the right position
		m_katana->GetBase()->recvMPS();
		m_katana ->movLM(x, y, z, phi, theta, psi, m_katana->getActivatePositionController(), m_katana->getMaximumLinearVelocity(), true, 100,
				Katana::TIMEOUT);
	}

	int Katana::getMotorEncoders(Motor motor)
	{
		return m_katana->getMotorEncoders(motor, true);
	}
}
