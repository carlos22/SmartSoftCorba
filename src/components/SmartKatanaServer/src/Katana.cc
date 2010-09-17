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


#include "Katana.hh"

#include "gen/SmartKatanaServer.hh"
#include "common/MathHelperFunctions.h"
#include "KNI_InvKin/KatanaKinematics.h"

#include <commPose3d.hh>
#include <armadillo.hh>
#include <EulerTransformationMatrices.hh>

#include <iostream>

using KNI_MHF::rad2deg;
using KNI_MHF::deg2rad;

Katana* Katana::_instance = NULL;

//////////////////////////////////////////////////
//
//	public methods
//
//////////////////////////////////////////////////

Katana* Katana::instance() {
	if (_instance == NULL) {
		COMP->KatanaMutex.acquire();
		if (_instance == NULL) {
			_instance = new Katana();
		}
		COMP->KatanaMutex.release();
	}
	return _instance;
}

Katana::~Katana() {
}

void Katana::init() {
	Guard guard(COMP->KatanaMutex);

	if (COMP->ini.katana.katana_device == "Lan") {
		int port = COMP->ini.network.port;
		std::string& ip = COMP->ini.network.ip_adress;

		char* tempIp = new char[ip.length() + 1];
		strcpy(tempIp, ip.c_str());

		device.reset(new CCdlSocket(tempIp, port));
	} else {
		int port = 0;
		std::string& pdev = COMP->ini.serial_port.device;

		// Parameters for serial port device, do not change
		TCdlCOMDesc ccd = { port, 57600, 8, 'N', 1, 300, 0 };
		device.reset(new CCdlCOM(ccd, pdev));
	}

	// initialize protocol
	protocol.reset(new CCplSerialCRC());
	protocol->init(device.get()); //fails if no response from Katana

	// create katana
	katana.reset(new CikBase());
	katana->create(COMP->ini.katana.katana_config_file.c_str(), protocol.get());

	// set private members
	motorCount = katana->getNumberOfMotors();
	jointCount = motorCount - 1;

	gripperId = motorCount - 1;
	gripperTimeout = 2500; // [ms]

	activated = false;

	// set point cloud id counter to 0
	pointCloudIdCounter = 0;

	Guard parGuard(COMP->ParameterMutex);
	globalParameters.singlePose = false;
	globalParameters.jointVelocityLimit.resize(motorCount);
	for (int i = 0; i < jointCount; ++i) {
		globalParameters.jointVelocityLimit[i] = katana->getMotorVelocityLimit(i);
	}
	globalParameters.gripperVelocityLimit = katana->getMotorVelocityLimit(gripperId);
	globalParameters.modified = true;
	parGuard.release();

	// calibrate katana
	calibrate();

	guard.release();
}

