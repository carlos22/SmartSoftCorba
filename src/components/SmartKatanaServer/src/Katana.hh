//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Jonas Brich, Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartKatana component".
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


#ifndef _KATANA_HH
#define _KATANA_HH

#include "smartSoft.hh"

// communication 0bjects
#include <CommVisionObjects/comm3dPointCloud.hh>
#include <CommManipulatorObjects/commManipulatorEventParameter.hh>
#include <CommManipulatorObjects/commManipulatorEventResult.hh>
#include <CommManipulatorObjects/manipulatorEventState.hh>
#include <CommManipulatorObjects/commManipulatorId.hh>
#include <CommManipulatorObjects/commManipulatorParameter.hh>
#include <CommManipulatorObjects/commManipulatorRequestScan3d.hh>
#include <CommManipulatorObjects/commManipulatorTrajectory.hh>
#include <CommManipulatorObjects/commMobileManipulatorState.hh>
#include <CommManipulatorObjects/scanEventState.hh>
#include <CommBasicObjects/commPose3d.hh>
#include <CommBasicObjects/commVoid.hh>
#include <CommBasicObjects/commMobileLaserScan.hh>

// katana api
#include "kniBase.h"

#include <vector>
#include <list>

#include "faulhaberMCDCGripper.hh"

#define KATANA Katana::instance()

/**
 * Singleton class which abstracts the Katana and offers
 * an easy interface for performing trajectories, getting the
 * current state and setting different parameters for the Katana
 */
class Katana
{
public:
	/**
	 * Struct which includes all parameters of the Katana
	 * which can be set from external
	 */
	struct KatanaParameters
	{
		/**
		 * Indicates if one of the parameters was modified since
		 * the last apply
		 */
		bool modified;

		/**
		 * Indicates if only the first trajectory element should be
		 * used (true) or if all should be used (false)
		 */
		bool singlePose;

		/**
		 * Velocity limit for each joint [enc]
		 */
		std::vector<int> jointVelocityLimit;

		/**
		 * Velocity limit for the gripper [enc]
		 */
		int gripperVelocityLimit;

	};

private:

	struct Point3d
	{
		inline Point3d(double px = 0, double py = 0, double pz = 0) :
			x(px), y(py), z(pz)
		{
		}
		double x;
		double y;
		double z;
	};

	/**
	 * Global Katana parameters which can be
	 * modified from external
	 */
	KatanaParameters globalParameters;

	/**
	 * Local Katana parameters which can not
	 * be modified from external
	 */
	KatanaParameters localParameters;

	/**
	 * Number of motors (inclusive gripper)
	 */
	int motorCount;

	/**
	 * Number of joints (exclusive gripper)
	 */
	int jointCount;

	/**
	 * Id of the gripper
	 */
	int gripperId;

	/**
	 * Timeout in [ms] which should maximal be
	 * wait for closing the gripper
	 */
	int gripperTimeout;

	/**
	 * Counter for the continuous id of the state
	 */
	uint32_t stateIdCounter;

	/**
	 * Indicates if the Katana can be used (true).
	 * If an exception like a collision happened it is set to false.
	 */
	bool activated;

	/**
	 * Contains the tcp pose of the last sent trajectory element.
	 */
	std::vector<double> goalPose;

	/**
	 * True if the Gripper has moved to his endpose.
	 */
	bool gripperEndPoseReached;

	/**
	 * Contains the last encoder values which were sent to the Katana
	 */
	std::vector<int> lastEncoderValues;

	/**
	 * The last captured point cloud.
	 */
	std::list<Point3d*> pointCloud;

	/**
	 * Counter for the captured point clouds.
	 */
	uint32_t pointCloudIdCounter;

	/**
	 * singleton instance
	 */
	static Katana *_instance;

	Katana();
	std::auto_ptr<CikBase> katana;
	std::auto_ptr<CCdlBase> device;
	std::auto_ptr<CCplBase> protocol;

	/**
	 * Faulhaber MCDC Gripper
	 */
	FaulhaberMCDCGripper faulhaberGripper;

public:
	/**
	 * Returns the singleton instance of the Katana
	 */
	static Katana *instance();

	virtual ~Katana();

	/**
	 * Initialize the connection to the Katana set the
	 * default parameters and calibrate it.
	 * This method locks the Katana until it is done.
	 */
	void init();

	/**
	 * Perform a given trajectory.
	 * This method locks the Katana until it is done.
	 *
	 * @param trajectory The trajectory which should be performed
	 */
	void performTrajectory(const CommManipulatorObjects::CommManipulatorTrajectory &trajectory);

	/**
	 * Reads the current state if possible from the Katana. If it is not possible
	 * the returned state is invalid.
	 * This method tries to lock the Katana for reading the current state.
	 * If this is possible the state is valid, otherwiese it will be set to invalid
	 *
	 * @param state The current state is written to this parameter
	 */
	void getCurrentState(CommManipulatorObjects::CommManipulatorState &state, bool block = false);

