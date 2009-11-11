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

#include "path_planning/OpenRave.hh"
#include "misc/Sem.hh"
#include "misc/Defines.hh"
#include <list>
#include <time.h>
#include <sstream>

using std::list;
using OpenRAVE::KinBody;

namespace mmp
{
	const string OpenRave::KATANA = "robots/katana.robot.xml";
	const string OpenRave::DEFAULTENV = "/home/wopfner/SOFTWARE/smartsoft/src/workspace_mdsd/MobileManipulationServer/src/data/default.env.xml";
	const int OpenRave::ITERATIONS = 90;

	OpenRave::OpenRave()
	{

	}

	OpenRave::~OpenRave()
	{
		this->resetAll();
		delete this->planner;
		delete this->problem;
		delete this->environment;
		delete this->pluginInfo;
	}

	/*************************************************

	 Public Methods

	 *************************************************/

	void OpenRave::run()
	{
		this->init();
		this->buildPluginList();
	}

	void OpenRave::startGUI()
	{
		string tmp = this->environment->GetViewer()->GetPluginName();
		if (tmp.empty())
		{
			const vector<const wchar_t*>* viewers = this->getPluginNames(OpenRAVE::PT_Viewer);
			this->environment->AttachViewer(this->environment->CreateViewer(viewers->front()));
		}
		this->environment->GetViewer()->main(true);
	}

	void OpenRave::stopGUI()
	{
		this->environment->GetViewer()->quitmainloop();
	}

	void OpenRave::resetAll()
	{
		this->objects->clear();
		this->environment->LockPhysics(true);
		vector<KinBody*> bodies;
		this->environment->GetBodies(bodies);
		// Delete all Objekts except the Robot
		for (unsigned int i = 0; i < bodies.size(); ++i)
		{
			if (!bodies[i]->IsRobot())
			{
				this->environment->RemoveKinBody(bodies[i], true);
			}
		}
//		this->environment->GetBodies(bodies);
		this->environment->Load(this->DEFAULTENV.c_str());
		this->environment->LockPhysics(false);
	}

	void OpenRave::deleteObject(KinBody* body)
	{
		this->environment->LockPhysics(true);
		this->environment->RemoveKinBody(body, true);
		this->environment->LockPhysics(false);
	}

	void OpenRave::readKinBody(const string& path, const string& type, const double matrix[], bool obstacle, bool import)
	{
		if (!import)
		{
			this->resetAll();
		}
		this->environment->LockPhysics(true);
		std::cout << path << " | " << type << std::endl;
		KinBody* body = this->environment->ReadKinBodyXML(NULL, path.c_str(), NULL);
		std::cout << "Read" << std::endl;
		// Add body to the grabable list if its not a obstacle
		if (!obstacle)
		{
			pair<string, KinBody*> tmp(type, body);
			this->objects->push_back(tmp);
		}
		// Transformation of the Body
		RaveTransformMatrix<dReal> transformMatrix;
		// Set the Rotation Matrix its 3x3 but its iternal 3x4
		for (int i = 1; i <= 12; ++i)
		{
			if (i % 4 != 0)
			{
				transformMatrix.m[i - 1] = (float) matrix[i - 1];
			}
		}
		// Set the Translation Vector
		transformMatrix.trans.x = (float) matrix[3];
		transformMatrix.trans.y = (float) matrix[7];
		transformMatrix.trans.z = (float) matrix[11];
		transformMatrix.trans.w = (float) matrix[15];

		body->SetTransform(transformMatrix);
		this->environment->AddKinBody(body);
		this->environment->LockPhysics(false);
	}

	void OpenRave::readEnv(const string& path, bool import)
	{
		if (!import)
		{
			this->resetAll();
		}
		this->environment->Load(path.c_str());
	}

	vector<pair<string, KinBody*> >* OpenRave::getAllKinBodies()
	{
		return this->objects;
	}

