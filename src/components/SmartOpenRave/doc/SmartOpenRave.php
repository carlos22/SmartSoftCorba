<?php
/**********************************************************************************
  SmartSoft Component Description File

  Author:	Jonas Brich, Dennis Stampfer
  Created:	22.9.2010

  Last changed (for matching with codebase): 
  			June 30th 2011

**********************************************************************************/


$components[] = array(
	"Name" => "SmartOpenRave",

	"Description" => "
		This component is based on the OpenRAVE framework. It allows to plan a
		trajectory to a given point with the specified manipulator. The path
		planning performs collision free manipulation by taking, for example,
		obstacles in the scene into account.
		The scene can be loaded from a XML file or via an
		object recognition. Furthermore it is possible to plan higher level
		tasks like grasping an object and putting it on a table.
		All coordinates are on the robot coordinate system.
		",




	"Metadata" => array(
		"Purpose" => "Manipulation",
		"License" => "LGPL",
		"Hardware" => "-",

		/* Library dependencies other than SmartSoft-Framework */
		"Library Dependencies" => "OpenRave = rev. 1780",
		"SVN" => "http://smart-robotics.svn.sourceforge.net/viewvc/smart-robotics/trunk/smartsoft/src/components/SmartOpenRave/",
	), //Metadata




	"States" => array(
		"neutral" => "The component does not perform any planning or IK calculation. It accepts parameters.",

		"trajectory" => "The component can plan paths or plan higher level tasks like grasping an object and place it somewhere.",

		"demonstration" => "The component just synchronizes the modeled manipulator with the real manipulator. This state is mainly for testing purpose.",

		"simulation" => "The component does not send any trajectory to the real manipulator. It computes all IK solutions and plans path as in ''trajectory'' state.",

	), // States




	"Parameters" => array(
		"ENV_CLEAR" => "The scene is reset to its default as loaded initially from the ini-configuration.",

		"ENV_LOAD_OBJECTRECOGNITION(?envid)" => "The environment with id ?envid is loaded from the object recognition. The environmentQueryClient is used to get the environment from the object recognition.",
		
		"ENV_SAVE_FILE(?fileid)" => "Saves the complete environment to a file. The filename can be specified in the ini-file. The ?fileid specifies the concrete file.",

		"ENV_LOAD_FILE(?fileid)" => "The environment is loaded from a file. The filename can be specified in the ini-file. The ?fileid specifies the concrete file.",

		"OPENRAVE_GRASPOBJ(?objid)" => "The specified object with id ?objid is grasped in OpenRAVE. This parameter is necessary for linking the object to the manipulator. After this operation the grasped object is part of the manipulator and is therefore considered in the path planning process. There will be no calculation how the object is grasped, the object will be just linked to the manipulator. If the object is not between the gripper the object will also be linked to the manipulator even if the object is not grasped in real. So be aware when to set this parameter.",

		"OPENRAVE_RELEASEOBJ(?objid)" => "The specified object with id ?objid is released in OpenRAVE and is no longer part of the manipulator. See GRASPOBJ for more information.",

		"OBJ_COMPUTEGRASPTABLE(?objid)" => "Computes the grasp tables to a given object with id ?objid. The grasp tables are necessary for the higher tasks. The grasp tables will be computed only once for one type of object, for example a cup will only have one grasp table as long as it has the same dimensions. Furthermore the computation needs a couple of minutes and should always be done separately, for example when new objects are learned.",

		"OBJ_ADD(?objid)(?type)" => "An object from ?type with ?objid is added to the current environment. The object type has to be known by the OpenRave component. The object is added into the origin (0,0,0).",

		"OBJ_DELETE(?objid)" => "Deletes the object with id ?objid out of the current environment.",


		"OBJ_MOVE(?objid)(?x)(?y)(?z)" => "Moves the object with ?objid to the specified (absolute) coords [m] in robot coordinates.",

		"GRASPING_NONE" => "Only the trajectory to the specific point is planned upon the next path planning send request to pathPlanningSendServer",

		"GRASPING_SIMPLE(?lowerHeight)(?upperHeight)(?lowerDepth)(?upperDepth)(?lowerAngle)(?upperAngle)(?lowerRoll)(?upperRoll)" => "When this is set, a simple iterative grasping method is applied to compoute a grasping pose to the given grasping position. The pose will be iterated through the given boundaries. Iteration starts at the lower bound and proceeds to the upper bound. Direction can be changed by giving the upper bound a lower value than the lower bound. Height, depth in meter. Angles in degree.",

		"GRASPING_ADVANCED" => "Not implemented.",
		// When this flag is set, the grasping method of OpenRave is used.

		"SIMULATION_TEST_IK_ONLY" => "When set, the component only tests whether an IK solution can be found and fires PATH_FOUND if found. No path calculation and manipulation will be done. Works only in ''simulation'' state.",

		"SIMULATION_PLAN_ALL" => "IK solution and path will be calculated.",

		"PARALLELIZATION_ON" => "Turns parallelization mode on. The virtual openRave manipulator is only synchronized once before the next trajectory. For all further trajectories, the openRave manipulator will not be synchronized with the real one.",

		"PARALLELIZATION_OFF" => "Turns parallelization mode off. The virtual openRave manipulator will be synchronized before every trajectory planning.",


		//"" => "",


	), // Parameters




	"Client-Ports" => array(
		"trajectorySendClient" => array(
			"Type" => "SmartSendClient",
			"Metadata" => array(
				"commObject" => "CommManipulatorTrajectory",
			),
			"Description" => "
				Connect this send client service to the real manipulator component, e.g. SmartKatanaServer. Via this port the trajectory which has been planned is sent to the real manipulator.
			",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "The port sends the planned trajectroy to the manipulator.",
				"demonstration" => "The port is neutral and does nothing.",
				"simulation" => "The port is neutral and does nothing.",
			)
		), //


		"manipulatorStatePushTimedClient" => array(
			"Type" => "SmartPushTimedClient",
			"Metadata" => array(
				"interval" => "1",
				"commObject" => "CommMobileManipulatorState",
			),
			"Description" => "
				Connect this client service to the real manipulator component (e.g. SmartKatanaServer)
				to receive the current joint configuration and tool center point of the manipulator.
			",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "The port receives new updates from the server.",
				"demonstration" => "The port receives new updates from the server.",
				"simulation" => "The port receives new updates from the server.",
			)
		), //

		"manipulatorEventClient" => array(
			"Type" => "SmartEventClient",
			"Metadata" => array(
				"eventParameter" => "CommManipulatorEventParameter",
				"eventResult" => "CommManipulatorEventResult",
			),
			"Description" => "
				Connect this client service to the real manipulator component
				to receive events. The events are important for the path planning. For example it is necessary to receive the \"GOAL_REACHED\" event to know if the real manipulator is ready to receive a next trajectory.
			",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "The port receives new events from the server.",
				"demonstration" => "The port receives new events from the server.",
				"simulation" => "The port receives new events from the server.",
			)
		), //

		"environmentQueryClient" => array(
			"Type" => "SmartQueryClient",
			"Metadata" => array(
				"commRequestObject" => "CommObjectRecognitionId",
				"commAnswerObject" => "CommObjectRecognitionEnvironment",
			),
			"Description" => "
				Connect this query client service to an object recognition to
				load an environment from it. The environment contains serveral objects which will be loaded into OpenRAVE. These objects will be considered in the path planning process. If no object recognition is available it is possible to load an
				environment from a file which can be specified in the ini-configuration.
			",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "The port queries the environment with the objects in it.",
				"demonstration" => "The port is neutral and does nothing.",
			)
		), //

		"objectQueryClient" => array(
			"Type" => "SmartQueryClient",
			"Metadata" => array(
				"commRequestObject" => "CommObjectRecognitionId",
				"commAnswerObject" => "CommObjectRecognitionObjectFeatures",
			),
			"Description" => "
				Connect this query client service to an object recognition to
				load objects from it. The object has to be loaded from this port to get information about the type and extents of the object. This values are necessary to compute the grasp tables for this object. If this port is not connected the computation of the grasp tables cannot be done.
			",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "The port queries the object.",
				"demonstration" => "The port is neutral and does nothing.",
			)
		), //



	), // Client-Ports




	"Server-Ports" => array(
		"stateServer" => array(
			"Type" => "SmartStateServer",
			"Metadata" => array(
				"serviceName" => "state",
			),
			"Description" => "
				See States for descriptions of possible states and their meaning.
			"
		), //


		"pathPlanningSendServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "pathPlanning",
				"commObject" => "CommManipulatorTrajectory",
			),
			"Description" => "
				Only the first tool center point or joint angle configuration in the communication object is used. For this tool center point or joint angle configuration a path planning is performed. The handler fires events when it begins to plan a trajectory ('PLANNING_PATH') or when a trajectory has been found ('PATH_FOUND') or when no trajectory can be calculated ('NO_PATH_FOUND' if the planning is unsuccessful or 'NO_IKSOLUTION_FOUND' if the inverse kinematic cannot be calculated). The environment has to be loaded before this handler is called over the parameter port. The tool center point in the trajectory must be given in the robot coordinate system.
			",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"trajectory" => "A path to the given pose is planned and sent to the real manipulator.",
				"demonstration" => "Port is neutral, does not consume new input while in this state.",
				"simulation" => "A path to the given pose is planned but not sent to the real manipulator.",
			)
		), //


		/**********
		  CURRENTLY NOT AVAILABLE
		"taskPlanningSendServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "taskPlanning",
				"commObject" => "CommManipulationTaskPlanning",
			),
			"Description" => "
			If the component is in “trajectory” state, the grasping task planning is performed. A trajectory to the given object is planned and sent to the manipulator. Then the object is graped according to the pre-calculated grasp tables. Finally a trajectory is calculated to the end pose where the object should be positioned.
The end pose where the object is put, must be given in the robot coordinate system.
			",
			"States" => array(
				"neutral" => "",
				"trajectory" => "",
				"demonstration" => ""
			)
		), //
		********************/

		"eventServer" => array(
			"Type" => "SmartEventServer",
			"Metadata" => array(
				"serviceName" => "eventServer",
				"eventParameter" => "CommManipulationPlannerEventParameter",
				"eventResult" => "CommManipulationPlannerEventResult",
				"eventState" => "CommManipulationPlannerEventState",
			),
			"Description" => "
				The event state UNKNOWN can be used for activation if the current state is unknown. The port will send the following events:

				<ul>
					<li>PLANNING_PATH: Fired when component started planning
						the path.

					<li>PATH_FOUND: Fired when the path planing was successful
						and a trajectory has been found.

					<li>NO_PATH_FOUND: Fired when no trajectory can be found
						to the specified position.

					<li>NO_IKSOLUTION_FOUND: Fired when the inverse kinematic caluclation fails.

					<li>MANIPULATOR_SYNC_FAIL: Event is fired if the modeled
						manipulator cannot be synced with the real manipulator.
				</ul>

					",
			"States" => array(
				"neutral" => "The port is neutral and does nothing.",
				"trajectory" => "All events are fired in this state.",
				"demonstration" => "All events are fired in this state.",
				"simulation" => "All events are fired in this state.",
			)
		), //


		"parameterServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "param",
				"commObject" => "CommManipulationPlannerParameter",
			),
			"Description" => "
				Accepts various parameters to handle the environment and objects in OpenRAVE. PARAMETER_SUCCESS and PARAMETER_FAIL are fired from the handler. Not all parameters fire the mentioned events. See parameter description for further information.
			",
			"States" => array(
				"neutral" => "All parameters are accepted in this state.",
				"trajectory" => "No parameters are accepted in this state.",
				"demonstration" => "No parameters are accepted in this state.",
				"simulation" => "No parameters are accepted in this state.",
			)
		), //



	), // Server-Ports


	"Ini-Configuration" => array(
		"OpenRave" => array(
			"debugOpenRave: boolean" => "Debugs communication with OpenRAVE. Default:
				false",

			"debugSend: boolean" => "Debugging for path planning send server. Default:
				false",
			
			"defaultEnvironmentPath: string" => "File to default OpenRAVE environment.
				Default: none, current working directory.",
			
			"gui_active: boolean" => "Enable/disable GUI to visualize. Default: true",
			
			"robotName: string" => "Name of the manipulator as specified in the
				robotPath xml file. Default: Katana",
			
			"robotPath: string" => "Xml file which contains the robot/manipulator
				specification. Default: robots/katana.robot.xml",
			
			"saveObjectsToFile: boolean" => "Saves every object in the scenes
				automatically to files named after object id. Default: file",
			
			"saveTrajectoryToFile: boolean" => "Saves automatically the trajectory to a
				file, see above. Default: false",
			
			"showCompleteTrajectory: boolean" => "Live visualization of robot movement
				while trajectory is executed. Default: false",
			
			"storedEnvironmentName: string" => "Filename prefix used to store the
				environment. Filename: &lt;storedEnvironmentName&gt;&lt;id&gt;.dae. Default:
				storedEnvironment",
			
			"storedEnvironmentPath: string" => "Path to store the files.",
		),
	
		"PortParameter" => array(
			"withManipulator: boolean" => "Component does not connect to katana server and does
				not perform real trajectories. Can be used for testing.
				Default: true",

			"withObjectRecognition: boolean" => "Specify whether component connects to the object recognition. If not connected only environments from the file can be loaded.
				Default: true",
		),
	), // Ini-Configuration


);
?>