void Katana::performTrajectory(const Smart::CommManipulatorTrajectory &trajectory) {
	try {
		arma::mat offset_manipulator;
		EulerTransformationMatrices::create_zyx_matrix(-COMP->ini.katana.x, -COMP->ini.katana.y, -COMP->ini.katana.z,
				-COMP->ini.katana.azimuth, -COMP->ini.katana.elevation, -COMP->ini.katana.roll, offset_manipulator);

		// first apply global parameters
		applyGlobalParameters();

		uint32_t trajectorySize = (localParameters.singlePose) ? 1 : trajectory.get_trajectory_size();

		// remove last goal
		Guard encoderGuard(COMP->EncoderMutex);
		goalPose.clear();
		encoderGuard.release();

		// send event that katana is now moving
		Smart::ManipulatorEventState state;
		state.set_event(Smart::GOAL_NOT_REACHED);
		COMP->eventServer->put(state);

		///////////////////////////
		// use joint angles
		///////////////////////////
		if (trajectory.get_valid_values() == Smart::JOINT_ANGLES) {
			std::vector<double> angles(trajectory.get_joint_count());
			std::vector<int> encoders;

			// for each element of the trajectory
			uint32_t i = 0;
			for (; i < trajectorySize; ++i) {
				// if the Katana is not activated cancel the trajectory
				if (!activated)
					return;

				// read all angles of an element
				for (uint32_t j = 0; j < trajectory.get_joint_count(); ++j) {
					angles[j] = trajectory.get_joint_angle(i, j);
				}

				// convert the rad angles to encoder angles
				convertJointsRadToEnc(angles, encoders);

				// lock katana and move manipulator
				Guard guard(COMP->KatanaMutex);
				moveManipulator(trajectory, trajectorySize, encoders, i);
				guard.release();

				Guard encoderGuard(COMP->EncoderMutex);
				lastEncoderValues = encoders;
				encoderGuard.release();
			}

			Guard encoderGuard(COMP->EncoderMutex);
			katana->getCoordinatesFromEncoders(goalPose, encoders);
			encoderGuard.release();
		}

		///////////////////////////
		// use pose_tcp values
		///////////////////////////
		else if (trajectory.get_valid_values() == Smart::POSES) {
			double x, y, z, azimuth, elevation, roll;
			double phi, theta, psi;
			arma::mat tcp_robot;

			// the encoders vector must have this size, otherwise the katana->IKCalcluate
			// call will cause an error.
			std::vector<int> encoders(motorCount);

			// for each element of the trajectory
			for (uint32_t i = 0; i < trajectorySize; ++i) {
				// if the Katana is not activated cancel the trajectory
				if (!activated)
					return;

				trajectory.get_pose_TCP_robot(i, x, y, z, azimuth, elevation, roll, 0.001);
				EulerTransformationMatrices::create_zyx_matrix(x, y, z, azimuth, elevation, roll, tcp_robot);
				arma::mat tcp_manipulator = offset_manipulator * tcp_robot;
				Smart::CommPose3d tcp_manip(tcp_manipulator);

				EulerTransformationMatrices::zyx_to_zxz_angles(tcp_manip.get_azimuth(), tcp_manip.get_elevation(),
						tcp_manip.get_roll(), phi, theta, psi);

				// lock Katana
				Guard guard(COMP->KatanaMutex);

				//				std::cout << tcp_manip.get_x() << ", " << tcp_manip.get_y() << ", " << tcp_manip.get_z() << ", " << phi
				//						<< ", " << theta << ", " << psi << "\n";

				katana->IKCalculate(tcp_manip.get_x(), tcp_manip.get_y(), tcp_manip.get_z(), phi, theta, psi,
						encoders.begin());
				encoders.resize(jointCount);
				moveManipulator(trajectory, trajectorySize, encoders, i);
				guard.release();

				Guard encoderGuard(COMP->EncoderMutex);
				lastEncoderValues = encoders;
				encoderGuard.release();
			}

			Guard encoderGuard(COMP->EncoderMutex);
			katana->getCoordinatesFromEncoders(goalPose, encoders);
			encoderGuard.release();
		}
	} catch (MotorCrashException ex) {
		handleException(ex.message() + " in Katana::performTrajectory()", Smart::COLLISION);

		// TODO: replace start() with switch to error state when feature available
		start();
	} catch (MotorOutOfRangeException ex) {
		handleException(ex.message() + " in Katana::performTrajectory()", Smart::VALUE_OUT_OF_RANGE);
	} catch (KNI::NoSolutionException ex) {
		handleException(ex.message() + " in Katana::performTrajectory()", Smart::NO_SOLUTION_FOUND);
	} catch (Exception ex) {
		handleException(ex.message() + " in Katana::performTrajectory()");
	} catch (...) {
		handleException("Unhandled in Katana::performTrajectory()");
	}
}