	vector<KinBody*>* OpenRave::getKinBodies(const string& type)
	{
		vector<KinBody*>* result = new vector<KinBody*> ();
		for (unsigned int i = 0; i < this->objects->size(); ++i)
		{
			if ((*this->objects)[i].first == type)
			{
				result->push_back((*this->objects)[i].second);
			}
		}
		return result;
	}

	KinBody* OpenRave::getKinBody(uint32_t id)
	{
		std::stringstream temp;
		temp << id;

		for (unsigned int i = 0; i < this->objects->size(); ++i)
		{
			if ((*this->objects)[i].first == temp.str())
			{
				return (*this->objects)[i].second;
			}
		}

		return NULL;
	}

	const Vector& OpenRave::getExtents(KinBody* object)
	{
		const Vector* extents;
		const vector<KinBody::Link*>& links = object->GetLinks();
		for (size_t i = 0; i < links.size(); ++i)
		{
			const list<KinBody::Link::GEOMPROPERTIES>& geoms = links[i]->GetGeometries();
			list<KinBody::Link::GEOMPROPERTIES>::const_iterator iter;
			for (iter = geoms.begin(); iter != geoms.end(); ++iter)
			{
				extents = &iter->GetBoxExtents();
			}
		}
		return *extents;
	}

	int OpenRave::getCurrentIteration()
	{
		return this->iterations;
	}

	void OpenRave::calculateIKSolution(KinBody* target, vector<dReal>& solution, int iteration, double offset)
	{
		CPose3D pose;
		TransformMatrix transform;
		vector<vector<dReal> > solutions;

		TransformMatrix bodyPosition = target->GetTransform();

		const Vector& extents = getExtents(target);

		// Vector on the x/y plane to the object
		double v = sqrt(bodyPosition.trans.x * bodyPosition.trans.x + bodyPosition.trans.y * bodyPosition.trans.y);
		// Subtract the gripper
		double vnew = v;// - extents.x;

		// Intercept therorems to get the new x, y and z values
		double xnew = (bodyPosition.trans.x / v) * vnew;
		double ynew = (bodyPosition.trans.y / v) * vnew;

		// yaw is the rotation around the Z axis of the robot, rotation of the Motor One
		double yaw = atan(ynew / xnew);
		// pitch is the rotation around the Y axis of the robot
		// variable angle which has to be ajusted for grabbing
		double pitch = M_PI_2;
		// roll is the rotation of the gripper
		double roll = 0;

		pose.x(xnew);
		pose.y(ynew);
		// Add the extents of the target Objects + a small offset
		pose.z(bodyPosition.trans.z + extents.z / 3 + offset);

		// Setting the pitch from 90° to 45° with 1° step
		// Gripper is horizontal and lies in the x/y plane with 90°
		// Gripper is the angle bisector between the x/y plane and the -z-Axis with 45°
		bool found = false;
		if (iteration != -1)
		{
			pose.setYawPitchRoll(yaw, pitch - iteration, roll);

			// Calculate new vector with angle inclution
			vnew = v; //- extents.x * cos((M_PI / 180) * iteration);
			// Intercept therorems to get the new x, y values
			pose.x((bodyPosition.trans.x / v) * vnew);
			pose.y((bodyPosition.trans.y / v) * vnew);

			// Get the Matrix from the Pose and set it into the OpenRave Matrix
			double* matrix = pose.getHomogeneousMatrixVal().m_Val;

			for (int j = 1; j < 12; ++j)
			{
				if (j % 4 == 0)
					continue;
				transform.m[j - 1] = matrix[j - 1];
			}
			transform.trans.x = matrix[3];
			transform.trans.y = matrix[7];
			transform.trans.z = matrix[11];

			// Find IK Solution
			if (this->robot->GetActiveManipulator()->FindIKSolutions(transform, solutions, true))
			{
				// Return the first solution
				solution = solutions.front();
				found = true;
			}
		}
		if (!found)
		{
			for (int i = 0; i <= OpenRave::ITERATIONS; ++i)
			{
#ifdef LOGOPENRAVE
				std::cout << "Iteration for Grasping: " << i << std::endl;
#endif

				pose.setYawPitchRoll(yaw, pitch - i, roll);

				// Calculate new vector with angle inclution
				vnew = v; //- extents.x * cos((M_PI / 180) * i);
				// Intercept therorems to get the new x, y values
				pose.x((bodyPosition.trans.x / v) * vnew);
				pose.y((bodyPosition.trans.y / v) * vnew);

				// Get the Matrix from the Pose and set it into the OpenRave Matrix
				double* matrix = pose.getHomogeneousMatrixVal().m_Val;

				for (int j = 1; j < 12; ++j)
				{
					if (j % 4 == 0)
						continue;
					transform.m[j - 1] = matrix[j - 1];
				}
				transform.trans.x = matrix[3];
				transform.trans.y = matrix[7];
				transform.trans.z = matrix[11];

				// Find IK Solution
				if (this->robot->GetActiveManipulator()->FindIKSolutions(transform, solutions, true))
				{
					// Return the first solution
					solution = solutions.front();
					this->iterations = i;
					break;
				}
			}
		}
	}