	/**
	 * Set the given parameter to the Katana.
	 * This method locks the Katana until it is done.
	 *
	 * @param param Parameter which should be set to the Katana
	 */
	void setParameter(const CommManipulatorObjects::CommManipulatorParameter &param);

	/**
	 * Switch all motors on.
	 */
	void switchOn();

	/**
	 * Switch all motors off. Before doing this, be sure the Katana
	 * is in a save position.
	 */
	void switchOff();

	void activate()
	{
		activated = true;
	}

	void deactivate()
	{
		activated = false;
	}

	/**
	 * Set activated=true and unblock motors when a collision happend before.
	 */
	void start();

	/**
	 * Set activated=false and freeze all motors so that they are not moving anymore.
	 */
	void stop();

	/**
	 * Returns a reference to the global Katana parameters.
	 * Before modifying the parameters acquire COMP->ParameterMutex.
	 * @return Reference to the global Katana parameters
	 */
	KatanaParameters& getParameters();

	/**
	 * Returns a reference to the global Katana parameters.
	 * @return Const reference to the global Katana parameters
	 */
	const KatanaParameters& getParameters() const;

	/**
	 * Performs a 3d laser scan with the given request parameters and returns
	 * an id for the point cloud.
	 */
	void performLaserScan(const CommManipulatorObjects::CommManipulatorRequestScan3d &request,
			CommManipulatorObjects::ScanEventState &answer);

	/**
	 * Returns the 3d point cloud for a given id. If the id
	 * does not exist the point cloud size is zero.
	 */
	void get3dPointCloud(const CommManipulatorObjects::CommManipulatorId &request, CommVisionObjects::Comm3dPointCloud &answer);

private:
	/**
	 * Copies the global parameters to the local parameters and
	 * apply them for the Katana.
	 */
	void applyGlobalParameters();

	/**
	 * Calibrates the Katana manipulator.
	 */
	void calibrate(bool fastCalibration = false);

	/**
	 * Checks all motors if the had a collision and if one happend
	 * a MotorCrashedException is thrown.
	 */
	void checkForCollision();

	/**
	 * Checks an execute what with the gripper should be done.
	 * @param trajectory The trajectory which is performed
	 * @param encoders If the gripper mode is set to ANGLE the encoder value
	 * 				   for the gripper is appended to the vector
	 * @param index Index of the current trajectory element which is performed
	 */
	void moveManipulator(const CommManipulatorObjects::CommManipulatorTrajectory &trajectory, uint32_t trajectorySize,
			std::vector<int> &encoders, uint32_t index);

	/**
	 * Open the gripper.
	 * @param waitUntilReached
	 * 			true	: wait until the gripper is fully open
	 * 			false	: open gripper but don't wait
	 */
	void openGripper(bool waitUntilReached = true);

	/**
	 * Close the gripper.
	 * @param waitUntilReached
	 * 			true	: wait until the gripper is fully closed
	 * 			false	: close gripper but don't wait
	 */
	void closeGripper(bool waitUntilReached = true);

	/**
	 * Convert a vector of joint angles to a vector of joint encoders.
	 * @param angles Vector of joint angles which should be converted [rad]
	 * @param encoders Vector of joint encoders
	 */
	void convertJointsRadToEnc(const std::vector<double> &angles, std::vector<int>& encoders) const;

	/**
	 * Convert a vector of joint encoders to a vector of joint angles.
	 * @param angles Vector of joint angles [rad]
	 * @param encoders Vector of joint encoders which should be converted
	 */
	void convertJointsEncToRad(std::vector<double> &angles, const std::vector<int>& encoders) const;

	/**
	 * Convert a joint angle value to a joint encoder value.
	 * @param motorId Id of the joint
	 * @param jointAngle Angle of the joint [rad]
	 * @return Encoder value of the joint
	 */
	int convertJointRadToEnc(uint32_t motorId, double jointAngle);

	/**
	 * Convert a joint encoder value to a joint angle value.
	 * @param motorId Id of the joint
	 * @param encoderValue Encoder value of the joint
	 * @return Angle of the joint [rad]
	 */
	double convertJointEncToRad(uint32_t motorId, int encoderValue);

	/**
	 * Converts the velocity of a motor which is given in rad/sec to enc/10ms
	 * which is needed for the katana.
	 */
	int convertVelocityRadToEnc(uint32_t motorId, double value);

	/**
	 * Converts the acceleration of a motor which is given in rad/sec^2 to enc/10ms^2
	 * which is needed for the katana.
	 */
	int convertAccelerationRadToEnc(uint32_t motorId, double value);

	/**
	 * Prints the message and set activated=false
	 */
	void handleException(const std::string& message);

	/**
	 * Sends the given event to all listeners and set activated=false
	 */
	void handleException(const std::string& message, CommManipulatorObjects::ManipulatorEvent event);
};

#endif /* KATANA_HH_ */
