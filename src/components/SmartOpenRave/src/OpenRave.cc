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
#include <string>
#include <fstream>
#include "gen/SmartOpenRave.hh"
#include "OpenRave.hh"
#include "ObjectXMLWriter.hh"

#include <commObjectRecognitionId.hh>
#include <commObjectRecognitionObjectFeatures.hh>

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
		COMP->OpenRaveMutex.acquire();
		if (_instance == NULL) {
			_instance = new OpenRave();
		}
		COMP->OpenRaveMutex.release();
	}
	return _instance;
}

OpenRave::~OpenRave() {
	this->environment->Reset();
}

void OpenRave::init() {
	this->environment = CreateEnvironment(true);

	this->environment->GetMutex().lock();
	// Load Default Environment with ground
	this->environment->Load(COMP->ini.OpenRave.defaultEnvironmentPath);
	// Load robot arm
	this->manipulator = this->environment->ReadRobotXMLFile(COMP->ini.OpenRave.robotPath);
	// Add Robot to the environment
	this->environment->AddRobot(this->manipulator);
	this->environment->GetMutex().unlock();

	this->planner = this->environment->CreatePlanner("BiRRT");
	this->problem = this->environment->CreateProblem("BaseManipulation");
	this->problem->main("Katana6M180");

	ErrorHandler::handleMessage("Initialize of OpenRave done.");
}

void OpenRave::resetAll() {
	// Delete all KinBodies except the manipulator
	this->deleteAllKinBodies(false, false);
	this->environment->GetMutex().lock();
	this->environment->Load(COMP->ini.OpenRave.defaultEnvironmentPath);
	this->environment->GetMutex().unlock();
}

void OpenRave::unlockEnvironment() {
	if (this->environment->GetMutex().try_lock()) {
		this->environment->GetMutex().unlock();
		return;
	}
	this->environment->GetMutex().unlock();
}

void OpenRave::syncManipulator() {
	// If the manipulator is attached, we work with it, otherwise we have a default behavior.
	if (COMP->ini.PortParameter.withManipulator) {
		COMP->manipulatorStateClient->setWaitState("trajectory");
		Smart::CommMobileManipulatorState manipulatorState;
		do {
			CHS::StatusCode status = COMP->manipulatorStatePushTimedClient->getUpdateWait(manipulatorState);

			if (status != CHS::SMART_OK) {
				ErrorHandler::handleMessage("Timed update of ManipulatorState could not be get. [syncManipulator in OpenRave]",
						Smart::MANIPULATOR_SYNC_FAIL, ErrorHandler::ERROR);
				return;
			}
		} while (!manipulatorState.is_valid());

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

		this->environment->GetMutex().lock();
		this->manipulator->SetTransform(transform);
		this->environment->GetMutex().unlock();

		std::vector<dReal> manipulatorAngles;
		// Get all Angles from the real Manipulator inclusive the gripper
		for (u_int32_t i = 0; i < manipulatorState.get_manipulator_state().get_joint_count(); ++i) {
			manipulatorAngles.push_back(manipulatorState.get_manipulator_state().get_joint_angle(i));
		}
		manipulatorAngles.push_back(manipulatorState.get_manipulator_state().get_gripper_angle());

		this->specificManipulator->convertRealAnglesToOpenRaveAngles(manipulatorAngles, manipulatorAngles);

		// Move the robot arm in OpenRave to the same position of real robot arm
		this->moveManipulatorToPosition(manipulatorAngles, true);
	} else {
		TransformMatrix transform;
		transform.identity();
		double offset_X = 0;
		double offset_Y = 0;
		double offset_Z = 0;
		specificManipulator->getOffsetRealManipulatortoOpenRaveManipulator(offset_X, offset_Y, offset_Z);
		transform.trans.x -= offset_X;
		transform.trans.y -= offset_Y;
		transform.trans.z = 0.590 - offset_Z;
		this->environment->GetMutex().lock();
		this->manipulator->SetTransform(transform);
		this->environment->GetMutex().unlock();

		std::vector<dReal> manipulatorAngles;
		// Calibrate Position
		manipulatorAngles.push_back(M_PI - 3.13371);
		manipulatorAngles.push_back(M_PI_2 - 2.10242);
		manipulatorAngles.push_back(M_PI - 0.99697);
		manipulatorAngles.push_back(M_PI - 1.231);
		manipulatorAngles.push_back(M_PI - 0.271071);

		this->moveManipulatorToPosition(manipulatorAngles, false);
	}
}

void OpenRave::setParameter(const Smart::CommManipulationPlannerParameter& param) {
	Smart::ManipulationPlannerParameterMode tag;
	int tmp = 0;

	param.get(tag, tmp);

	switch (tag) {
	case Smart::CLEARENV: {
		this->resetAll();
		break;
	}

	case Smart::LOADENV: {
		this->resetAll();
		this->loadEnvironment(tmp);
		break;
	}

	case Smart::LOADENVFF: {
		this->loadEnvironmentFromFile(tmp);
		break;
	}

	case Smart::DELETEOBJ: {
		this->deleteKinBody(tmp);
		break;
	}

	case Smart::SAVEENVTF: {
		this->saveEnvironementToFile(tmp);
		break;
	}

	case Smart::COMPUTEGRASPTABLE: {
		this->computeGraspTable(tmp);
		break;
	}
	}
}