void Katana::getCurrentState(Smart::CommManipulatorState &state) {
	try {
		// set id for the state
		state.set_id(stateIdCounter++);

		std::vector<int> encoders(motorCount);
		std::vector<double> angles;
		std::vector<double> pose;

		state.set_pose_manipulator(COMP->ini.katana.x, COMP->ini.katana.y, COMP->ini.katana.z,
				COMP->ini.katana.azimuth, COMP->ini.katana.elevation, COMP->ini.katana.roll);

		// read values from katana
		Guard guard(COMP->KatanaMutex, false);
		if (guard.tryacquire() != -1) {
			// check if the manipulator collided
			checkForCollision();

			// read current encoders from the manipulator
			katana->getRobotEncoders(encoders.begin(), encoders.end(), true);

			// all actions which must be synchronized are done
			guard.release();
		} else {
			Guard encoderGuard(COMP->EncoderMutex);
			encoders = lastEncoderValues;
			encoderGuard.release();
		}

		if (encoders.size() >= (size_t) motorCount) {
			// calculate the pose from the encoders
			katana->getCoordinatesFromEncoders(pose, encoders);

			// check if current pose is the goalPose
			Guard encoderGuard(COMP->EncoderMutex);
			if (goalPose.size() == 6) {
				double posTol = COMP->ini.katana.goal_position_approach;
				double orTol = COMP->ini.katana.goal_angle_approach;

				if (fabs(pose[0] - goalPose[0]) < posTol && fabs(pose[1] - goalPose[1]) < posTol && fabs(pose[2]
						- goalPose[2]) < posTol && fabs(pose[3] - goalPose[3]) < orTol && fabs(pose[4] - goalPose[4])
						< orTol && fabs(pose[5] - goalPose[5]) < orTol) {
					Smart::ManipulatorEventState state;
					state.set_event(Smart::GOAL_REACHED);
					COMP->eventServer->put(state);

					goalPose.clear();
				}
			}
			encoderGuard.release();

			// set pose of the manipulator on the robot
			state.set_pose_manipulator(COMP->ini.katana.x, COMP->ini.katana.y, COMP->ini.katana.z,
					COMP->ini.katana.azimuth, COMP->ini.katana.elevation, COMP->ini.katana.roll);

			// set pose to state object
			double phi, theta, psi;
			EulerTransformationMatrices::zxz_to_zyx_angles(pose[3], pose[4], pose[5], phi, theta, psi);
			state.set_pose_TCP_manipulator(pose[0], pose[1], pose[2], phi, theta, psi, 0.001);

			//			std::cout << pose[0] << ", " << pose[1] << ", " << pose[2] << ", " << pose[3] << ", " << pose[4] << ", "
			//					<< pose[5] << "\n";

			// set joint angles to state object
			convertJointsEncToRad(angles, encoders);
			state.set_joint_count(angles.size() - 1);
			for (size_t i = 0; i < angles.size() - 1; ++i) {
				state.set_joint_angle(i, angles[i]);
			}

			// set angle of gripper
			state.set_gripper_angle(angles[angles.size() - 1]);

			// set state to valid
			state.set_valid(true);
		}

	} catch (MotorCrashException ex) {
		handleException(ex.message() + " in Katana::getCurrentState()", Smart::COLLISION);

		// TODO: replace start() with switch to error state when feature available
		start();
	} catch (Exception ex) {
		handleException(ex.message() + " in Katana::performTrajectory()");
	} catch (...) {
		handleException("Unhandled exception in Katana::getCurrentPose()");
	}
}

void Katana::setParameter(const Smart::CommManipulatorParameter &param) {
	try {
		Smart::ManipulatorParameterMode tag;
		int p1;

		param.get(tag, p1);

		Guard guard(COMP->ParameterMutex);
		switch (tag) {
		case Smart::TRAJECTORY: {
			globalParameters.singlePose = false;
			globalParameters.modified = true;
			break;
		}

		case Smart::SINGLE_POSE: {
			globalParameters.singlePose = true;
			globalParameters.modified = true;
			break;
		}

		case Smart::SET_ALL_JOINT_VELOCITY_LIMITS: {
			// set velocity limit for joints
			for (int i = 0; i < jointCount; i++) {
				globalParameters.jointVelocityLimit[i] = convertVelocityRadToEnc(i, deg2rad(p1));
			}
			globalParameters.modified = true;
			break;
		}

		case Smart::SET_GRIPPER_VELOCITY_LIMIT: {
			// set velocity limit for gripper
			globalParameters.gripperVelocityLimit = convertVelocityRadToEnc(gripperId, deg2rad(p1));
			globalParameters.modified = true;
			break;
		}
		}
		guard.release();
	} catch (...) {
		handleException("Unhandled exception in Katana::setParameters()");
	}

}

