//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
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

#include "PTUWrapper.hh"

#include "gen/SmartPTUServer.hh"

#include <EulerTransformationMatrices.hh>

#include <iostream>

PTUWrapper::PTUWrapper()
{
}

PTUWrapper::~PTUWrapper()
{
	CHS::SmartGuard guard(COMP->PTUMutex);
	ptu.close();
	guard.release();
}

bool PTUWrapper::init(const std::string& device)
{
	bool success = false;

	// calculate transformation matrix for robot offset
	EulerTransformationMatrices::create_zyx_matrix(COMP->ini.PTU.x, COMP->ini.PTU.y, COMP->ini.PTU.z, COMP->ini.PTU.azimuth,
			COMP->ini.PTU.elevation, COMP->ini.PTU.roll, ptuRobotOffset);

	default_base_position.set_x(COMP->ini.base.x);
	default_base_position.set_y(COMP->ini.base.y);
	default_base_position.set_z(COMP->ini.base.z);
	default_base_position.set_base_alpha(COMP->ini.base.base_a);
	default_base_position.set_steer_alpha(COMP->ini.base.steer_a);
	default_base_position.set_turret_alpha(COMP->ini.base.turret_a);

	zero_velocity.set_v(0);
	zero_velocity.set_omega_base(0);
	zero_velocity.set_omega_steer(0);
	zero_velocity.set_omega_turret(0);

	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		while (!success)
		{
			std::cout << "[PTUWrapper] init ptu ...\n";
			success = ptu.init(device);
			if (success)
			{
				success = ptu.inmediateExecution(true);
				ptu.clearErrors();
				ptu.save();
			}
			else {
				sleep(1);
			}
		}

		std::cout << "[PTUWrapper] ptu init done!\n";
	}
	guard.release();

	CHS::SmartGuard paramGuard(COMP->ParamMutex);
	{// pan parameters
		std::cout << "setting pan parameters\n";
		globalParam.speed_pan = COMP->ini.hardware.speed_pan;
		globalParam.start_up_speed_pan = COMP->ini.hardware.start_up_speed_pan;
		globalParam.min_speed_pan = COMP->ini.hardware.min_speed_pan;
		globalParam.max_speed_pan = COMP->ini.hardware.max_speed_pan;
		globalParam.acceleration_pan = COMP->ini.hardware.acceleration_pan;

		// tilt parameters
		std::cout << "setting tilt parameters\n";
		globalParam.speed_tilt = COMP->ini.hardware.speed_tilt;
		globalParam.start_up_speed_tilt = COMP->ini.hardware.start_up_speed_tilt;
		globalParam.min_speed_tilt = COMP->ini.hardware.min_speed_tilt;
		globalParam.max_speed_tilt = COMP->ini.hardware.max_speed_tilt;
		globalParam.acceleration_tilt = COMP->ini.hardware.acceleration_tilt;

		globalParam.modified = true;
	}
	paramGuard.release();

	// get current position;
	double pan, tilt;
	getPos(pan, tilt);

	CHS::SmartGuard posGuard(COMP->PTUMutex);
	{
		this->commandedPan = pan;
		this->commandedTilt = tilt;
	}
	posGuard.release();

	start();
	posChanged = true;
	return success;
}