OpenRave::OpenRaveParameter& OpenRave::getParameter() {
	return this->parameter;
}

void OpenRave::moveManipulatorToPosition(const std::vector<dReal>& angles, bool withGripper) {
	if (withGripper) {
		this->setActiveDofsWithGripper();
	} else {
		this->setActiveDofsWithoutGripper();
	}
	this->environment->GetMutex().lock();
	this->manipulator->SetActiveDOFValues(angles, false);
	this->environment->GetMutex().unlock();
}

void OpenRave::moveManipulatorToPosition(TrajectoryBaseConstPtr trajectory, bool withGripper) {
	if (withGripper) {
		this->setActiveDofsWithGripper();
	} else {
		this->setActiveDofsWithoutGripper();
	}
	this->environment->GetMutex().lock();
	if (COMP->ini.OpenRave.showCompleteTrajectory) {
		for (unsigned int i = 0; i < trajectory->GetPoints().size(); ++i) {
			this->manipulator->SetActiveDOFValues(trajectory->GetPoints()[i].q, false);
			this->environment->GetMutex().unlock();
			usleep(50000);
			this->environment->GetMutex().lock();
		}
	} else {
		this->manipulator->SetActiveDOFValues(trajectory->GetPoints()[trajectory->GetPoints().size() - 1].q, false);
	}
	this->environment->GetMutex().unlock();
}