	void OpenRave::calculateIKSolution(const CPose3D& pose, vector<dReal>& solution)
	{
		std::cout << pose << std::endl;

		TransformMatrix transform;
		vector<vector<dReal> > solutions;

		// Get the Matrix from the Pose and set it into the OpenRave Matrix
		double* matrix = pose.getHomogeneousMatrixVal().m_Val;

		for (int i = 1; i < 12; ++i)
		{
			if (i % 4 == 0)
				continue;
			transform.m[i - 1] = matrix[i - 1];
		}
		transform.trans.x = matrix[3];
		transform.trans.y = matrix[7];
		transform.trans.z = matrix[11];

		// Find IK Solution
		if (this->robot->GetActiveManipulator()->FindIKSolutions(transform, solutions, false))
		{
			std::cout << solutions.size() << " solutions found" << std::endl;
			solution = solutions.front();
		}
		else
		{
			std::cout << "no solution found" << std::endl;
		}
	}

	Trajectory* OpenRave::planPath(KinBody* target, int iteration, double offset)
	{
		PlannerBase::PlannerParameters params;
		// without the third Dof because its a dummy Dof
		vector<int> dofs;
		dofs.push_back(0);
		dofs.push_back(1);
		dofs.push_back(2);
		dofs.push_back(4);
		dofs.push_back(5);
		this->environment->LockPhysics(true);
		this->robot->SetActiveDOFs(dofs);
		this->robot->GetActiveDOFValues(params.vinitialconfig);

		this->calculateIKSolution(target, params.vgoalconfig, iteration, offset);
		if (params.vgoalconfig.size() == 0)
		{
			this->environment->LockPhysics(false);
			return NULL;
		}
		// Cut the third Dof
		for (unsigned int i = 4; i < params.vgoalconfig.size(); ++i)
		{
			params.vgoalconfig[i - 1] = params.vgoalconfig[i];
		}
		params.vgoalconfig.pop_back();

		Trajectory* traj = this->environment->CreateTrajectory(this->robot->GetActiveDOF());

		this->planner->InitPlan(this->robot, &params);

#ifdef ORTIME
		clock_t start = clock();
#endif
		bool success = this->planner->PlanPath(traj);
#ifdef ORTIME
		clock_t end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "Time KinBody_Path: " << time << " sec" << std::endl;
#endif

		if (success)
		{
			this->environment->LockPhysics(false);
			return traj;
		}
		this->environment->LockPhysics(false);
		delete traj;
		return NULL;
	}

