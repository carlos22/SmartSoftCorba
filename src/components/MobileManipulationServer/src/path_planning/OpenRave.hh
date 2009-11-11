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

#ifndef OPENRAVE_H_
#define OPENRAVE_H_

#include <string>
#include <vector>
#include <set>
#include <openrave-core.h>
#include <mrpt/core.h>

using namespace OpenRAVE;
using std::string;
using std::vector;
using std::pair;
using std::wstring;
using mrpt::math::CPose3D;

namespace mmp
{
	/**
	 * This class is a wrapper for the OpenRAVE tool
	 */
	class OpenRave
	{
		public:
			/**
			 * Pathname to the Katana XML-File
			 */
			static const string KATANA;

			/**
			 * Default Environment with Katana and Floor and all things in the right spot
			 */
			static const string DEFAULTENV;

			/**
			 * How much iterations of the Gripperposition should be done
			 */
			static const int ITERATIONS;

		private:
			/**
			 * This is the main class of OpenRAVE
			 */
			EnvironmentBase* environment;

			/**
			 * Holds the current robot
			 */
			RobotBase* robot;

			/**
			 * Holds the planner
			 */
			PlannerBase* planner;

			/**
			 * Holds the problem Instance
			 */
			ProblemInstance* problem;

			/**
			 * This holds all the objects which are grabable
			 */
			vector<pair<string, KinBody*> >* objects;

			/**
			 * All Plugins which can be loaded into OpenRAVE
			 */
			PLUGININFO* pluginInfo;

			/**
			 * Current Iteration of the IK
			 */
			int iterations;

			TransformMatrix grabbedTransform;
		public:
			/**
			 * Default Constructor
			 */
			OpenRave();

			/**
			 * Destructor
			 */
			virtual ~OpenRave();

			/**
			 * Starts OpenRAVE, after this methode OpenRave is ready for use
			 */
			void run();

			/**
			 * Starts the GUI
			 */
			void startGUI();

			/**
			 * Stops the GUI
			 */
			void stopGUI();

			/**
			 * Resets the hole scene.
			 *
			 * Deletes all objects in it, Planer/Problems will be not deleted
			 */
			void resetAll();

			void deleteObject(KinBody* body);

			/**
			 * Reads a KinBody into OpenRAVE
			 *
			 * @param path the path where the file is stored
			 * @param type a string which specifies which type the object is
			 * @param matrix the matrix defines the pose in the world
			 * @param obstacle true if it is a obstacle otherwise false
			 * @param import true if the scene should be reseted otherwise false
			 */
			void readKinBody(const string& path, const string& type,
					const double matrix[], bool obstacle = false, bool import =
							false);

			/**
			 * Reads an Environment with Objects and Robots into OpenRAVE
			 */
			void readEnv(const string& path, bool import = false);

			/**
			 * Gets all grabable KinBodies
			 */
			vector<pair<string,KinBody*> >* getAllKinBodies();

			/**
			 * Gets all grabable KinBodies of the specified type
			 */
			vector<KinBody*>* getKinBodies(const string& type);

			/**
			 * Get KinBody for the specified id
			 */
			KinBody* getKinBody(uint32_t id);

			/**
			 * Returns the Extentions of a Object
			 */
			const Vector& getExtents(KinBody* object);

			int getCurrentIteration();

			/**
			 * Calculates the IKSolution to a KinBody
			 */
			void calculateIKSolution(KinBody* target, vector<dReal>& solution, int iteration = -1, double offset = 0);

			/**
			 * Calcualtes the IKSolution to a PoseObject
			 */
			void calculateIKSolution(const CPose3D& pose, vector<dReal>& solution);

			void convertAnglesKatanaToOpenRave(const vector<dReal>& katanaAngles, vector<dReal>& openRaveAngles);

			void convertAnglesOpenRaveToKatana(const vector<dReal>& openRaveAngles, vector<dReal>& katanaAngles);


			/**
			 * Plans a Path with avoiding obstacles to the given target
			 */
			Trajectory* planPath(KinBody* target, int iteration = -1, double offset = 0);

			/**
			 * Plans a Path with avoiding obstacles to the given target
			 */
			Trajectory* planPath(const CPose3D& target);

			/**
			 * Plans a Path with avoiding abtacles to the given joint angles
			 */
			Trajectory* planPath(const vector<dReal>& angles);

			/**
			 * Moves the Robot to the given position which is specified by joint values
			 */
			void moveRobotToPosition(const vector<dReal>& solution);

			/**
			 * Moves the Robot to the given position which is specified by a Trajectory
			 */
			void moveRobotToPosition(Trajectory* traj);

			void openGripper();

			void closeGripper();

			/**
			 * Grabs the given object
			 */
			void grab(KinBody* object);

			/**
			 * Releases all grabbed objects
			 */
			void release(KinBody* object);

			void release(KinBody* object, RaveVector<dReal> target);

			unsigned int getRandom();

		private:
			/**
			 * Initializes all necessary things, the Environment class and the PLUGININFO struct
			 */
			void init();

			/**
			 * Returns all Names of the Plugins from the same type.
			 */
			vector<const wchar_t*>* getPluginNames(PluginType type);

			/**
			 * Gets all Plugins and builds a new Pluginlist in which all Plugins are stored.
			 *
			 * This Pluginlist (PLUGININFO) can be searched and accessed.
			 */
			void buildPluginList();

			/**
			 * Sub Methode for the Methode buildPluginList.
			 *
			 * It adds all Plugins to the given Container.
			 */
			void getPluginListSubMethode(vector<wstring>& container,
					const vector<wstring>::iterator& begin, const vector<
							wstring>::iterator& end);
	};
}

#endif /* OPENRAVE_H_ */
