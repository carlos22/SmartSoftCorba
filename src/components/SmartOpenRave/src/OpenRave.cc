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

#include <list>
#include <set>
#include <string>
#include <fstream>
#include "gen/SmartOpenRave.hh"
#include "OpenRave.hh"
#include "util/ObjectXMLWriter.hh"
#include "util/ObjectDatabase.hh"

#include <CommObjectRecognitionObjects/commObjectRecognitionId.hh>
#include <CommObjectRecognitionObjects/commObjectRecognitionObjectProperties.hh>
#include <CommBasicObjects/commPose3d.hh>

#include <boost/python.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

using namespace boost::python;

OpenRave* OpenRave::_instance = NULL;

//////////////////////////////////////////////////
//
//	public methods
//
//////////////////////////////////////////////////

OpenRave* OpenRave::instance() {
	if (_instance == NULL) {
		CHS::SmartGuard guard(COMP->OpenRaveMutex);
		if (_instance == NULL) {
			_instance = new OpenRave();
		}
		guard.release();
	}
	return _instance;
}

OpenRave::~OpenRave() {
	this->environment->Reset();
}

void OpenRave::init() {
	//this->environment = CreateEnvironment(true);
	RaveInitialize(true, Level_Info);
	this->environment = RaveCreateEnvironment();

	OpenRaveGuard guard(this->environment->GetMutex());
	// Load Default Environment with ground
	this->environment->Load(COMP->ini.OpenRave.defaultEnvironmentPath);
	// Load robot arm
	this->manipulator = this->environment->ReadRobotXMLFile(COMP->ini.OpenRave.robotPath);
	if (!this->manipulator) {
		ErrorHandler::handleMessage("Manipulator could not be loaded.", ErrorHandler::ERROR);
		guard.release();
		this->environment->Destroy();
		return;
	}
	// TODO: New Version
	//	// 0 is for 5dof and 1 is for 6dof
	//	this->manipulator->SetActiveManipulator(this->manipulator->GetManipulators().at(1)->GetName());
	// Add Robot to the environment
	this->environment->AddRobot(this->manipulator);

	// TODO: New Version
	//	// Python version
	//	ProblemInstancePtr ikfast = RaveCreateProblem(this->environment, "ikfast");
	//	this->environment->LoadProblem(ikfast, "");
	//	std::stringstream sin, sout;
	//	sin << "LoadIKFastSolver " << this->manipulator->GetName() << " " << (int) IkParameterization::Type_Transform6D;
	//	if (!ikfast->SendCommand(sout, sin)) {
	//		ErrorHandler::handleMessage("IKSolver could not be created.", ErrorHandler::ERROR);
	//		guard.release();
	//		this->environment->Destroy();
	//		return;
	//	}
	guard.release();

	this->planner = RaveCreatePlanner(this->environment, "BiRRT");
	this->problem = RaveCreateProblem(this->environment, "BaseManipulation");
	this->problem->main("Katana6M180");
	this->environment->LoadProblem(this->problem, "");

	ErrorHandler::handleMessage("Initialize of OpenRave done.");
}

void OpenRave::resetAll() {
	// Delete all KinBodies except the manipulator
	this->deleteAllKinBodies(false, false);
	OpenRaveGuard guard(this->environment->GetMutex());
	this->environment->Load(COMP->ini.OpenRave.defaultEnvironmentPath);
	guard.release();
}

void OpenRave::applyParameters() {

	CHS::SmartGuard guard(COMP->ParameterMutex);
	if (this->globalParameters.modified) {
		this->globalParameters.modified = false;

		if (this->globalParameters.clearEnvironment) {
			this->resetAll();
			this->globalParameters.clearEnvironment = false;
		}
		if (this->globalParameters.loadEnvironmentObjectRecognition) {
			this->loadEnvironment(this->globalParameters.loadEnvironmentObjectRecognition_id);
			this->globalParameters.loadEnvironmentObjectRecognition = false;
		}
		if (this->globalParameters.computeGraspTable) {
			// TODO: currently not available
			//			this->computeGraspTable(this->globalParameters.computeGraspTable_id);
			this->globalParameters.computeGraspTable = false;
		}
		if (this->globalParameters.openrave_GraspObject) {
			// TODO: look how the gripper could be closed differently
			this->openGripper();
			this->graspKinBody(this->globalParameters.openrave_GraspObject_id);
			this->globalParameters.openrave_GraspObject = false;
		}
		if (this->globalParameters.openrave_ReleaseObject) {
			this->releaseKinBody(this->globalParameters.openrave_ReleaseObject_id);
			this->globalParameters.openrave_ReleaseObject = false;
		}
		localParameters = globalParameters;
	}
	guard.release();
}