	Trajectory* OpenRave::planPath(const CPose3D& target)
	{
		PlannerBase::PlannerParameters params;
		// without the third Dof because its a dummy Dof
		vector<int> dofs;
		dofs.push_back(0);
		dofs.push_back(1);
		dofs.push_back(2);
		dofs.push_back(4);
		dofs.push_back(5);
		this->environment->LockPhysics(true);
		this->robot->SetActiveDOFs(dofs);
		this->robot->GetActiveDOFValues(params.vinitialconfig);

		this->calculateIKSolution(target, params.vgoalconfig);
		if (params.vgoalconfig.size() == 0)
		{
			this->environment->LockPhysics(false);
			return NULL;
		}
		// Cut the third Dof
		for (unsigned int i = 4; i < params.vgoalconfig.size(); ++i)
		{
			params.vgoalconfig[i - 1] = params.vgoalconfig[i];
		}
		params.vgoalconfig.pop_back();

		Trajectory* traj = this->environment->CreateTrajectory(this->robot->GetActiveDOF());

		this->planner->InitPlan(this->robot, &params);

#ifdef ORTIME
		clock_t start = clock();
#endif
		bool success = this->planner->PlanPath(traj);
#ifdef ORTIME
		clock_t end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "Time Pose_Path: " << time << " sec" << std::endl;
#endif

		if (success)
		{
			this->environment->LockPhysics(false);
			return traj;
		}
		this->environment->LockPhysics(false);
		delete traj;
		return NULL;
	}


	void OpenRave::convertAnglesKatanaToOpenRave(const vector<dReal>& katanaAngles, vector<dReal>& openRaveAngles)
	{
		// Adding one more Angles because OpenRAVE-Katana has an dummy joint
		openRaveAngles = katanaAngles;
		openRaveAngles.push_back(0);
		openRaveAngles[5] = openRaveAngles[4];
		openRaveAngles[4] = openRaveAngles[3];
		openRaveAngles[3] = 0;

		// Make conversion from Katana Jointangles to OpenRave Jointangles
		openRaveAngles[0] = M_PI - openRaveAngles[0];
		openRaveAngles[1] = M_PI_2 - openRaveAngles[1];
		openRaveAngles[2] = M_PI - openRaveAngles[2];
		openRaveAngles[4] = M_PI - openRaveAngles[4];
		openRaveAngles[5] = M_PI - openRaveAngles[5];
	}


	void OpenRave::convertAnglesOpenRaveToKatana(const vector<dReal>& openRaveAngles, vector<dReal>& katanaAngles)
	{
		katanaAngles = openRaveAngles;
		katanaAngles[3] = katanaAngles[4];
		katanaAngles[4] = katanaAngles[5];
		katanaAngles.pop_back();

		// Make conversion from Katana Jointangles to OpenRave Jointangles
		katanaAngles[0] = M_PI - katanaAngles[0];
		katanaAngles[1] = M_PI_2 - katanaAngles[1];
		katanaAngles[2] = M_PI - katanaAngles[2];
		katanaAngles[3] = M_PI - katanaAngles[3];
		katanaAngles[4] = M_PI - katanaAngles[4];
	}


	Trajectory* OpenRave::planPath(const vector<dReal>& angles)
	{
		PlannerBase::PlannerParameters params;
		// without the third Dof because its a dummy Dof
		vector<int> dofs;
		dofs.push_back(0);
		dofs.push_back(1);
		dofs.push_back(2);
		dofs.push_back(4);
		dofs.push_back(5);
		this->environment->LockPhysics(true);
		this->robot->SetActiveDOFs(dofs);
		this->robot->GetActiveDOFValues(params.vinitialconfig);
		params.vgoalconfig = angles;

		Trajectory* traj = this->environment->CreateTrajectory(this->robot->GetActiveDOF());

		this->planner->InitPlan(this->robot, &params);

#ifdef ORTIME
		clock_t start = clock();
#endif
		bool success = this->planner->PlanPath(traj);
#ifdef ORTIME
		clock_t end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "Time Angles_Path: " << time << " sec" << std::endl;
#endif

		if (success)
		{
			this->environment->LockPhysics(false);
			return traj;
		}
		this->environment->LockPhysics(false);
		delete traj;
		return NULL;
	}

