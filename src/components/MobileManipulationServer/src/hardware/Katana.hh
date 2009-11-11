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

#ifndef __KATANA_H__
#define __KATANA_H__

#include <string>
#include <vector>
#include "kniBase.h"

using std::vector;
using std::string;

namespace KatanaAPI {

	struct MotorAngles
	{
		double motor1;
		double motor2;
		double motor3;
		double motor4;
		double motor5;

		MotorAngles(double motor1=0, double motor2=0, double motor3=0, double motor4=0, double motor5=0)
		{
			this->motor1 = motor1;
			this->motor2 = motor2;
			this->motor3 = motor3;
			this->motor4 = motor4;
			this->motor5 = motor5;
		}
	};

	/*
	 * - CKatana class is only basic functions like controlling motors by encoder or by angles.
	 * - CikBase class is with Inverse Kinematic, that means that you can now give the
	 * Katanaarm coordinates to which it should move
	 * - CLMBase class is for Linear Movement
	 * - Katana class is a new class which encapsels all important functions from the KNI_API
	 */
	class Katana {

	public:
		enum Motor {
			MotorOne = 0,
			MotorTwo = 1,
			MotorThree = 2,
			MotorFour = 3,
			MotorFive = 4,
			MotorGripper = 5
		};

	private:
		static const int TIMEOUT = 10000;
		static const int GRIPPERTIMEOUT = 2500;

		static const int ENCODER_TOLERANCE = 100;

		static const int ENCODER_GRIPPER_OPEN = 31000;
		static const int ENCODER_GRIPPER_CLOSED = -16768;

		static const int MOTORNUMBER = 5;

		CLMBase* m_katana;

		/*
		 * Init the Katanaarm;
		 *
		 * This Methode will be called from the Constructor.
		 * It will set the Port and Protocol which is used to communicate with the Katana.
		 * This Methode needs a Port and a ConfigFile which will be handed from the Constructor
		 * The Port should be 0 on Linux otherwise 1.
		 * The ConfigFile must fit the Katanaarm (300).
		 */
		void init(int port, const string& configFile);
		/**
		 * Sets the Default Parameter for the Katana Arm.
		 *
		 * PositionCollisionLimits for every Motor
		 * SpeedCollisionLimits for every Motor
		 * Roboter velocity limit
		 * Roboter acceleration limit
		 * Maximum Linear velocity
		 */
		void setDefaultParameters();

		/**
		 * Enables the Crashlimits which has been set before
		 */
		void enableCrashLimits();
		/**
		 * Disables the Crashlimits which has been set before
		 */
		void disableCrashLimits();

	public:

		Katana(int port, const string& configFile);

		virtual ~Katana();

		/**
		 * Calibrates the Katana Arm.
		 *
		 * Crashlimits will be disabled before calibration and afterwards enabled
		 *
		 * ------------ Not sure if its working correctly with the enable / disable Crashlimits -------
		 */
		void calibrate();
		/**
		 * Turns the Robot On and freezes all motors
		 */
		void robotOn();
		/**
		 * Turns the Robot Off and unfreezes all motors
		 *
		 * * Warning * The Katana arm will crash on the ground if it is not in a stable position
		 */
		void robotOff();
		/**
		 * Unblocks the Robot after a Crash.
		 *
		 * This Methode should be called after the Robot has crashed
		 * through a MotorCrashException or similar events. After that
		 * it is possible to move to another position.
		 */
		void unBlockRobot();

		/**
		 * Opens the Gripper which is attached to the Katana arm
		 */
		void openGripper();
		/**
		 * Closes the Gripper which is attached to the Katana arm
		 *
		 * Internally it will throw a MotorTimeoutExeption,
		 * this Exception will be catched so this function can be used normally
		 */
		void closeGripper();

		/**
		 * This Methode returns the Coordinates of the current positon of the Katana arm.
		 * The angles are in Degree.
		 *
		 * The parameters are per reference and will be changed internal, so the parameters are the return value.
		 */
		void getCoordinates_Deg(double& x, double& y, double& z, double& phi,
				double& theta, double& psi);
		/**
		 * This Methode returns the Coordinates of the current positon of the Katana arm.
		 * The angles are in Rad.
		 *
		 * The parameters are per reference and will be changed internal, so the parameters are the return value.
		 */
		void getCoordinates_Rad(double& x, double& y, double& z, double& phi,
				double& theta, double& psi);