void Katana::switchOn() {
	try {
		// lock katana
		Guard guard(COMP->KatanaMutex);
		katana->switchRobotOn();
		guard.release();
	} catch (...) {
		handleException("Unhandled exception in Katana::switchOn()");
	}
}

void Katana::switchOff() {
	try {
		// lock katana
		Guard guard(COMP->KatanaMutex);
		katana->switchRobotOff();
		guard.release();
	} catch (...) {
		handleException("Unhandled exception in Katana::switchOff()");
	}
}

void Katana::start() {
	try {
		activated = true;

		// lock katana
		Guard guard(COMP->KatanaMutex);
		katana->unBlock();
		guard.release();
	} catch (...) {
		handleException("Unhandled exception in Katana::start()");
	}
}

void Katana::stop() {
	try {
		activated = false;

		// lock katana
		Guard guard(COMP->KatanaMutex);
		katana->freezeRobot();
		guard.release();
	} catch (...) {
		handleException("Unhandled exception in Katana::stop()");
	}
}

Katana::KatanaParameters& Katana::getParameters() {
	return globalParameters;
}

const Katana::KatanaParameters& Katana::getParameters() const {
	return globalParameters;
}

void Katana::performLaserScan(const Smart::CommManipulatorRequestScan3d &request, Smart::ScanEventState &answer) {
	try {
		// activate laser
		if (COMP->ini.laser.high_precision) {
			COMP->laserStateClient->setWaitState("query_only");
		} else {
			COMP->laserStateClient->setWaitState("active");
		}

		//acquire point cloud mutex
		Guard guard(COMP->PointCloudMutex);

		// increment point cloud id counter
		if (COMP->ini.katana.verbose)
			std::cout << "clear old point cloud ...\n";

		pointCloudIdCounter++;
		for (std::list<Point3d*>::iterator iter = pointCloud.begin(); iter != pointCloud.end(); iter++) {
			delete *iter;
		}
		pointCloud.clear();

		int num_joints = ((int) request.get_joint_count() < jointCount) ? (int) request.get_joint_count() : jointCount;

		std::vector<double> angles(num_joints);
		std::vector<double> angle_increments(num_joints);
		std::vector<double> angles_end(num_joints);
		std::vector<int> encoders(num_joints);
		std::vector<double> pose;

		if (COMP->ini.katana.verbose)
			std::cout << "calculate encoder values ...\n";

		// get begin encoders
		for (uint32_t j = 0; j < angles.size(); j++) {
			angles[j] = request.get_begin_joint_angle(j);
		}

		// get end encoders
		for (uint32_t j = 0; j < angles_end.size(); j++) {
			angles_end[j] = request.get_end_joint_angle(j);
		}

		// get encoder increments based on the number of steps.
		for (uint32_t j = 0; j < angle_increments.size(); j++) {
			angle_increments[j] = (angles_end[j] - angles[j]) / request.get_step_count();
		}

		Smart::CommVoid laserRequest;
		Smart::CommMobileLaserScan laserResponse;
		arma::vec point(4);

		// get the homogenous matrix of the manipulator on the robot
		Smart::CommPose3d pose_manipulator(COMP->ini.katana.x, COMP->ini.katana.y, COMP->ini.katana.z,
				COMP->ini.katana.azimuth, COMP->ini.katana.elevation, COMP->ini.katana.roll);
		arma::mat mat_manipulator = pose_manipulator.getHomogeneousMatrix();

		Smart::CommPose3d pose_sensor(COMP->ini.laser.x, COMP->ini.laser.y, COMP->ini.laser.z, COMP->ini.laser.azimuth,
				COMP->ini.laser.elevation, COMP->ini.laser.roll);
		arma::mat mat_sensor_offset = pose_sensor.getHomogeneousMatrix();

		if (COMP->ini.katana.verbose)
			std::cout << "performing laser scan ...\n";

		// perform 3d laser scan
		for (uint32_t i = 0; i < request.get_step_count(); ++i) {

			if (!activated)
				return;

			convertJointsRadToEnc(angles, encoders);

			// move manipulator to next scan position
			Guard guard_katana(COMP->KatanaMutex);
			katana->moveRobotToEnc(encoders.begin(), encoders.end(), true);
			guard_katana.release();

			// ask for laser scan
			if (COMP->laserQueryClient->query(laserRequest, laserResponse) == CHS::SMART_OK) {

				if (COMP->ini.katana.verbose)
					std::cout << "laser scan received\n";

				if (laserResponse.get_scan_size() > 0) {
					// calculate the pose from the encoders
					katana->getCoordinatesFromEncoders(pose, encoders);

					// create zxz matrix for tcp pose
					arma::mat mat_tcp(4, 4);
					mat_tcp.zeros();
					EulerTransformationMatrices::create_zxz_matrix(pose[3], pose[4], pose[5], mat_tcp);

					mat_tcp(0, 3) = pose[0];
					mat_tcp(1, 3) = pose[1];
					mat_tcp(2, 3) = pose[2];
					mat_tcp(3, 3) = 1;

					arma::mat mat_trans = mat_manipulator * mat_tcp * mat_sensor_offset;

					// add points to point cloud
					for (Smart::CommMobileLaserScan::const_polar_iterator iter = laserResponse.polar_begin(); iter
							!= laserResponse.polar_end(); iter++) {

						if ((*iter).angle > request.get_laser_begin_angle() || (*iter).angle
								< request.get_laser_end_angle()) {

							point.zeros();
							point.row(0) = (*iter).distance * cos((*iter).angle);
							point.row(1) = (*iter).distance * sin((*iter).angle);
							point.row(3) = 1;

							arma::vec new_point = mat_trans * point;
							pointCloud.push_back(new Point3d(new_point.row(0), new_point.row(1), new_point.row(2)));
						}
					}

				} else if (COMP->ini.katana.verbose) {
					std::cout << "invalid laser scan\n";
				}
			} else {
				std::cerr << ">> query to laser scanner was not possible\n";
			}

			// increment encoders for next step
			for (uint32_t j = 0; j < angle_increments.size(); j++) {
				angles[j] += angle_increments[j];
			}
		}

		if (COMP->ini.katana.verbose)
			std::cout << "laser scan is captured\n";

		answer.set_id(pointCloudIdCounter);
		answer.set_valid(true);

		//release point cloud mutex
		guard.release();

		COMP->laserStateClient->setWaitState("neutral");

	} catch (MotorCrashException ex) {
		handleException(ex.message() + " in Katana::performLaserScan()", Smart::COLLISION);

		// TODO: replace start() with switch to error state when feature available
		start();
	} catch (MotorOutOfRangeException ex) {
		handleException(ex.message() + " in Katana::performLaserScan()", Smart::VALUE_OUT_OF_RANGE);
	} catch (KNI::NoSolutionException ex) {
		handleException(ex.message() + " in Katana::performLaserScan()", Smart::NO_SOLUTION_FOUND);
	} catch (Exception ex) {
		handleException(ex.message() + " in Katana::performLaserScan()");
	} catch (...) {
		handleException("Unhandled in Katana::performLaserScan()");
	}
}