void OpenRave::loadEnvironment(unsigned int envId) {
	Smart::CommObjectRecognitionId request;
	request.set_id(envId);
	Smart::CommObjectRecognitionEnvironment response;
	COMP->environementQueryClient->query(request, response);
	double x = 0;
	double y = 0;
	double z = 0;
	ObjectXMLWriter writer;

	for (uint32_t i = 0; i < response.get_size(); i++) {
		Smart::CommObjectRecognitionObjectFeatures object = response.get_object_features(i);

		object.get_dimension(x, y, z, 1);
		Smart::CommPose3d pose = object.get_pose();

		std::string output;
		std::stringstream stream;
		stream << object.get_id();
		writer.writeKinBodyBox(stream.str(), x, y, z, output);

		this->addKinBody(output, pose.getHomogeneousMatrix(1));
	}
	ErrorHandler::handleMessage("Environment load was successful.", Smart::PARAMETER_SUCCESS, ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::loadEnvironmentFromFile(unsigned int envId) {
	std::stringstream stream;
	stream << COMP->ini.OpenRave.storedEnvironmentPath;
	stream << COMP->ini.OpenRave.storedEnvironmentName;
	stream << envId;
	stream << ".dae";

	// Delete all objects
	this->environment->Reset();

	// Load environment from file
	this->environment->Load(stream.str());

	this->environment->GetMutex().lock();

	// Delete only manipulator
	this->deleteAllKinBodies(false, true);
	// Load the manipulator over the correct location
	this->manipulator = this->environment->ReadRobotXMLFile(COMP->ini.OpenRave.robotPath);
	// Add manipulator to the environment
	this->environment->AddRobot(this->manipulator);
	this->environment->GetMutex().unlock();
	ErrorHandler::handleMessage("File environment load was successful.", Smart::PARAMETER_SUCCESS, ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::saveEnvironementToFile(unsigned int envId) {
	std::stringstream stream;
	stream << COMP->ini.OpenRave.storedEnvironmentPath;
	stream << COMP->ini.OpenRave.storedEnvironmentName;
	stream << envId;
	stream << ".dae";
	this->environment->Save(stream.str());
	ErrorHandler::handleMessage("Environment save was successful.", Smart::PARAMETER_SUCCESS, ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
}

void OpenRave::addKinBody(const std::string& data, const arma::mat& matrix) {
	std::list<std::pair<std::string, std::string> > atts;
	KinBodyPtr body;
	this->environment->GetMutex().lock();
	body = this->environment->ReadKinBodyXMLData(body, data, atts);

	TransformMatrix transform;
	this->copy4x4MatrixToTransformMatrix(matrix, transform);

	body->SetTransform(transform);

	this->environment->AddKinBody(body);
	this->environment->GetMutex().unlock();
}

void OpenRave::deleteKinBody(unsigned int objId) {
	this->environment->GetMutex().lock();
	std::stringstream stream;
	stream << objId;
	KinBodyPtr body = this->environment->GetKinBody(stream.str());
	if (body == NULL) {
		ErrorHandler::handleMessage("KinBody cannot be found. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				ErrorHandler::WARNING);
		this->environment->GetMutex().unlock();
		return;
	}
	if (this->environment->RemoveKinBody(body)) {
		ErrorHandler::handleMessage(
				"KinBody has been successfully removed. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	} else {
		ErrorHandler::handleMessage("KinBody could not be removed. ID: " + stream.str() + " [deleteKinBody in OpenRave]",
				Smart::PARAMETER_FAIL, ErrorHandler::WARNING);
	}
	this->environment->GetMutex().unlock();
}

void OpenRave::startGUI() {
	this->environment->AttachViewer(this->environment->CreateViewer("qtcoin"));
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
	// Find IK Solution and take always the first
	if (this->manipulator->GetActiveManipulator()->FindIKSolutions(ikPara, solutions, true)) {
		solution = solutions.front();
		return true;
	}
	return false;
}

bool OpenRave::planPath(const std::vector<dReal>& angles, TrajectoryBasePtr& trajectory) {
	PlannerBase::PlannerParametersPtr params(new PlannerBase::PlannerParameters);

	this->setActiveDofsWithoutGripper();

	this->environment->GetMutex().lock();
	params->SetRobotActiveJoints(this->manipulator);
	this->manipulator->GetActiveDOFValues(params->vinitialconfig);
	params->vgoalconfig = angles;

	ErrorHandler::handleMessage("Creating Trajectory.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	trajectory = this->environment->CreateTrajectory(this->manipulator->GetActiveDOF());

	ErrorHandler::handleMessage("Initialized Planner.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	this->planner->InitPlan(this->manipulator, params);

	ErrorHandler::handleMessage("Planning path.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	bool success = this->planner->PlanPath(trajectory);
	this->environment->GetMutex().unlock();

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

void OpenRave::getRobotTransform(TransformMatrix& transform) {
	transform = this->manipulator->GetTransform();
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

	this->environment->GetMutex().lock();
	TransformMatrix matrix = this->manipulator->GetActiveManipulator()->GetEndEffectorTransform();
	this->environment->GetMutex().unlock();

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

	this->environment->GetMutex().lock();
	this->problem->SendCommand(output, input);
	this->environment->GetMutex().unlock();

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
	//	this->environment->GetMutex().lock();
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
	//	this->environment->GetMutex().unlock();
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
}

void OpenRave::computeGraspTable(unsigned int objectId) {
	Smart::CommObjectRecognitionId request;
	request.set_id(objectId);
	Smart::CommObjectRecognitionObjectFeatures response;

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

	// Test if grasp tables already computed for this object.
	string cmdgen = boost::str(boost::format("openrave.py --database grasping --robot=%s --target=%s --show --noviewer >> /dev/null 2>&1")
			% this->manipulator->GetXMLFilename() % name);
	ErrorHandler::handleMessage("Testing if grasp tables already computed.", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	FILE * pipe = popen(cmdgen.c_str(), "r");
	int ret = pclose(pipe);
	if (ret == 0) {
		ErrorHandler::handleMessage("Grasp tables are already computed.", Smart::PARAMETER_SUCCESS, ErrorHandler::INFO,
				COMP->ini.OpenRave.debugOpenRave);
		return;
	}

	// Compute grasp tables
	int graspingnoise = 0;

	cmdgen = boost::str(boost::format(
			"openrave.py --database grasping --robot=%s --target=%s --graspingnoise=%s --noviewer >> /dev/null 2>&1")
			% this->manipulator->GetXMLFilename() % name % graspingnoise);

	ErrorHandler::handleMessage("Start computing Grasp Table with openrave.py.", ErrorHandler::INFO,
			COMP->ini.OpenRave.debugOpenRave);
	pipe = popen(cmdgen.c_str(), "r");
	ErrorHandler::handleMessage("Computing...", ErrorHandler::INFO, COMP->ini.OpenRave.debugOpenRave);
	ret = pclose(pipe);
	if (ret == 0) {
		ErrorHandler::handleMessage("Computing Grasp Table finished successful.", Smart::PARAMETER_SUCCESS, ErrorHandler::INFO,
				COMP->ini.OpenRave.debugOpenRave);
	} else {
		std::stringstream stream;
		stream << "Computing Grasp Table failed. Error Code: ";
		stream << ret;
		ErrorHandler::handleMessage(stream.str(), Smart::PARAMETER_FAIL, ErrorHandler::ERROR);
	}
}

void OpenRave::setActiveDofsWithGripper() {
	std::vector<int> dofs;
	this->specificManipulator->getActivDofsWithGripper(dofs);
	this->environment->GetMutex().lock();
	this->manipulator->SetActiveDOFs(dofs);
	this->environment->GetMutex().unlock();
}

void OpenRave::setActiveDofsWithoutGripper() {
	std::vector<int> dofs;
	this->specificManipulator->getActivDofsWithoutGripper(dofs);
	this->environment->GetMutex().lock();
	this->manipulator->SetActiveDOFs(dofs);
	this->environment->GetMutex().unlock();
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
	this->environment->GetMutex().lock();
	this->environment->GetBodies(bodies);
	for (unsigned int i = 0; i < bodies.size(); ++i) {
		if ((bodies[i]->IsRobot() && withManipulator == true) || (bodies[i]->IsRobot() && onlyManipulator == true)) {
			this->environment->RemoveKinBody(bodies[i]);
		} else if (!bodies[i]->IsRobot() && onlyManipulator == false) {
			this->environment->RemoveKinBody(bodies[i]);
		}
	}
	this->environment->GetMutex().unlock();
}