void OpenRave::syncManipulator() {
	std::cout << "OpenRave::syncManipulator()\n";
	if (this->localParameters.parallelization == true && this->parallelization_initial_sync == true) {
		ErrorHandler::handleMessage(
				"Manipulator will not be synchronized because of Parallelization mode. [syncManipulator in OpenRave]",
				ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		return;
	}
	// If the manipulator is attached, we work with it, otherwise we have a default behavior.
	if (COMP->ini.PortParameter.withManipulator) {
		CommManipulatorObjects::CommMobileManipulatorState manipulatorState;
		do {
			CHS::StatusCode status = COMP->manipulatorStatePushTimedClient->getUpdateWait(manipulatorState);

			if (status != CHS::SMART_OK) {
				ErrorHandler::handleMessage("Timed update of ManipulatorState could not be get. [syncManipulator in OpenRave]",
						CommManipulationPlannerObjects::ManipulationPlannerEvent::MANIPULATOR_SYNC_FAIL, ErrorHandler::ERROR);
				return;
			}
		} while (!manipulatorState.is_valid());

		// Sync only once at the beginning. Arm should be stationary during the runtime of the component.
		if (!this->initial_sync) {
			// Translate robot arm so it fits into the environment
			// The 0,0,0 Point is in the second joint angle of the robot arm
			TransformMatrix transform;
			double x = 0;
			double y = 0;
			double z = 0;
			double azimuth = 0;
			double elevation = 0;
			double roll = 0;
			manipulatorState.get_manipulator_state().get_pose_manipulator(x, y, z, azimuth, elevation, roll, 1);

			arma::mat matrix(4, 4);

			EulerTransformationMatrices::create_zyx_matrix(x, y, z, azimuth, elevation, roll, matrix);

			this->copy4x4MatrixToTransformMatrix(matrix, transform);

			double offset_X = 0;
			double offset_Y = 0;
			double offset_Z = 0;
			specificManipulator->getOffsetRealManipulatortoOpenRaveManipulator(offset_X, offset_Y, offset_Z);
			transform.trans.x -= offset_X;
			transform.trans.y -= offset_Y;
			transform.trans.z -= offset_Z;

			OpenRaveGuard guard(this->environment->GetMutex());
			this->manipulator->SetTransform(transform);
			guard.release();

			this->initial_sync = true;
		}

		std::vector<dReal> manipulatorAngles;
		// Get all Angles from the real Manipulator inclusive the gripper
		for (u_int32_t i = 0; i < manipulatorState.get_manipulator_state().get_joint_count(); ++i) {
			manipulatorAngles.push_back(manipulatorState.get_manipulator_state().get_joint_angle(i));
		}
		// TODO: sync without gripper
		//manipulatorAngles.push_back(manipulatorState.get_manipulator_state().get_gripper_angle());

		this->specificManipulator->convertRealAnglesToOpenRaveAngles(manipulatorAngles, manipulatorAngles);

		// Move the robot arm in OpenRave to the same position of real robot arm
		this->moveManipulatorToPosition(manipulatorAngles, false);
		this->parallelization_initial_sync = true;
	} else {
		// Sync only once at the beginning. Arm should be stationary during the runtime of the component.
		if (!this->initial_sync) {
			TransformMatrix transform;
			transform.identity();
			double offset_X = 0;
			double offset_Y = 0;
			double offset_Z = 0;
			specificManipulator->getOffsetRealManipulatortoOpenRaveManipulator(offset_X, offset_Y, offset_Z);
			transform.trans.x -= offset_X;
			transform.trans.y -= offset_Y;
			transform.trans.z = 0.590 - offset_Z;
			OpenRaveGuard guard(this->environment->GetMutex());
			this->manipulator->SetTransform(transform);
			guard.release();

			this->initial_sync = true;
		}

		std::vector<dReal> manipulatorAngles;
		// Calibrate Position
		manipulatorAngles.push_back(M_PI - 3.13371);
		manipulatorAngles.push_back(M_PI_2 - 2.10242);
		manipulatorAngles.push_back(M_PI - 0.99697);
		manipulatorAngles.push_back(M_PI - 1.231);
		manipulatorAngles.push_back(M_PI - 0.271071);

		this->moveManipulatorToPosition(manipulatorAngles, false);
		this->parallelization_initial_sync = true;
	}
	ErrorHandler::handleMessage("Manipulator successfully synchronized. [syncManipulator in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::setParameter(const CommManipulationPlannerObjects::CommManipulationPlannerParameter& param) {
	CommManipulationPlannerObjects::ManipulationPlannerParameterMode tag(
			CommManipulationPlannerObjects::ManipulationPlannerParameterMode::ENV_CLEAR);

	std::stringstream ss;
	int id = 0;
	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;
	double lowerAngle = 0;
	double upperAngle = 0;
	std::string type;

	param.get(tag, id, x, y, z, w, lowerAngle, upperAngle, type);

	CHS::SmartGuard guard(COMP->ParameterMutex);
	switch (tag) {
	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::ENV_CLEAR: {
		ss << "PARAMETER received: ENV_CLEAR";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.clearEnvironment = true;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::ENV_LOAD_OBJECTRECOGNITION: {
		ss << "PARAMETER received: ENV_LOAD_OBJECTRECOGNITION " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.loadEnvironmentObjectRecognition = true;
		this->globalParameters.loadEnvironmentObjectRecognition_id = id;
		this->parallelization_initial_sync = false;
		this->globalParameters.clearEnvironment = true;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::ENV_LOAD_FILE: {
		ss << "PARAMETER received: ENV_LOAD_FILE " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->resetAll();
		this->loadEnvironmentFromFile(id);
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OBJ_DELETE: {
		ss << "PARAMETER received: OBJ_DELETE " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->deleteKinBody(id);
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OBJ_ADD: {
		ss << "PARAMETER received: OBJ_ADD " << id << " " << type;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);

		CommBasicObjects::CommPose3d pose(0, 0, 0, 0, 0, 0, 1);

		ObjectXMLWriter writer;
		ObjectDatabase database("");

		std::stringstream stream;
		stream << id;

		std::string output;
		if (database.isKnownObject(type)) {
			if (database.getObjectShape(type) == ObjectDatabase::BOUNDINGBOX) {
				database.getObjectBoundingBox(x, y, z);
				writer.writeKinBodyBox(stream.str(), x, y, z, output);
			} else if (database.getObjectShape(type) == ObjectDatabase::CYLINDER) {
				database.getObjectCylinder(x, y);
				writer.writeKinBodyCylinder(stream.str(), x, y, output);
				pose.set_roll(pose.get_roll() + M_PI_2);
			} else if (database.getObjectShape(type) == ObjectDatabase::MESH) {
				std::string name;
				database.getObjectMeshFilename(name);
				writer.writeKinBodyMesh(stream.str(), name, output);
				pose.set_roll(pose.get_roll() + M_PI_2);
			}
			this->addKinBody(output, pose.getHomogeneousMatrix(1));
		} else {
			ErrorHandler::handleMessage("Object could not be added. [setParameter in OpenRave]", ErrorHandler::WARNING);
		}
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OBJ_MOVE: {
		ss << "PARAMETER received: OBJ_MOVE " << id << " " << x << " " << y << " " << z;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		std::stringstream name;
		name << id;
		KinBodyPtr kinbody = this->environment->GetKinBody(name.str());
		if (kinbody == NULL) {
			break;
		}
		TransformMatrix transform = kinbody->GetTransform();
		transform.trans.x = x;
		transform.trans.y = y;
		transform.trans.z = z;
		kinbody->SetTransform(transform);
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::ENV_SAVE_FILE: {
		ss << "PARAMETER received: ENV_SAVE_FILE " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->saveEnvironmentToFile(id);
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OBJ_COMPUTEGRASPTABLE: {
		ss << "PARAMETER received: OBJ_COMPUTEGRASPTABLE " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.computeGraspTable = true;
		this->globalParameters.computeGraspTable_id = id;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OPENRAVE_GRASPOBJ: {
		ss << "PARAMETER received: OPENRAVE_GRASPOBJ " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		//		this->openGripper();
		//		this->graspKinBody(id);
		this->globalParameters.openrave_GraspObject = true;
		this->globalParameters.openrave_GraspObject_id = id;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::OPENRAVE_RELEASEOBJ: {
		ss << "PARAMETER received: OPENRAVE_RELEASEOBJ " << id;
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		//		this->releaseKinBody(id);
		this->globalParameters.openrave_ReleaseObject = true;
		this->globalParameters.openrave_ReleaseObject_id = id;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::PARALLELIZATION_ON: {
		ss << "PARAMETER received: PARALLELIZATION_ON";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.parallelization = true;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::PARALLELIZATION_OFF: {
		ss << "PARAMETER received: PARALLELIZATION_OFF";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.parallelization = false;
		this->parallelization_initial_sync = false;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::GRASPING_NONE: {
		ss << "PARAMETER received: GRASPING_NONE";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.grasping_none = true;
		this->globalParameters.grasping_simple = false;
		this->globalParameters.grasping_advanced = false;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::GRASPING_SIMPLE: {
		ss << "PARAMETER received: GRASPING_SIMPLE";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.grasping_none = false;
		this->globalParameters.grasping_simple = true;
		this->globalParameters.grasping_simple_lowerHeightBound = x;
		this->globalParameters.grasping_simple_upperHeightBound = y;
		this->globalParameters.grasping_simple_lowerDepthBound = z;
		this->globalParameters.grasping_simple_upperDepthBound = w;
		this->globalParameters.grasping_simple_lowerAngleBound = lowerAngle;
		this->globalParameters.grasping_simple_upperAngleBound = upperAngle;
		this->globalParameters.grasping_advanced = false;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::GRASPING_ADVANCED: {
		ss << "PARAMETER received: GRASPING_ADVANCED";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.grasping_none = false;
		this->globalParameters.grasping_simple = false;
		this->globalParameters.grasping_advanced = true;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::SIMULATION_TEST_IK_ONLY: {
		ss << "PARAMETER received: SIMULATION_TEST_IK_ONLY";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.simulation_test_ik_only = true;
		this->globalParameters.modified = true;
		break;
	}

	case CommManipulationPlannerObjects::ManipulationPlannerParameterMode::SIMULATION_PLAN_ALL: {
		ss << "PARAMETER received: SIMULATION_PLAN_ALL";
		ErrorHandler::handleMessage(ss.str(), ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		this->globalParameters.simulation_test_ik_only = false;
		this->globalParameters.modified = true;
		break;
	}
	}
	guard.release();
}

OpenRave::OpenRaveParameter OpenRave::getParameter() const {
	return this->localParameters;
}

void OpenRave::moveManipulatorToPosition(const std::vector<dReal>& angles, bool withGripper) {
	if (withGripper) {
		this->setActiveDofsWithGripper();
	} else {
		this->setActiveDofsWithoutGripper();
	}
	OpenRaveGuard guard(this->environment->GetMutex());
	this->manipulator->SetActiveDOFValues(angles, false);
	guard.release();
}

void OpenRave::moveManipulatorToPosition(TrajectoryBaseConstPtr trajectory, bool withGripper) {
	if (withGripper) {
		this->setActiveDofsWithGripper();
	} else {
		this->setActiveDofsWithoutGripper();
	}
	OpenRaveGuard guard(this->environment->GetMutex());
	if (COMP->ini.OpenRave.showCompleteTrajectory) {
		for (unsigned int i = 0; i < trajectory->GetPoints().size(); ++i) {
			this->manipulator->SetActiveDOFValues(trajectory->GetPoints()[i].q, false);
			guard.release();
			usleep(50000);
			guard.acquire();
		}
	} else {
		this->manipulator->SetActiveDOFValues(trajectory->GetPoints()[trajectory->GetPoints().size() - 1].q, false);
	}
	guard.release();
}

void OpenRave::openGripper() {
	std::cout << "OpenRave::openGripper()\n";
	this->setActiveDofsWithGripper();
	std::vector<double> dofvalues;
	OpenRaveGuard guard(this->environment->GetMutex());
	this->manipulator->GetActiveDOFValues(dofvalues);
	if (dofvalues.size() < 6) {
		ErrorHandler::handleMessage("Gripper could not be opened. [openGripper in OpenRave]", ErrorHandler::WARNING);
		return;
	}
	dofvalues[5] = this->specificManipulator->getOpenGripperAngleOpenRave();
	this->manipulator->SetActiveDOFValues(dofvalues, true);
	guard.release();
}

void OpenRave::closeGripper() {
	std::cout << "OpenRave::closeGripper()\n";
	this->setActiveDofsWithGripper();
	std::vector<double> dofvalues;
	OpenRaveGuard guard(this->environment->GetMutex());
	this->manipulator->GetActiveDOFValues(dofvalues);
	if (dofvalues.size() < 6) {
		ErrorHandler::handleMessage("Gripper could not be closed. [closeGripper in OpenRave]", ErrorHandler::WARNING);
		return;
	}
	dofvalues[5] = this->specificManipulator->getClosedGripperAngleOpenRave();
	this->manipulator->SetActiveDOFValues(dofvalues, true);
	guard.release();
}

void OpenRave::startGUI() {
	this->environment->AttachViewer(RaveCreateViewer(this->environment, "qtcoin"));
	this->environment->GetViewer()->main(true);
}

bool OpenRave::calculateIKSolution(const dReal& x, const dReal& y, const dReal& z, const dReal& azimuth, const dReal& elevation,
		const dReal& roll, std::vector<dReal>& solution) {

	arma::mat rotmatrix(4, 4);
	//	EulerTransformationMatrices::create_zyx_matrix(0, 0, 0, -M_PI_2, -M_PI_2, 0, rotmatrix);
	//	EulerTransformationMatrices::create_zyx_matrix(0, 0, 0, 0, 0, M_PI_2, rotmatrix);

	double zxz_phi = 0;
	double zxz_theta = 0;
	double zxz_psi = 0;
	EulerTransformationMatrices::zyx_to_zxz_angles(azimuth, elevation, roll, zxz_phi, zxz_theta, zxz_psi);

	arma::mat matrix(4, 4);

	EulerTransformationMatrices::create_zxz_matrix(x, y, z, zxz_phi, zxz_theta, zxz_psi - M_PI_2, matrix);

	//	matrix = matrix * rotmatrix;

	TransformMatrix transform;
	std::vector<std::vector<dReal> > solutions;

	this->copy4x4MatrixToTransformMatrix(matrix, transform);

	IkParameterization ikPara;
	ikPara.SetTransform(transform);

	if (this->manipulator->GetActiveManipulator()->FindIKSolutions(ikPara, solutions, OpenRAVE::IKFO_CheckEnvCollisions)) {
		solution = solutions.front();
		return true;
	}
	return false;
}

bool OpenRave::iterateToGetGraspingIKSolution(const dReal& x, const dReal& y, const dReal& z, std::vector<dReal>& solution) {
	double x_offset = 0;
	double y_offset = 0;
	double z_offset = 0;
	this->specificManipulator->getOffsetRealManipulatortoOpenRaveManipulator(x_offset, y_offset, z_offset);

	dReal azimuth = 0;
	dReal elevation = 0;
	dReal roll = 0;
	dReal default_calculation = 180 / M_PI * M_PI_2;

	double v = sqrt(x * x + y * y);
	double vnew = v;
	double new_x = 0;
	double new_y = 0;

	OpenRaveGuard guard(this->environment->GetMutex());
	/**
	 * The outer loop iterates over the depth.
	 * 		That means that the TCP of the manipulator is moved closer or farther to the object.
	 * 		The input is a lower and a upper bound. In the default case the iteration is going from lower to upper
	 * 		which means that it is approaching the object from far to near.
	 * 		If the lower bound is specified bigger than the upper bound, the iteration is going from near to far.
	 * The middle loop iterates over the height.
	 * 		That means that the TCP of the manipulator is set to a different grasping height according to the object.
	 * 		Normally the height is in the middle of the object, with this it can be varied.
	 * 		In the default case the iteration is from lower to upper bound
	 * 		which means that the iteration is from under the middle of the object to above the object.
	 * 		If the lower bound is specified bigger than the upper bound, the iteration is going from top to bottom.
	 * The inner loop iterates over the angle.
	 * 		That means that the elevation angle is varied. Normally the grasping is performed with 90°.
	 * 		In the default case the iteration is from lower to upper bound which means that the manipulator is lifted to over 90°.
	 * 		If the lower bound is higher than the upper bound the manipulator will start at the top and iterates to the bottom.
	 */
	double iter_depth = localParameters.grasping_simple_lowerDepthBound;
	bool depthExpression = false;
	do {
		vnew = v + iter_depth;

		new_x = (x / v) * vnew;
		new_y = (y / v) * vnew;

		azimuth = atan((new_y + y_offset) / (new_x + x_offset)) + M_PI_2;

		double iter_height = 0;
		bool heightExpression = false;
		iter_height = z + localParameters.grasping_simple_lowerHeightBound;
		do {
			double iter_angle = localParameters.grasping_simple_lowerAngleBound;
			bool angleExpression = false;
			do {
				// Calculate new angle for grasping starting from 90° (gripper is horizontal) to 180° (gripper is vertical)
				elevation = M_PI / 180 * ((default_calculation) + iter_angle);

				std::cout << "Iteration: " << iter_depth << " " << iter_height << " " << iter_angle
						<< "  --- x, y, z, azimuth, roll, elevation :  " << new_x << "  " << new_y << "  " << iter_height << "  "
						<< azimuth << "  " << roll << "  " << elevation << std::endl;

				// Roll has to be in the middle, don't know why but it works
				if (calculateIKSolution(new_x, new_y, iter_height, azimuth, roll, elevation, solution)) {
					std::cout << "iterateToGetGraspingIKSolution found IK solution\n";
					guard.release();
					return true;
				}
				if (localParameters.grasping_simple_lowerAngleBound < localParameters.grasping_simple_upperAngleBound) {
					iter_angle += 1.0;
					angleExpression = iter_angle < localParameters.grasping_simple_upperAngleBound ? true : false;
				} else {
					iter_angle -= 1.0;
					angleExpression = iter_angle > localParameters.grasping_simple_upperAngleBound ? true : false;
				}
			} while (angleExpression);
			if (localParameters.grasping_simple_lowerHeightBound < localParameters.grasping_simple_upperHeightBound) {
				iter_height += 0.005;
				heightExpression = iter_height < z + localParameters.grasping_simple_upperHeightBound ? true : false;
			} else {
				iter_height -= 0.005;
				heightExpression = iter_height > z + localParameters.grasping_simple_upperHeightBound ? true : false;
			}
		} while (heightExpression);
		if (localParameters.grasping_simple_lowerDepthBound < localParameters.grasping_simple_upperDepthBound) {
			iter_depth += 0.002;
			depthExpression = iter_depth < localParameters.grasping_simple_upperDepthBound ? true : false;
		} else {
			iter_depth -= 0.002;
			depthExpression = iter_depth > localParameters.grasping_simple_upperDepthBound ? true : false;
		}
	} while (depthExpression);
	return false;
}

bool OpenRave::planPath(const std::vector<dReal>& angles, TrajectoryBasePtr& trajectory) {
	PlannerBase::PlannerParametersPtr params(new PlannerBase::PlannerParameters);

	this->setActiveDofsWithoutGripper();

	OpenRaveGuard guard(this->environment->GetMutex());
	params->SetRobotActiveJoints(this->manipulator);
	this->manipulator->GetActiveDOFValues(params->vinitialconfig);
	params->vgoalconfig = angles;
	guard.release();

	ErrorHandler::handleMessage("Creating Trajectory.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	trajectory = RaveCreateTrajectory(this->environment, this->manipulator->GetActiveDOF());

	ErrorHandler::handleMessage("Initialized Planner.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	this->planner->InitPlan(this->manipulator, params);

	ErrorHandler::handleMessage("Planning path.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	bool success = this->planner->PlanPath(trajectory);

	if (COMP->ini.OpenRave.saveTrajectoryToFile) {
		ErrorHandler::handleMessage("Writing Trajectory.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		std::ofstream output;
		output.open("./trajectoryOpenRave.txt", std::ofstream::trunc);
		trajectory->Write(output, Trajectory::TO_IncludeTimestamps);
		output.close();

	}
	ErrorHandler::handleMessage("Plan Path Finished.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);

	return success;
}

//////////////////////////////////////////////////
//
//	Begin Test methods
//
//////////////////////////////////////////////////

void OpenRave::testConstraint() {

	//	this->problem = this->environment->CreateProblem("BaseManipulation");
	//	this->problem->main("Katana6M180");

	// Move to position we want to plan
	std::vector<dReal> manipulatorAngles;
	manipulatorAngles.push_back(-2.05048);
	manipulatorAngles.push_back(-0.334874);
	manipulatorAngles.push_back(-1.44076);
	manipulatorAngles.push_back(-3.45412e-06);
	manipulatorAngles.push_back(-0.639737);

	this->moveManipulatorToPosition(manipulatorAngles, false);

	manipulatorAngles.clear();

	OpenRaveGuard guard1(this->environment->GetMutex());
	TransformMatrix matrix = this->manipulator->GetActiveManipulator()->GetEndEffectorTransform();
	guard1.release();

	// Move back to initial position
	manipulatorAngles.push_back(M_PI - 3.13371);
	manipulatorAngles.push_back(M_PI_2 - 2.10242);
	manipulatorAngles.push_back(M_PI - 0.99697);
	manipulatorAngles.push_back(M_PI - 1.231);
	manipulatorAngles.push_back(M_PI - 0.271071);

	this->moveManipulatorToPosition(manipulatorAngles, false);

	std::stringstream output;
	std::stringstream input;
	int maxiter = 10000;
	int maxtries = 5;
	int seedik = 3;
	// x, y ,z and azimuth, elevation, roll
	boost::array<double, 6> constraintfreedoms = { { 1, 1, 1, 1, 1, 1 } };
	TransformMatrix constraintmatrix;
	constraintmatrix.identity();
	double constrainterrorthresh = 0.92;

	matrix.trans.x += (RaveRandomInt() - 0.5) * (1.0 - constraintfreedoms[3]);
	matrix.trans.y += (RaveRandomInt() - 0.5) * (1.0 - constraintfreedoms[4]);
	matrix.trans.z += (RaveRandomInt() - 0.5) * (1.0 - constraintfreedoms[5]);

	input << "MoveToHandPosition";
	input << " matrix " << matrix;
	input << " maxiter " << maxiter;
	input << " maxtries " << maxtries;
	input << " seedik " << seedik;
	input << " constraintfreedoms ";
	for (unsigned int i = 0; i < constraintfreedoms.size(); ++i) {
		input << constraintfreedoms[i] << " ";
	}
	input << " constraintmatrix " << constraintmatrix;
	input << " constrainterrorthresh " << constrainterrorthresh;

	std::cout << input.str() << std::endl;

	OpenRaveGuard guard2(this->environment->GetMutex());
	this->problem->SendCommand(output, input);
	guard2.release();

	std::cout << output.str() << std::endl;
}
void OpenRave::testGrasping() {

}
void OpenRave::testManipulationGraspPlanning() {
	//	this->problem = this->environment->CreateProblem("TaskManipulation");
	//	this->problem->main("Katana6M180");
	//	std::cout << this->problem->GetDescription() << std::endl;

	this->problem = this->environment->CreateProblem("Grasper");
	this->problem->main("Katana6M180");
	std::cout << this->problem->GetDescription() << std::endl;

	std::stringstream output;
	std::stringstream input;

	input << "Grasp ";
	input << "target " << "1";

	this->problem->SendCommand(output, input);
	std::cout << output.str() << std::endl;

	input.str("");

	input << "Grasp ";
	input << "target " << "1";

	this->problem->SendCommand(output, input);
	std::cout << output.str() << std::endl;

	//	this->planner = this->environment->CreatePlanner("GrasperPlanner");
	//
	//	PlannerBase::PlannerParametersPtr params(new PlannerBase::PlannerParameters);
	//	OpenRaveGuard guard(this->environment->GetMutex());
	//	params->SetRobotActiveJoints(this->manipulator);
	//	this->manipulator->GetActiveDOFValues(params->vinitialconfig);
	//	params->vgoalconfig = angles;
	//
	//	ErrorHandler::handleMessage("Creating Trajectory..\n", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	//	trajectory = this->environment->CreateTrajectory(this->manipulator->GetActiveDOF());
	//
	//	ErrorHandler::handleMessage("Init Planner..\n", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	//	this->planner->InitPlan(this->manipulator, params);
	//
	//	ErrorHandler::handleMessage("Plan path..\n", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	//	bool success = this->planner->PlanPath(trajectory);
	//	guard.release();
	//
	//	if (COMP->ini.OpenRave.saveTrajectoryToFile) {
	//		ErrorHandler::handleMessage("Write Trajectory ...\n", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	//		std::ofstream output;
	//		output.open("./trajectoryOpenRave.txt", std::ofstream::trunc);
	//		trajectory->Write(output, Trajectory::TO_IncludeTimestamps);
	//		output.close();
	//
	//	}
	//	ErrorHandler::handleMessage("Plan Path Finished.\n", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	//
	//	return success;

}

void OpenRave::testPython() {
	//	try {
	//		Py_Initialize();
	//
	//		object main_module((handle<> (borrowed(PyImport_AddModule("__main__")))));
	//
	//		object main_namespace = main_module.attr("__dict__");
	//
	//		handle<> ignored((PyRun_String("print \"Hello, World\"", Py_file_input, main_namespace.ptr(), main_namespace.ptr())));
	//	} catch (error_already_set ) {
	//		PyErr_Print();
	//	}

	std::string name("1");
	KinBodyPtr body = this->environment->GetKinBody(name);
	std::cout << "Bodyname: " << body->GetName() << std::endl;

	int graspingnoise = 0;
	string cmdgen = boost::str(boost::format(
			"openrave.py --database grasping --robot=%s --target=%s --graspingnoise=%s --noviewer >> /dev/null 2>&1")
			% this->manipulator->GetXMLFilename() % "$SMART_ROOT/OpenRaveObject_1.kinbody.xml" % graspingnoise);
	std::string filename;
	std::cout << "Opening pipe ....\n";
	FILE * pipe;
	try {
		pipe = popen(cmdgen.c_str(), "r");
		std::cout << "Pipe open.\n";
		//		{
		//			boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source> fpstream(fileno(pipe));
		//			std::istream in(&fpstream);
		//			std::getline(in, filename);
		//			if (!in) {
		//				pclose(pipe);
		//				return;
		//			}
		//		}
		std::cout << "Pipe closing.\n";
		int tmp = pclose(pipe);
		std::cout << "Return pClose: " << tmp << std::endl;
		std::cout << "Pipe closed:\n";
	} catch (...) {
		std::cout << "Catched something.\n";
	}

	//	int generateexit = pclose(pipe);
}

//////////////////////////////////////////////////
//
//	End Test methods
//
//////////////////////////////////////////////////


//////////////////////////////////////////////////
//
//	private methods
//
//////////////////////////////////////////////////

OpenRave::OpenRave() {
	this->specificManipulator = MANIPULATORFACTORY->createManipulatorClass(COMP->ini.OpenRave.robotName);
	this->globalParameters.clearEnvironment = false;
	this->globalParameters.computeGraspTable = false;
	this->globalParameters.computeGraspTable_id = 0;
	this->globalParameters.grasping_advanced = false;
	this->globalParameters.grasping_none = true;
	this->globalParameters.grasping_simple = false;
	this->globalParameters.grasping_simple_lowerHeightBound = 0.0;
	this->globalParameters.grasping_simple_upperHeightBound = 0.0;
	this->globalParameters.grasping_simple_lowerDepthBound = 0.0;
	this->globalParameters.grasping_simple_upperDepthBound = 0.0;
	this->globalParameters.grasping_simple_lowerAngleBound = 0.0;
	this->globalParameters.grasping_simple_upperAngleBound = 0.0;
	this->globalParameters.loadEnvironmentObjectRecognition = false;
	this->globalParameters.loadEnvironmentObjectRecognition_id = 0;
	this->globalParameters.openrave_GraspObject = false;
	this->globalParameters.openrave_GraspObject_id = 0;
	this->globalParameters.openrave_ReleaseObject = false;
	this->globalParameters.openrave_ReleaseObject_id = 0;
	this->globalParameters.parallelization = false;
	this->globalParameters.simulation_test_ik_only = false;
	this->globalParameters.modified = false;
	this->localParameters = this->globalParameters;
	this->parallelization_initial_sync = false;

	this->initial_sync = false;
}

void OpenRave::loadEnvironment(unsigned int envId) {
	CommObjectRecognitionObjects::CommObjectRecognitionId request;
	request.set_id(envId);
	CommObjectRecognitionObjects::CommObjectRecognitionEnvironment response;
	COMP->environmentQueryClient->query(request, response);
	double x = 0;
	double y = 0;
	double z = 0;
	ObjectXMLWriter writer;
	// TODO: Database inizializieren
	ObjectDatabase database("");

	for (uint32_t i = 0; i < response.get_size(); i++) {
		CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties object = response.get_object_properties(i);

		std::string output;

		std::string type;
		object.get_type(type);

		CommBasicObjects::CommPose3d pose = object.get_pose();
		// Move the table a little bit down to prevent collisions with objects
		if (type == "table") {
			pose.set_z(pose.get_z(1) - 0.0075, 1);
		}

		std::stringstream stream;
		stream << object.get_id();

		if (database.isKnownObject(type)) {
			if (database.getObjectShape(type) == ObjectDatabase::BOUNDINGBOX) {
				database.getObjectBoundingBox(x, y, z);
				writer.writeKinBodyBox(stream.str(), x, y, z, output, COMP->ini.OpenRave.saveObjectsToFile);
			} else if (database.getObjectShape(type) == ObjectDatabase::CYLINDER) {
				database.getObjectCylinder(x, y);
				writer.writeKinBodyCylinder(stream.str(), x, y, output, COMP->ini.OpenRave.saveObjectsToFile);
				pose.set_roll(pose.get_roll() + M_PI_2);
			} else if (database.getObjectShape(type) == ObjectDatabase::MESH) {
				std::string filename;
				database.getObjectMeshFilename(filename);
				writer.writeKinBodyMesh(stream.str(), filename, output, COMP->ini.OpenRave.saveObjectsToFile);
				pose.set_roll(pose.get_roll() + M_PI_2);
			}
		} else {
			object.get_dimension(x, y, z, 1);
			writer.writeKinBodyBox(stream.str(), x, y, z, output, COMP->ini.OpenRave.saveObjectsToFile);
		}
		this->addKinBody(output, pose.getHomogeneousMatrix(1));
	}
	ErrorHandler::handleMessage("Environment load was successful. [loadEnvironment in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::loadEnvironmentFromFile(unsigned int envId) {
	std::stringstream stream;
	stream << COMP->ini.OpenRave.storedEnvironmentPath;
	stream << COMP->ini.OpenRave.storedEnvironmentName;
	stream << envId;
	stream << ".xml";
	this->environment->Load(stream.str());

	ErrorHandler::handleMessage("File environment load was successful File: " + stream.str()
			+ " [loadEnvironmentToFile in OpenRave]", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::saveEnvironmentToFile(unsigned int envId) {
	std::stringstream stream;
	stream << COMP->ini.OpenRave.storedEnvironmentPath;
	stream << COMP->ini.OpenRave.storedEnvironmentName;
	stream << envId;
	stream << ".dae";
	if (this->environment->Save(stream.str())) {
		ErrorHandler::handleMessage("Environment could not be saved. File: " + stream.str()
				+ " [saveEnvironmentToFile in OpenRave]", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
		return;
	}
	ErrorHandler::handleMessage(
			"Environment save was successful. File: " + stream.str() + " [saveEnvironmentToFile in OpenRave]",
			ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::addKinBody(const std::string& data, const arma::mat& matrix) {
	std::list<std::pair<std::string, std::string> > atts;
	KinBodyPtr body;
	OpenRaveGuard guard(this->environment->GetMutex());
	body = this->environment->ReadKinBodyXMLData(body, data, atts);

	TransformMatrix transform;
	this->copy4x4MatrixToTransformMatrix(matrix, transform);

	body->SetTransform(transform);

	if (this->environment->AddKinBody(body)) {
		ErrorHandler::handleMessage("KinBody has been successfully added. ID: " + body->GetName() + " [addKinBody in OpenRave]",
				ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	} else {
		ErrorHandler::handleMessage("KinBody could not be added. ID: " + body->GetName() + " [addKinBody in OpenRave]",
				ErrorHandler::ERROR);
	}
	guard.release();
}

void OpenRave::deleteKinBody(unsigned int objId) {
	OpenRaveGuard guard(this->environment->GetMutex());
	std::stringstream stream;
	stream << objId;
	KinBodyPtr body = this->environment->GetKinBody(stream.str());
	if (body == NULL) {
		ErrorHandler::handleMessage("KinBody cannot be found. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				ErrorHandler::WARNING);
		guard.release();
		return;
	}
	if (this->manipulator->IsGrabbing(body)) {
		this->releaseKinBody(objId);
	}
	if (this->environment->Remove(body)) {
		ErrorHandler::handleMessage(
				"KinBody has been successfully removed. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	} else {
		ErrorHandler::handleMessage("KinBody could not be removed. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				ErrorHandler::ERROR);
	}
	guard.release();
}

void OpenRave::graspKinBody(unsigned int objId) {
	std::stringstream stream;
	stream << objId;
	KinBodyPtr body = this->environment->GetKinBody(stream.str());

	std::vector<int> gripperIndices = this->manipulator->GetActiveManipulator()->GetGripperIndices();
	std::set<int> indicesToIgnore;
	for (size_t i = 0; i < gripperIndices.size(); ++i) {
		indicesToIgnore.insert(gripperIndices[i]);
	}
	if (!this->manipulator->Grab(body, this->manipulator->GetActiveManipulator()->GetEndEffector(), indicesToIgnore)) {
		//if (!this->manipulator->Grab(body)) {
		ErrorHandler::handleMessage("Object could not be grasped. [graspKinBody in OpenRave]", ErrorHandler::ERROR);
		return;
	}
	ErrorHandler::handleMessage("Object has been successfully grasped. [graspKinBody in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::releaseKinBody(unsigned int objId) {
	std::stringstream stream;
	stream << objId;
	KinBodyPtr body = this->environment->GetKinBody(stream.str());

	this->manipulator->Release(body);
	ErrorHandler::handleMessage("Object has been successfully released. [graspKinBody in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::computeGraspTable(unsigned int objectId) {
	CommObjectRecognitionObjects::CommObjectRecognitionId request;
	request.set_id(objectId);
	CommObjectRecognitionObjects::CommObjectRecognitionObjectProperties response;

	CHS::StatusCode status = COMP->objectQueryClient->query(request, response);

	if (status != CHS::SMART_OK) {
		ErrorHandler::handleMessage("Object could not be get from object recognition. [computeGraspTable in OpenRave]",
				ErrorHandler::WARNING);
		return;
	}

	std::string name;
	double x = 0;
	double y = 0;
	double z = 0;
	ObjectXMLWriter writer;

	response.get_dimension(x, y, z, 1);
	response.get_type(name);

	std::string output;
	writer.writeKinBodyBox(name, x, y, z, output, true);

	name.clear();
	name.append("$SMART_ROOT/");
	name.append(writer.getLastFileName());

	std::cout << "Name: " << name << std::endl;

	// Test if grasp tables already computed for this object.
	string cmdgen = boost::str(boost::format(
			"openrave.py --database grasping --robot=%s --target=%s --show --noviewer >> /dev/null 2>&1")
			% this->manipulator->GetXMLFilename() % name);
	ErrorHandler::handleMessage("Testing if grasp tables already computed. [computeGraspTable in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
	FILE * pipe = popen(cmdgen.c_str(), "r");
	int ret = pclose(pipe);
	if (ret == 0) {
		ErrorHandler::handleMessage("Grasp tables are already computed. [computeGraspTable in OpenRave]", ErrorHandler::INFO,
				COMP->ini.OpenRave.debugOpenRave);
		return;
	}

	// Compute grasp tables
	int graspingnoise = 0;

	cmdgen = boost::str(boost::format(
			"openrave.py --database grasping --robot=%s --target=%s --graspingnoise=%s --noviewer >> /dev/null 2>&1")
			% this->manipulator->GetXMLFilename() % name % graspingnoise);

	ErrorHandler::handleMessage("Start computing Grasp Table with openrave.py. [computeGraspTable in OpenRave]",
			ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	pipe = popen(cmdgen.c_str(), "r");
	ErrorHandler::handleMessage("Computing...", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	ret = pclose(pipe);
	if (ret != 0) {
		std::stringstream stream;
		stream << "Computing Grasp Table failed. Error Code: ";
		stream << ret;
		stream << " [computeGraspTable in OpenRave]";
		ErrorHandler::handleMessage(stream.str(), ErrorHandler::ERROR);
		return;
	}
	ErrorHandler::handleMessage("Computing Grasp Table finished successful. [computeGraspTable in OpenRave]", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::setActiveDofsWithGripper() {
	std::vector<int> dofs;
	this->specificManipulator->getActivDofsWithGripper(dofs);
	OpenRaveGuard guard(this->environment->GetMutex());
	this->manipulator->SetActiveDOFs(dofs);
	guard.release();
}

void OpenRave::setActiveDofsWithoutGripper() {
	std::vector<int> dofs;
	this->specificManipulator->getActivDofsWithoutGripper(dofs);
	OpenRaveGuard guard(this->environment->GetMutex());
	this->manipulator->SetActiveDOFs(dofs);
	guard.release();
}

bool OpenRave::copy4x4MatrixToTransformMatrix(const arma::mat& matrix, TransformMatrix& transform) {
	if (!(matrix.n_cols == 4 && matrix.n_rows == 4)) {
		ErrorHandler::handleMessage("No 4x4 Matrix as input. [copy4x4MatrixToTransformMatrix in OpenRave]", ErrorHandler::WARNING);
		return false;
	}
	for (int i = 0; i < 12; ++i) {
		// Only copy 3x3 matrix Vector must be done separate
		if (!((i % 4) == 3)) {
			transform.m[i] = matrix.at(i / 4, i % 4);
		}
	}
	transform.trans.x = matrix.at(0, 3);
	transform.trans.y = matrix.at(1, 3);
	transform.trans.z = matrix.at(2, 3);
	transform.trans.w = matrix.at(3, 3);
	return true;
}

void OpenRave::deleteAllKinBodies(bool withManipulator, bool onlyManipulator) {
	std::vector<KinBodyPtr> bodies;
	OpenRaveGuard guard(this->environment->GetMutex());
	this->environment->GetBodies(bodies);
	for (unsigned int i = 0; i < bodies.size(); ++i) {
		if ((bodies[i]->IsRobot() && withManipulator == true) || (bodies[i]->IsRobot() && onlyManipulator == true)) {
			this->environment->Remove(bodies[i]);
		} else if (!bodies[i]->IsRobot() && onlyManipulator == false) {
			this->environment->Remove(bodies[i]);
		}
	}
	guard.release();
}