void Katana::get3dPointCloud(const Smart::CommManipulatorId &request, Smart::Comm3dPointCloud &answer) {

	if (request.is_valid() && request.get_id() == pointCloudIdCounter) {
		Guard guard(COMP->PointCloudMutex);

		answer.set_size(pointCloud.size());
		uint32_t index = 0;
		for (std::list<Point3d*>::const_iterator iter = pointCloud.begin(); iter != pointCloud.end(); iter++) {
			answer.set_point(index, (*iter)->x, (*iter)->y, (*iter)->z);
			index++;
		}
		guard.release();
	} else {
		answer.set_size(0);
	}

}

//////////////////////////////////////////////////
//
//	private methods
//
//////////////////////////////////////////////////

Katana::Katana() {
	stateIdCounter = 0;
	pointCloudIdCounter = 0;
}

void Katana::applyGlobalParameters() {
	//-------- copy parameters --------
	Guard parGuard(COMP->ParameterMutex);
	if (globalParameters.modified) {
		// copy global parameters for local use which must not be synchronized
		globalParameters.modified = false;
		localParameters = globalParameters;
		parGuard.release();

		// print the current Katana mode
		if (localParameters.singlePose)
			std::cout << "Katana mode: singlePose\n";
		else
			std::cout << "Katana mode: trajectory\n";

		//-------- set parameters to Katana --------
		Guard guard(COMP->KatanaMutex);

		// set joint velocity limits
		std::cout << "set joint velocity limits: ";
		for (int i = 0; i < jointCount; ++i) {
			std::cout << localParameters.jointVelocityLimit[i] << ", ";
			katana->setMotorVelocityLimit(i, localParameters.jointVelocityLimit[i]);
		}
		std::cout << "\n";

		// set gripper velocity limit
		std::cout << "set gripper velocity limit: " << localParameters.gripperVelocityLimit << "\n";
		katana->setMotorVelocityLimit(gripperId, localParameters.gripperVelocityLimit);

		guard.release();
	}
}