void PTUWrapper::setParameter(const CommPTUObjects::CommPTUParameter& param)
{
	CommPTUObjects::PTUTagType tag = CommPTUObjects::PTUTagType::UNSET;
	double p1, p2, p3, p4, p5, p6;

	std::cout << param << "\n";
	param.get(tag, p1, p2, p3, p4, p5, p6);

	globalParam.reset = false;

	CHS::SmartGuard guard(COMP->ParamMutex);
	switch (tag)
	{
	case CommPTUObjects::PTUTagType::RESET:
	{
		globalParam.reset = true;
		break;
	}
	case CommPTUObjects::PTUTagType::ACCELERATION_PAN:
	{
		globalParam.acceleration_pan = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::ACCELERATION_TILT:
	{
		globalParam.acceleration_tilt = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::SPEED_LIMIT_PAN:
	{
		globalParam.min_speed_pan = p1;
		globalParam.max_speed_pan = p2;
		break;
	}
	case CommPTUObjects::PTUTagType::SPEED_LIMIT_TILT:
	{
		globalParam.min_speed_tilt = p1;
		globalParam.max_speed_tilt = p2;
		break;
	}
	case CommPTUObjects::PTUTagType::SPEED_PAN:
	{
		globalParam.speed_pan = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::SPEED_TILT:
	{
		globalParam.speed_tilt = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::START_UP_SPEED_PAN:
	{
		globalParam.start_up_speed_pan = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::START_UP_SPEED_TILT:
	{
		globalParam.start_up_speed_tilt = p1;
		break;
	}
	case CommPTUObjects::PTUTagType::SENSOR_OFFSET:
	{
		globalParam.sensorOffset.set_x(p1);
		globalParam.sensorOffset.set_y(p2);
		globalParam.sensorOffset.set_z(p3);
		globalParam.sensorOffset.set_azimuth(p4);
		globalParam.sensorOffset.set_elevation(p5);
		globalParam.sensorOffset.set_roll(p6);
		break;
	}
	case CommPTUObjects::PTUTagType::UNSET:
	{
		std::cerr << "[PTU Wrapper] parameter flag is unset\n";
		break;
	}

	}

	globalParam.modified = true;
	guard.release();
}

/*-----------------------------------------------------------------
 move
 ----------------------------------------------------------------*/
void PTUWrapper::move(const CommPTUObjects::CommPTUMoveRequest& request, CommPTUObjects::CommPTUMoveResponse& answer)
{
	PTUStatus status = PTUStatus::GOAL_REACHED;

	if (ptuActive)
	{

		/////////////////////////////
		// apply parameters
		/////////////////////////////

		CHS::SmartGuard paramGuard(COMP->ParamMutex);
		if (globalParam.modified)
		{
			localParam = globalParam;
			globalParam.modified = false;

			if (COMP->ini.PTU.verbose)
			{
				std::cout << "Setting new parameters to PTU ...\n";
			}

			CHS::SmartGuard guard(COMP->PTUMutex);
			if (localParam.reset)
			{
				ptu.reset();
			}

			// pan parameters
			ptu.baseSpeed(CPtuDPerception::Pan, localParam.start_up_speed_pan);
			ptu.lowerSpeed(CPtuDPerception::Pan, localParam.min_speed_pan);
			ptu.upperSpeed(CPtuDPerception::Pan, localParam.max_speed_pan);
			ptu.speed(CPtuDPerception::Pan, localParam.speed_pan);
			ptu.aceleration(CPtuDPerception::Pan, localParam.acceleration_pan);

			// tilt parameters
			ptu.baseSpeed(CPtuDPerception::Tilt, localParam.start_up_speed_tilt);
			ptu.lowerSpeed(CPtuDPerception::Tilt, localParam.min_speed_tilt);
			ptu.upperSpeed(CPtuDPerception::Tilt, localParam.max_speed_tilt);
			ptu.speed(CPtuDPerception::Tilt, localParam.speed_tilt);
			ptu.aceleration(CPtuDPerception::Tilt, localParam.acceleration_tilt);
			guard.release();

			if (COMP->ini.PTU.verbose)
			{
				std::cout << "New parameters were set to PTU\n";
			}
		}
		paramGuard.release();

		/////////////////////////////
		// select move mode
		/////////////////////////////


		CHS::SmartGuard guard(COMP->PTUMutex);
		{
			ptu.powerMode(true, 'H');

			char mode;
			ptu.powerModeQ(true, mode);
			std::cout << "mode: " << mode << "\n";
		}
		guard.release();

		switch (request.get_move_mode())
		{

		case CommPTUObjects::PTUMoveFlag::PAN_ABSOLUTE:
			status = movePanAbs(request.get_pan());
			break;

		case CommPTUObjects::PTUMoveFlag::PAN_RELATIVE:
			status = movePanOff(request.get_pan());
			break;

		case CommPTUObjects::PTUMoveFlag::TILT_ABSOLUTE:
			status = moveTiltAbs(request.get_tilt());
			break;

		case CommPTUObjects::PTUMoveFlag::TILT_RELATIVE:
			status = moveTiltOff(request.get_tilt());
			break;

		case CommPTUObjects::PTUMoveFlag::PAN_TILT_ABSOLUTE:
			status = moveToAbsPos(request.get_pan(), request.get_tilt());
			break;

		case CommPTUObjects::PTUMoveFlag::PAN_TILT_RELATIVE:
			status = moveToOffPos(request.get_pan(), request.get_tilt());
			break;

		case CommPTUObjects::PTUMoveFlag::POSITION_ROBOT:
			status = lookPointRobot(request);
			break;

		case CommPTUObjects::PTUMoveFlag::POSITION_WORLD:
			status = lookPointWorld(request);
			break;

		}
		answer.set_status(status);
	} else
	{
		answer.set_status(CommPTUObjects::PTUMoveStatus::FAILURE);
	}

	posChanged = true;
}

PTUWrapper::PTUStatus PTUWrapper::lookPointRobot(const CommPTUObjects::CommPTUMoveRequest& request)
{
	arma::mat offset;
	arma::vec point(4);
	point[3] = 1;

	offset = localParam.sensorOffset.getHomogeneousMatrix();
	request.get_position(point[0], point[1], point[2]);
	point = inv(ptuRobotOffset) * inv(offset) * point;

	double radius = sqrt(pow(point[0], 2) + pow(point[1], 2) + pow(point[2], 2));
	double pan = atan2(point[1], point[0]);
	double tilt = -1* (acos (point[2] / radius) - M_PI_2);

	return moveToAbsPos(pan, tilt);
}

PTUWrapper::PTUStatus PTUWrapper::lookPointWorld(const CommPTUObjects::CommPTUMoveRequest& request)
{
	CommBasicObjects::CommVoid voidRequest;
	CommBasicObjects::CommBaseState baseState;

	CHS::StatusCode statusCode;
	arma::mat offset;
	arma::mat robotPose;
	arma::vec point(4);
	point[3] = 1;

	if (COMP->ini.base.on_base)
	{
		statusCode = COMP->baseStateQueryClient->query(voidRequest, baseState);
		if (statusCode != CHS::SMART_OK)
		{
			std::cerr << "ERROR during base get update state: " << CHS::StatusCodeConversion(statusCode) << "\n";
			return CommPTUObjects::PTUMoveStatus::FAILURE;
		}
		robotPose = baseState.get_base_position().get_base_pose3d().getHomogeneousMatrix();
	} else
	{
		robotPose = default_base_position.get_base_pose3d().getHomogeneousMatrix();
	}

	std::cout << "robot Pose: " << robotPose << std::endl;

	offset = localParam.sensorOffset.getHomogeneousMatrix();
	request.get_position(point[0], point[1], point[2]);
	point = inv(robotPose) * inv(ptuRobotOffset) * inv(offset) * point;

	double radius = sqrt(pow(point[0], 2) + pow(point[1], 2) + pow(point[2], 2));
	double pan = atan2(point[1], point[0]);
	double tilt = -1* (acos (point[2] / radius) - M_PI_2);

	return moveToAbsPos(pan, tilt);
}

void PTUWrapper::getPos(double& pan, double& tilt)
{

	if (posChanged)
	{
		CHS::SmartGuard guard(COMP->PTUMutex);
		ptu.absPosQ(CPtuDPerception::Pan, currentPan);
		ptu.absPosQ(CPtuDPerception::Tilt, currentTilt);
		guard.release();
		posChanged = false;
	}

	pan = currentPan;
	tilt = currentTilt;
}

void PTUWrapper::start()
{
	ptuActive = true;
}

void PTUWrapper::stop()
{
	ptuActive = false;
	CHS::SmartGuard guard(COMP->PTUMutex);
	ptu.haltAll();
	guard.release();

	if (COMP->ini.PTU.verbose)
	{
		std::cout << ">> PTU halted\n";
	}
}

PTUWrapper::PTUStatus PTUWrapper::movePanAbs(double pan)
{
	std::cout << "[absolute] pan: " << pan << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits
		ptu.clearErrors();
		if (!ptu.moveToAbsPos(CPtuDPerception::Pan, pan))
		{
			checkForErros(status, PAN);
			return status;
		}
		this->commandedPan = pan;
		waitTillPosReached(status);

	}
	guard.release();
	return status;
}

PTUWrapper::PTUStatus PTUWrapper::movePanOff(double pan)
{
	std::cout << "[relative] pan: " << pan << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits
		if (!ptu.moveToOffPos(CPtuDPerception::Pan, pan))
		{
			checkForErros(status, PAN);
			return status;
		}
		this->commandedPan += pan;
		waitTillPosReached(status);

	}
	guard.release();

	return status;
}

PTUWrapper::PTUStatus PTUWrapper::moveTiltAbs(double tilt)
{
	std::cout << "[absolute] tilt: " << tilt << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits
		if (!ptu.moveToAbsPos(CPtuDPerception::Tilt, tilt))
		{
			checkForErros(status, TILT);
			return status;
		}
		this->commandedTilt = tilt;
		waitTillPosReached(status);

	}
	guard.release();

	return status;
}

PTUWrapper::PTUStatus PTUWrapper::moveTiltOff(double tilt)
{
	std::cout << "[relative] tilt: " << tilt << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits

		if (!ptu.moveToOffPos(CPtuDPerception::Tilt, tilt))
		{
			checkForErros(status, TILT);
			return status;
		}
		this->commandedTilt += tilt;
		waitTillPosReached(status);

	}
	guard.release();

	return status;
}

PTUWrapper::PTUStatus PTUWrapper::moveToAbsPos(double pan, double tilt)
{
	std::cout << "[absolute] pan: " << pan << ", tilt: " << tilt << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits


		if (!ptu.moveToAbsPos(CPtuDPerception::Pan, pan))
		{
			checkForErros(status, PAN);
			return status;
		}
		if (!ptu.moveToAbsPos(CPtuDPerception::Tilt, tilt))
		{
			checkForErros(status, TILT);
			return status;
		}
		this->commandedPan = pan;
		this->commandedTilt = tilt;
		waitTillPosReached(status);

	}
	guard.release();

	return status;
}

