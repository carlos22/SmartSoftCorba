<?php
/**********************************************************************************
  SmartSoft Component Description File

  Author:	Dennis Stampfer
  Created:	22.9.2010

  Last changed (for matching with codebase): 
  			27.4.2011 / mdsd

**********************************************************************************/


$components[] = array(
	"Name" => "SmartPlannerBreadthFirstSearch",

	"Description" => "	
		The SmartPlannerBreadthFirstSearch provides path planning services
		based on grid maps. It uses a grid map from a map building component
		(e.g. SmartMapperGridMap) and sends an intermediate waypoint as well as
		the goalpoint to the motion execution (e.g. SmartCdlServer).
		
		<p>A wave propagation algorithm starting from goal cells backward to the
		current position of the robot is used to calculate a path. The path
		planning is further enhanced by a path shortening heuristic: the path
		is followed starting at the current robot position until a straight
		line from the robot position to the cell in question on the path
		interferes with an obstacle. The prior cell is then sent as an
		intermediate waypoint. The geometric path planning is applied
		continuously every one second.
		
		<p>Several goal circles and/or goal lines can be specified as goals in
		the planner. The planner will generate a path to the goal with the shortest
		distance.

		<p>The SmartPlannerBreadthFirstSearch for example can be used with SmartCdlServer
		which cannot handle local minimas. Goals are then specified in the
		SmartPlannerBreadthFirstSearch. A CDL_GOAL_REACHED event is fired by the CDL
		component as soon as the final goal is reached.

		<p>See also:<br>
		<i>
		Christian Schlegel. <a
		href='http://www.hs-ulm.de/users/cschlege/_downloads/phd-thesis-schlegel.pdf'>Navigation
		and Execution for Mobile Robots in Dynamic Environments: An Integrated
		Approach.</a> p. 27-29. Dissertation, Fakult&auml;t f&uuml;r Informatik, Universit&auml;t Ulm,
		2004.
		</i>

",




	"Metadata" => array(
		"Purpose" => "Navigation",
		"License" => "LGPL",
		"Hardware" => "-",

		/* Library dependencies other than SmartSoft-Framework */
		"Library Dependencies" => "none",
		"SVN" => "http://smart-robotics.svn.sourceforge.net/viewvc/smart-robotics/trunk/smartsoft/src/components/SmartPlannerBreadthFirstSearch/",
	), //Metadata




	"States" => array(
		"neutral" => "The path planning execution cycle is stopped. No path
			planning is performed, no intermediate waypoints and goal points are sent.",

		"pathplanning" => "The component will continuously (every 1 second)
			plan a path to the goalpoints. Intermediate waypoints and goals are sent.",
	), // States


	"Parameters" => array(
		"ID(?id)" => "Set the goal id. Used to synchronize components, for
			example with SmartMapperGridMap and SmartCdlServer.",

		"DELETEGOAL" => "Delete all specified planner goals.",
		
		"SETDESTINATIONCIRCLE(?x)(?y)(?r)" => "Specify a goal point by setting
			the coordinates [mm] ?x, ?y and radius ?r for the goal circle.",

		"SETDESTINATIONLINE(?x1)(?y1)(?x2)(?y2)" => "Specify a goal as a line:
			the planner will plan the shortest path from the current position to a
			line between the point ?x1,?y1 and ?x2, ?y2.",

	), // Parameters




	"Client-Ports" => array(
		"curMapClient" => array(
			"Type" => "SmartPushNewestClient",
			"Metadata" => array(
				"commObject" => "CommGridMap",
			),
			"Description" => "
				The planner will plan paths in grid maps sent to this port.
				Typically connected to a map building component such as 
				SmartMapperGridMap.
				",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"pathplanning" => "Latest map from this port is considered for path planning.",
			)
		), //

		"baseClient" => array(
			"Type" => "SmartPushTimedClient",
			"Metadata" => array(
				"interval" => "1",
				"commObject" => "CommBaseState",
			),
			"Description" => "
				Base states as input for path planning shall be sent to this
				port.  The path is planned based on the current position sent
				through this port. Typically connected to the robot base, e.g.
				SmartPioneerBaseServer.",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while
					in this state.", 
				"pathplanning" => "The latest position is
					considered as the current position for path planning.",
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
				",
			/*
			"States" => array(
				"neutral" => "Accepts parameters.",
				"pathplanning" => "Accepts parameters.",
			)
			*/
		), //

		"paramServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "param",
				"commObject" => "CommPlannerParameter",
			),
			"Description" => "
				Accepts various parameters for managing goals. See Parameters.
				",
			"States" => array(
				"neutral" => "Accepts parameters.",
				"pathplanning" => "Accepts parameters.",
			)
		), //

		"plannerGoalServer" => array(
			"Type" => "SmartPushNewestServer",
			"Metadata" => array(
				"serviceName" => "goal",
				"commObject" => "CommPlannerGoal",
			),
			"Description" => "If a path was found (event PLANNER_NO_ERROR
				fired), this port pushes an valid
				(invalid flag set to false) intermediate waypoint and goal
				point calculated by the planner to the motion execution. If no
				path was found, the waypoint and goalpoint are sent with the
				invalid flag set to true.",
			"States" => array(
				"neutral" => "Port is neutral, does not send any intermediate
					waypoints and goal points while in this state.",

				"pathplanning" => "Planner goals are sent to the motion
					execution as intermediate waypoints and goal points.",
			)
		), //

		/*
		PLANNER_GOAL_OK is never sent!
		PLANNER_PATH_FOUND is never sent!
		*/
		"plannerEventServer" => array(
			"Type" => "SmartEventServer",
			"Metadata" => array(
				"serviceName" => "plannerEvent",
				"eventParameter" => "CommPlannerEventParameter",
				"eventResult" => "CommPlannerEventResult",
				"eventState" => "PlannerEventState",
			),
			"Description" => "
				The event state PLANNER_UNKNOWN can be used for activation if
				the current state is unknown. Events are sent on every change.

				<p>The port will send the events listed below:

				<ul>
					<li>PLANNER_NO_PATH: 
						Fired if no path could be found for at least five
						seconds. Invalid goals (invalid flag=true) are pushed
						via plannerUpdateNewestServer when no path was found.

					<li>PLANNER_NO_ERROR:
						No errors in planner, valid goals (invalid flag=false)
						are pushed via plannerUpdateNewestServer.

					<li>PLANNER_UNKNOWN_ERROR:
		  				An unknown error occurred.

					<li>PLANNER_NO_GOAL_AVAILABLE:
			 			No goals available.

					<li>PLANNER_GOAL_NOT_MARKED:
						Every goal cell is already occupied. The goal cells
						could not be marked.

					<li>PLANNER_START_OCCUPIED_OBSTACLE:
						The start cell (current robot position) is occupied by
						an obstacle.

					<li>PLANNER_START_OCCUPIED_GOAL:
						The robot is standing on the goal cell.

					<li>PLANNER_WRONG_MAPID:
						The latest grid map is of another id than the current
						planner id.  The planner will not plan anything and
						will not send intermediate waypoints and goal points.

				</ul>
				",
			"States" => array(
				"neutral" => "Neutral, will not send any events.",
				"pathplanning" => "Will send the above events.",
			)
		), //



	), // Server-Ports

	/*
	   check description when adding cycle time parameter!

	"Ini-Configuration" => array(
		"group" => array(
			"" => "",
		),
	), // Ini-Configuration
	*/

	


);
?>