void Katana::calibrate() {
	bool calibrate = true;
	const int encoders = 100;

	// check if gripper collides in both cases (open and close gripper)
	if (COMP->ini.katana.fast_calibration) {
		katana->enableCrashLimits();
		calibrate = false;

		try {
			katana->moveMotorByEnc(1, encoders);
			std::cout << "no calibration required\n";

			// move motor back to previous pos
			katana->moveMotorByEnc(1, 0);
		} catch (...) {
			std::cout << "first calibration collision\n";
			try {
				katana->moveMotorByEnc(1, -encoders);
				std::cout << "no calibration required\n";
			} catch (...) {
				std::cout << "second calibration collision\n" << "calibrate Katana\n";
				calibrate = true;
			}
		}
	}

	if (calibrate) {
		katana->disableCrashLimits();
		katana->calibrate();
		katana->enableCrashLimits();
	}
}

void Katana::checkForCollision() {
	if (activated) {
		const TKatMOT* mot = katana->GetBase()->GetMOT();
		katana->GetBase()->recvGMS(); // get status flags for all motors
		for (int idx = 0; idx < motorCount; idx++) {
			if (mot->arr[idx].GetPVP()->msf == MSF_MOTCRASHED)
				throw MotorCrashException();
		}
	}
}

void Katana::moveManipulator(const Smart::CommManipulatorTrajectory & trajectory, uint32_t trajectorySize, std::vector<
		int> & encoders, uint32_t index) {
	Smart::ManipulatorGripperAction action = trajectory.get_gripper_action();

	// check if the manipulator collided
	checkForCollision();

	// CLOSE_BEFORE_OPEN_AFTER when one element trajectory
	if (trajectorySize == 1 && action == Smart::CLOSE_BEFORE_OPEN_AFTER) {
		closeGripper(true);
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), true);
		openGripper(true);
	}
	// OPEN_BEFORE_CLOSE_AFTER when one element trajectory
	else if (trajectorySize == 1 && action == Smart::OPEN_BEFORE_CLOSE_AFTER) {
		openGripper(true);
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), true);
		closeGripper(true);
	}

	// CLOSE_BEFORE and CLOSE_BEFORE_OPEN_AFTER
	else if (index == 0 && (action == Smart::CLOSE_BEFORE || action == Smart::CLOSE_BEFORE_OPEN_AFTER)) {
		closeGripper(true);
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), trajectory.get_wait_until_each_pose_reached());
	}
	// OPEN_BEFORE and OPEN_BEFORE_CLOSE_AFTER
	else if (index == 0 && (action == Smart::OPEN_BEFORE || action == Smart::OPEN_BEFORE_CLOSE_AFTER)) {
		openGripper(true);
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), trajectory.get_wait_until_each_pose_reached());
	}

	// CLOSE_AFTER and OPEN_BEFORE_CLOSE_AFTER
	else if (index == trajectorySize - 1 && (action == Smart::CLOSE_AFTER || action == Smart::OPEN_BEFORE_CLOSE_AFTER)) {
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), true);
		closeGripper(true);
	}
	// OPEN_AFTER and CLOSE_BEFORE_OPEN_AFTER
	else if (index == trajectorySize - 1 && (action == Smart::OPEN_AFTER || action == Smart::CLOSE_BEFORE_OPEN_AFTER)) {
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), true);
		openGripper(true);
	}

	// ANGLE
	else if (action == Smart::ANGLE) {
		encoders.push_back(convertJointRadToEnc(encoders.size(), trajectory.get_gripper_angle(index)));
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), trajectory.get_wait_until_each_pose_reached());
	} else {
		katana->moveRobotToEnc(encoders.begin(), encoders.end(), trajectory.get_wait_until_each_pose_reached());
	}
}