	void OpenRave::moveRobotToPosition(const vector<dReal>& solution)
	{
		dReal array[solution.size()];

		for (unsigned int i = 0; i < solution.size(); ++i)
		{
			array[i] = solution[i];
		}
		this->environment->LockPhysics(true);
		this->robot->SetActiveDOFValues(NULL, array, true);
		this->environment->LockPhysics(false);
	}

	void OpenRave::moveRobotToPosition(Trajectory* traj)
	{
		this->environment->LockPhysics(true);
		this->robot->SetActiveMotion(traj, 10.0);
		this->environment->LockPhysics(false);
	}

	void OpenRave::openGripper()
	{
		vector<int> dofs;
		dofs.push_back(0);
		dofs.push_back(1);
		dofs.push_back(2);
		dofs.push_back(3);
		dofs.push_back(4);
		dofs.push_back(5);
		dofs.push_back(6);
		dofs.push_back(7);
		dReal array[8];
		this->environment->LockPhysics(true);
		this->robot->SetActiveDOFs(dofs);
		this->robot->GetActiveDOFValues(array);
		array[7] = 0.34;
		this->robot->SetActiveDOFValues(NULL, array, false);
		this->environment->LockPhysics(false);
	}

	void OpenRave::closeGripper()
	{
		string result;
		problem->SendCommand("CloseFingers", result);
	}

	void OpenRave::grab(KinBody* object)
	{
		this->grabbedTransform = object->GetTransform();
		TransformMatrix transform = this->robot->GetActiveManipulator()->GetEndEffectorTransform();
		transform.trans.z += getExtents(object).x;
		RaveVector<dReal> rotation(0, 1, 0);
		object->SetTransform(transform);
		this->robot->Grab(object, NULL);
		//		string result;
		//		problem->SendCommand("GrabBody name object1", result);
	}

	void OpenRave::release(KinBody* object)
	{
		this->robot->Release(object);
		object->SetTransform(this->grabbedTransform);
		//		string result;
		//		problem->SendCommand("ReleaseAll", result);
	}

	void OpenRave::release(KinBody* object, RaveVector<dReal> target)
	{
		this->robot->Release(object);
		this->grabbedTransform.trans = target;
		object->SetTransform(this->grabbedTransform);
		//		string result;
		//		problem->SendCommand("ReleaseAll", result);
	}

	unsigned int OpenRave::getRandom() {
		return this->environment->RandomInt() / 2;
	}

	/*************************************************

	 Private Methods

	 *************************************************/

	void OpenRave::init()
	{
		this->environment = CreateEnvironment(true);
		this->pluginInfo = new PLUGININFO();

		this->environment->LockPhysics(true);
		// Load Default Environment with ground
		this->environment->Load(this->DEFAULTENV.c_str());
		// Load Katana from OpenRave and save it
		this->robot = this->environment->ReadRobotXML(NULL, this->KATANA.c_str(), NULL);
		// Translate Katana so it fits into the environment
		TransformMatrix transform;
		transform.identity();
		transform.trans.z = -0.215;
		this->robot->ApplyTransform(transform);
		// Add Robot to the environment
		this->environment->AddRobot(this->robot);
		this->environment->LockPhysics(false);

		//	this->planner = this->environment->CreatePlanner(L"BasicRRT");
		this->planner = this->environment->CreatePlanner(L"rBirrt");
		//	this->planner = this->environment->CreatePlanner(L"RA*");
		//	this->planner = this->environment->CreatePlanner(L"BiSpace");
		this->problem = this->environment->CreateProblem(L"BaseManipulation");
		this->problem->main("Katana6M180");
		this->objects = new vector<pair<string, KinBody*> > ();
	}