		void getAllMotorAngles_Deg(vector<double>& angles);

		void getAllMotorAngles_Rad(vector<double>& angles);

		/**
		 * Moves the specified Motor to the given Angle.
		 * The angle is in Degree.
		 */
		void moveMotorToAngle_Deg(Motor motor, double angle);
		/**
		 * Moves the specified Motor to the given Angle.
		 * The angle is in Rad.
		 */
		void moveMotorToAngle_Rad(Motor motor, double angle);
		/**
		 * Moves the specified Motor to the given Encoder Ticks.
		 */
		void moveMotorToEncoder(Motor motor, int encoder);

		/**
		 * Moves the specified Motor by the given Angle.
		 * The angle is in Degree.
		 */
		void moveMotorByAngle_Deg(Motor motor, double angle);
		/**
		 * Moves the specified Motor by the given Angle.
		 * The angle is in Rad.
		 */
		void moveMotorByAngle_Rad(Motor motor, double angle);
		/**
		 * Moves the specified Motor by the given Encoder Ticks.
		 */
		void moveMotorByEncoder(Motor motor, int encoder);

		/**
		 * Moves all Motors to the given angles
		 * The angles are in Degree.
		 */
		void moveMotorsToAngles_Deg(const vector<double>& angles);
		/**
		 * Moves all Motors to the given angles
		 * The angles are in Rad.
		 */
		void moveMotorsToAngles_Rad(const vector<double>& angles, bool waitUntilReached = false);

		/**
		 * Moves the Robot to a given position.
		 * The angles are in Degree.
		 *
		 * Phi is the angle around the Z-Axis.
		 * Theta is the angle around the rotated X-Axis from the phi-rotation.
		 * Psi is the angle around the rotated Z-Axis from the theta-rotation.
		 *
		 * If the 6 parameters are not corresponding to
		 * each other this methode can throw a NoSolutionException
		 */
		void moveRobotTo_Deg(double x, double y, double z, double phi,
				double theta, double psi);
		/**
		 * Moves the Robot to a given position.
		 * The angles are in Rad.
		 *
		 * Phi is the angle around the Z-Axis.
		 * Theta is the angle around the rotated X-Axis from the phi-rotation.
		 * Psi is the angle around the rotated Z-Axis from the theta-rotation.
		 *
		 * If the 6 parameters are not corresponding to
		 * each other this methode can throw a NoSolutionException
		 */
		void moveRobotTo_Rad(double x, double y, double z, double phi,
				double theta, double psi);

		void calculateIK(double x, double y, double z, double phi,
				double theta, double psi, std::vector<double>& angles);


		/**
		 * Moves the Robot in a linear movment to a given position.
		 * The angles are in Degree.
		 *
		 * Phi is the angle around the Z-Axis.
		 * Theta is the angle around the rotated X-Axis from the phi-rotation.
		 * Psi is the angle around the rotated Z-Axis from the theta-rotation.
		 *
		 * If the 6 parameters are not corresponding to
		 * each other this methode can throw a NoSolutionException
		 */
		void moveRobotLinearTo_Deg(double x, double y, double z, double phi,
				double theta, double psi);
		/** Moves the Robot in a linear movment to a given position.
		 * The angles are in Rad.
		 *
		 * Phi is the angle around the Z-Axis.
		 * Theta is the angle around the rotated X-Axis from the phi-rotation.
		 * Psi is the angle around the rotated Z-Axis from the theta-rotation.
		 *
		 * If the 6 parameters are not corresponding to
		 * each other this methode can throw a NoSolutionException
		 */
		void moveRobotLinearTo_Rad(double x, double y, double z, double phi,
				double theta, double psi);

		/**
		 * Returns the Encoder Ticks of the given Motor.
		 */
		int getMotorEncoders(Motor motor);
	};

}

#endif /* __KATANA_H__ */