void Katana::openGripper(bool waitUntilReached) {
	try {
		katana->openGripper(waitUntilReached, gripperTimeout);
	} catch (MotorCrashException ex) {
		//		Is not thrown by Katana for the gripper
		//		std::cout << ex.message() << " in openGripper()\n";
	} catch (MotorTimeoutException ex) {
		//		std::cout << ex.message() << " in openGripper()\n";
	}
}

void Katana::closeGripper(bool waitUntilReached) {
	try {
		katana->closeGripper(waitUntilReached, gripperTimeout);
	} catch (MotorCrashException ex) {
		//		Is not thrown by Katana for the gripper
		//		std::cout << ex.message() << " in closeGripper()\n";
	} catch (MotorTimeoutException ex) {
		//		std::cout << ex.message() << " in closeGripper()\n";
	}
}

void Katana::convertJointsRadToEnc(const std::vector<double>& angles, std::vector<int>& encoders) const {
	// make the encoder vector the same size as the angles vector
	encoders.resize(angles.size());

	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = NULL;

	for (size_t i = 0; i < angles.size(); ++i) {
		// get parameters for the specific joint
		param = mArray[i].GetInitialParameters();

		// convert rad value to encoder value
		encoders[i] = ((param->angleOffset - angles[i]) * param->encodersPerCycle * param->rotationDirection) / (2
				* M_PI) + param->encoderOffset;
	}

}

void Katana::convertJointsEncToRad(std::vector<double>& angles, const std::vector<int>& encoders) const {
	// make the angle vector the same size as the encoder vector
	angles.resize(encoders.size());

	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = NULL;

	for (size_t i = 0; i < encoders.size(); ++i) {
		// get parameters for the specific joint
		param = mArray[i].GetInitialParameters();

		// convert encoder value to rad value
		angles[i] = param->angleOffset - ((encoders[i] - param->encoderOffset) * 2 * M_PI)
				/ (double) (param->encodersPerCycle * param->rotationDirection);
	}
}

int Katana::convertJointRadToEnc(uint32_t motorId, double jointAngle) {
	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = mArray[motorId].GetInitialParameters();

	// convert rad value to encoder value
	return ((param->angleOffset - jointAngle) * param->encodersPerCycle * param->rotationDirection) / (2 * M_PI)
			+ param->encoderOffset;
}

double Katana::convertJointEncToRad(uint32_t motorId, int encoderValue) {
	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = mArray[motorId].GetInitialParameters();

	return param->angleOffset - ((encoderValue - param->encoderOffset) * 2 * M_PI) / (double) (param->encodersPerCycle
			* param->rotationDirection);
}

int Katana::convertVelocityRadToEnc(uint32_t motorId, double value) {
	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = mArray[motorId].GetInitialParameters();

	// convert rad value to encoder value
	int enc = (value * param->encodersPerCycle) / (2 * M_PI * 100);

	// limit encoder to 180 because this is the allowed maximum
	return (enc > 180) ? 180 : enc;
}

int Katana::convertAccelerationRadToEnc(uint32_t motorId, double value) {
	CMotBase* mArray = katana->GetBase()->GetMOT()->arr;
	const TMotInit* param = mArray[motorId].GetInitialParameters();

	// convert rad value to encoder value
	int enc = (value * param->encodersPerCycle) / (2 * M_PI * 100);

	// limit encoder to 4 because this is the allowed maximum
	return (enc > 4) ? 4 : enc;
}

void Katana::handleException(const std::string& message) {
	activated = false;
	goalPose.clear();
	std::cout << message << "\n";
}

void Katana::handleException(const std::string& message, Smart::ManipulatorEvent event) {
	handleException(message);

	// send event to other components
	Smart::ManipulatorEventState state;
	state.set_event(event);
	COMP->eventServer->put(state);
}