PTUWrapper::PTUStatus PTUWrapper::moveToOffPos(double pan, double tilt)
{
	std::cout << "[relative] pan: " << pan << ", tilt: " << tilt << "\n";

	PTUStatus status = PTUStatus::GOAL_REACHED;
	CHS::SmartGuard guard(COMP->PTUMutex);
	{
		// check if angle is in limits


		if (!ptu.moveToOffPos(CPtuDPerception::Pan, pan))
		{
			checkForErros(status, PAN);
			return status;
		}
		if (!ptu.moveToOffPos(CPtuDPerception::Tilt, tilt))
		{
			checkForErros(status, TILT);
			return status;
		}
		this->commandedPan += pan;
		this->commandedTilt += tilt;
		waitTillPosReached(status);

	}
	guard.release();

	return status;
}

void PTUWrapper::waitTillPosReached(PTUStatus& status)
{
	uint32_t tries = 0;
	while (ptuActive && tries < waitTries)
	{
		ptu.clearErrors();
		ptu.aWait();

		if (!ptu.timeoutError())
		{
			checkForErros(status, NONE);
			return;
		}
		tries++;
	}

	checkForErros(status, NONE);
}

void PTUWrapper::checkForErros(PTUStatus& status, PTUAxis axis)
{

	// check if there is an error
	if (!ptu.noError() || !ptuActive)
	{

		if (ptu.maxLimitError() || ptu.minLimitError() || ptu.outOfRange())
		{
			if (status == PTUStatus::GOAL_REACHED && axis == PAN)
			{
				status = PTUStatus::PAN_OUT_OF_RANGE;
			} else if (status == PTUStatus::GOAL_REACHED && axis == TILT)
			{
				status = PTUStatus::TILT_OUT_OF_RANGE;
			} else if (status == PTUStatus::TILT_OUT_OF_RANGE && axis == PAN)
			{
				status = PTUStatus::PAN_TILT_OUT_OF_RANGE;
			} else if (status == PTUStatus::PAN_OUT_OF_RANGE && axis == TILT)
			{
				status = PTUStatus::PAN_TILT_OUT_OF_RANGE;
			}
		} else if (!ptuActive)
		{
			ptu.haltAll();
			status = PTUStatus::HALTED;
		} else
		{
			status = PTUStatus::FAILURE;
		}

		if (COMP->ini.PTU.verbose)
		{
			// check if errors occurred during movement
			std::cout << ">> PTU Error: " << ptu.checkErrors() << "\n";
		}

		// clear all errors
		ptu.clearErrors();
	}
}