	vector<const wchar_t*>* OpenRave::getPluginNames(PluginType type)
	{
		vector<const wchar_t*>* result = new vector<const wchar_t*> ();
		vector<wstring> container;

		switch (type)
		{
		case OpenRAVE::PT_CollisionChecker:
			container = this->pluginInfo->collisioncheckers;
			break;
		case OpenRAVE::PT_Controller:
			container = this->pluginInfo->controllers;
			break;
		case OpenRAVE::PT_InverseKinematicsSolver:
			container = this->pluginInfo->iksolvers;
			break;
		case OpenRAVE::PT_PhysicsEngine:
			container = this->pluginInfo->physicsengines;
			break;
		case OpenRAVE::PT_Planner:
			container = this->pluginInfo->planners;
			break;
		case OpenRAVE::PT_ProblemInstance:
			container = this->pluginInfo->problems;
			break;
		case OpenRAVE::PT_Robot:
			container = this->pluginInfo->robots;
			break;
		case OpenRAVE::PT_Sensor:
			container = this->pluginInfo->sensors;
			break;
		case OpenRAVE::PT_SensorSystem:
			container = this->pluginInfo->sensorsystems;
			break;
		case OpenRAVE::PT_Server:
			container = this->pluginInfo->servers;
			break;
		case OpenRAVE::PT_Trajectory:
			container = this->pluginInfo->trajectories;
			break;
#ifndef OPENRAVE_WITHOUT_GUI
		case OpenRAVE::PT_Viewer:
			container = this->pluginInfo->viewers;
			break;
#endif
		default:
			return result;
		}

		for (unsigned int i = 0; i < container.size(); ++i)
		{
			result->push_back(container[i].c_str());
		}

		return result;
	}

	void OpenRave::buildPluginList()
	{
		list<std::pair<string, PLUGININFO> > info;

		this->environment->GetPluginInfo(info);

		list<std::pair<string, PLUGININFO> >::iterator iter;

		for (iter = info.begin(); iter != info.end(); ++iter)
		{
			if (iter->second.collisioncheckers.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->collisioncheckers, iter->second.collisioncheckers.begin(),
						iter->second.collisioncheckers.end());
			} else if (iter->second.controllers.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->controllers, iter->second.controllers.begin(), iter->second.controllers.end());
			} else if (iter->second.iksolvers.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->iksolvers, iter->second.iksolvers.begin(), iter->second.iksolvers.end());
			} else if (iter->second.physicsengines.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->physicsengines, iter->second.physicsengines.begin(),
						iter->second.physicsengines.end());
			} else if (iter->second.planners.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->planners, iter->second.planners.begin(), iter->second.planners.end());
			} else if (iter->second.problems.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->problems, iter->second.problems.begin(), iter->second.problems.end());
			} else if (iter->second.robots.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->robots, iter->second.robots.begin(), iter->second.robots.end());
			} else if (iter->second.sensors.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->sensors, iter->second.sensors.begin(), iter->second.sensors.end());
			} else if (iter->second.sensorsystems.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->sensorsystems, iter->second.sensorsystems.begin(), iter->second.sensorsystems.end());
			} else if (iter->second.servers.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->servers, iter->second.servers.begin(), iter->second.servers.end());
			} else if (iter->second.trajectories.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->trajectories, iter->second.trajectories.begin(), iter->second.trajectories.end());
			}
#ifndef OPENRAVE_WITHOUT_GUI
			else if (iter->second.viewers.size() > 0)
			{
				this->getPluginListSubMethode(this->pluginInfo->viewers, iter->second.viewers.begin(), iter->second.viewers.end());
			}
#endif
		}
	}

	void OpenRave::getPluginListSubMethode(vector<wstring>& container, const vector<wstring>::iterator& begin, const vector<wstring>::iterator& end)
	{
		for (vector<wstring>::iterator iter = begin; iter < end; ++iter)
		{
			container.push_back(*iter);
		}
	}
}
