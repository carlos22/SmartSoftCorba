<?php
/**********************************************************************************

  NOT YET FINISHED! TODO!

  SmartSoft Component Description File

  Author:	Manuel Wopfner
  Created:	16.9.2010

  Last changed (for matching with codebase): 
  			16.9.2010



  TODO:
   - gripper actions?
   - default value for katana.high_precision = false?
   - ip_adress in ini file ist in komponente falsch geschrieben.
   - behavior of ports in states
   - grafik mit pattern an ports

**********************************************************************************/


$components[] = array(
	"Name" => "SmartKatanaServer",

	"Description" => "This component controls a Katana manipulator. It is
		possible to perform trajectories consisting of a sequence of points. The
		arm can also be set to demonstration where it is possible to move the arm
		by hand. Further, if a laser ranger is mounted on the Katana, it is
		possible to request a 3d laser scan, which is then captured by the
		component.

		<p>GPL-License: Component includes a patched Katana KNI 4.2.0 which is licensed
		under GPL.
		",




	"Metadata" => array(
		"Purpose" => "Hardware-Driver",
		"License" => "GPL",
		"Hardware" => "Katana 300, 350, 400, 450",

		/* Library dependencies other than SmartSoft-Framework */
		"Library Dependencies" => "-",
		"SVN" => "http://smart-robotics.svn.sourceforge.net/viewvc/smart-robotics/trunk/smartsoft/src/components/SmartKatanaServer/",
	), //Metadata




	"States" => array(
		"neutral" => "In 'neutral' state the Katana does not perform any
			trajectory. No manipulator state is pushed to posePushTimedServer port.", 

		"trajectory" => "In
			'trajectory' state given trajectories are performed by the Katana.",

		"demonstration" => "In 'demonstration' state the Katana deactivates its motors,
			so that the arm can be moved by a person. It is important to bring the arm in a
			save position before the state change, so it does not fall down on the robot.",

		"scan" => "In 'scan' state and if a laser ranger is mounted on the Katana it is
			possible to capture 3d point clouds by the manipulator.",
	), // States


	"Parameters" => array(
		"STRATEGY(?value)" => "The following strategies can be set:
							   <ul>
									<li>TRAJECTORY: All poses specified in the trajecotry sequence are performed.</li>
									<li>SINGLEPOSE: Only the first pose in a trajectory sequence is performed.</li>
							   </ul>",
		"ALLJOINTVELLIMITS(?limit)" => "Set the velocity limit ?limit [rad/s] for all joints.",
		"GRIPPERVELLIMIT(?limit)" => "Set the velocity limit ?limit [rad/s] for the gripper.",
	), // Parameters




	"Client-Ports" => array(
	

		"basePushTimedClient" => array(
			"Type" => "SmartPushTimedClient",
			"Metadata" => array(
				"commObject" => "CommBaseState",
				"interval" => "1",
			),
			"Description" => "Typically connected to the robot base (e.g.
				SmartPioneerBaseServer) to receive the base state which is included in the
				laser scan. This port is connected optionally and can be activated/deactivated in the .ini file
				with the base.on_base parameter.",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"trajectory" => "???",
				"demonstration" => "???",
				"scan" => "??The base state is included in the laser.??",
			)
		), //

		"laserQueryClient" => array(
			"Type" => "SmartQueryClient",
			"Metadata" => array(
				"commRequestObject" => "CommVoid",
				"commAnswerObject" => "CommMobileLaserScan",
			),
			"Description" => "If it should be possible to capture 3d laser
				scans, a laser must be mounted on the last joint of the Katana.
				Connect this client service to the component that is responsible
				for this laser ranger. This port is optional and can be activated/deactivated in the .ini file
				with the laser.laser_is_mounted parameter.",
			"States" => array(
				"neutral" => "Inactive in this state.",
				"trajectory" => "Inactive in this state.",
				"demonstration" => "Inactive in this state.",
				"scan" => "Requests the current laser scan from the laser ranger.",
			)
		), //

		"laserStateClient" => array(
			"Type" => "SmartStateClient",
			"Metadata" => array(
				"commAnswerObject" => "CommMobileLaserScan",
			),
			"Description" => "If it should be possible to capture 3d laser
				scans, a laser must be mounted on the last joint of the Katana.
				Connect this client service to the component that is responsible
				for this laser ranger. This port is optional and can be activated/deactivated in the .ini file
				with the laser.laser_is_mounted parameter.",
			"States" => array(
				"neutral" => "Inactive in this state.",
				"trajectory" => "Inactive in this state.",
				"demonstration" => "Inactive in this state.",
				"scan" => "Sets the state of the laser ranger to active.",
			)
		), //



	), // Client-Ports




	"Server-Ports" => array(
		"trajectorySendServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "performTrajectory",
				"commObject" => "CommManipulatorTrajectory",
			),
			"Description" => "A new trajectory (one or more points) which is sent to the Katana is performed.
					The trajectories are stored in a queue. The next trajectory is performed when the
					last point from the current trajectory is send to the Katana. There is no check if
					the next trajectory starts at the current TCP position. The
					component fires an event (see eventServer) when the Katana
					reaches the last position. The poses of the trajectory must
					be given in the robot coordinate system.
					
					It is possible to choose between different gripper actions
					for a trajectory, which can be set in
					CommManipulatorTrajectory.
					<ul>
						<li>NO_OP: No action is performed on the gripper.</li>
						<li>ANGLE: The joint angles given in the trajectory are used.</li>
						<li>CLOSE_AFTER: The gripper is closed after the arm reaches the goal.</li>
						<li>CLOSE_BEFORE: The gripper is closed before the arm starts to move.</li>
						<li>CLOSE_BEFORE_OPEN_AFTER: The gripper is closed before the arm starts to move
							and opend after it reaches the goal.</li>
						<li>OPEN_AFTER: The gripper is opend after the arm reaches the goal.</li>
						<li>OPEN_BEFORE: The gripper is opend before the arm starts to move.</li>
						<li>OPEN_BEFORE_CLOSE_AFTER: The gripper is opend before the arm starts to move
							and closed after it reaches the goal.</li>
					</ul>
					
					Wann kommt GOAL_RECHED bei OPEN_AFTER zb? nachdem gripper offen oder nachdem punkt angefahren?
					",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"trajectory" => "A trajectory is performed.",
				"demonstration" => "Port is neutral, does not consume new input while in this state.",
				"scan" => "Port is neutral, does not consume new input while in this state.",
			)
		), //

		"posePushTimedServer" => array(
			"Type" => "SmartPushTimedServer",
			"Metadata" => array(
				"serviceName" => "currentPose",
				"cycle" => "100ms",
				"commObject" => "CommMobileManipulatorState",
			),
			"Description" => "The current pose of the manipulator (tcp pose,
				angles, ...) is pushed to all clients which are subscribed.",
			"States" => array(
				"neutral" => "Port is neutral, does not publish new poses while in this state.",
				"trajectory" => "The Katana pose is pushed but there is no guarantee that it is the current pose.
						   It can also be the last pose of the Katana.",
				"demonstration" => "The current pose is always read from the Katana.",
				"scan" => "The Katana pose is pushed but there is no guarantee that it is the current pose.
						   It can also be the last pose of the Katana.",
			)
		), //

		"stateServer" => array(
			"Type" => "SmartStateServer",
			"Metadata" => array(
				"serviceName" => "state",
			),
			"Description" => "See States for descriptions of possible states and their meaning.",
		), //

		"eventServer" => array(
			"Type" => "SmartEventServer",
			"Metadata" => array(
				"serviceName" => "eventServer",
				"eventParameter" => "CommManipulatorEventParameter",
				"eventResult" => "CommManipulatorEventResult",
			),
			"Description" => "
				The event state PLANNER_UNKNOWN can be used for activation if the current state is unknown. 
				
				The following events are sent by this port:
				<ul>
					<li>COLLISION: Is fired when the arm collides with an
						obstacle

					<li>NO_SOLUTION_FOUND: Is fired if the given tcp pose
						is not reachable by the Katana
					
					<li>VALUE_OUT_OF_RANGE: Is fired if an
						angle specified in a trajectory is not reachable by the Katana
			
					<li>GOAL_REACHED: Is fired when the arm reaches the position specified
						by the last trajectory
	
					<li>GOAL_NOT_REACHED: Sent when a new goal was specified.

				</ul>",
			"States" => array(
				"neutral" => "Port is neutral, does not publish new events while in this state.",
				"trajectory" => "Will send Katana events.",
				"demonstration" => "Port is neutral, does not publish new events while in this state.",
				"scan" => "Port is neutral, does not publish new events while in this state.",
			)
		), //

		"paramServer" => array(
			"Type" => "SmartParameterServer",
			"Metadata" => array(
				"serviceName" => "param",
				"commObject" => "CommManipulatorParameter",
			),
			"Description" => "Allows the parametrization of the component. See Parameters.",
			"States" => array(
				"neutral" => "Accepts parameters.",
				"trajectory" => "Accepts parameters.",
				"demonstration" => "Accepts parameters.",
				"scan" => "Accepts parameters.",
			)
		), //

		"requestScan3dSendServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"serviceName" => "request3dscan",
				"commObject" => "CommManipulatorRequestScan3d",
			),
			"Description" => "3d laser scans can be requested if a laser ranger
				is mounted on the manipulator. The scan is then performed in the
				area, specified by the commObject, and if the scan is finished an event is sent.",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"trajectory" => "Port is neutral, does not consume new input while in this state.",
				"demonstration" => "Port is neutral, does not consume new input while in this state.",
				"scan" => "Laser scans can be requested.",
			)
		), //

		"scan3dEventServer" => array(
			"Type" => "SmartEventServer",
			"Metadata" => array(
				"serviceName" => "newScan",
				"eventParameter" => "CommVoid",
				"eventResult" => "CommManipulatorId",
			),
			"Description" => "If a new 3d laser scan was captured, an event with
				an id is fired. The id allows to get the 3d point cloud from the
				component.",
			"States" => array(
				"neutral" => "Port is neutral, does not publish new events while in this state.",
				"trajectory" => "Port is neutral, does not publish new events while in this state.",
				"demonstration" => "Port is neutral, does not publish new events while in this state.",
				"scan" => "Sends new id of 3d point cloud.",
			)
		), //

		"scan3dQueryServer" => array(
			"Type" => "SmartQueryServer",
			"Metadata" => array(
				"serviceName" => "3dscan",
				"commRequestObject" => "CommManipulatorId",
				"commAnswerObject" => "Comm3dPointCloud",
			),
			"Description" => "A 3d point cloud for the specified id is
				returned. The component only saves the last point cloud. If the id of the last
				point cloud is different from the requested one, a point cloud with zero points is returned.",
			"States" => array(
				"neutral" => "Port is neutral, does not consume new input while in this state.",
				"trajectory" => "Port is neutral, does not consume new input while in this state.",
				"demonstration" => "Port is neutral, does not consume new input while in this state.",
				"scan" => "Returns 3d point cloud.",
			)
		), //




	), // Server-Ports


	/**
	  TODO:
	  	- add types
	  **/
	"Ini-Configuration" => array(
		"base" => array(
			"on_base" => "Specify if manipulator is mounted (true) on a mobile
				platform or not mounted on a mobile (false) platform. If false, fixed values
				will be used as the base pose. Default: true",

			"base_a" => "The orientation [rad] of the base if not mounted on a
				mobile platform. Default: 0",
			
			"steer_a" => "The steering angle [rad] of the base if not mounted on a
				mobile platform. Default: 0",
			
			"turret_a" => "The angle [rad] of the turret on the base if not mounted on a
				mobile platform. Default: 0",
			
			"x" => "The x world coordinate [mm] of the base if not mounted on a
				mobile platform. Default: 0",

			"y" => "The y world coordinate [mm] of the base if not mounted on a
				mobile platform. Default: 0",

			"z" => "The z world coordinate [mm] of the base if not mounted on a
				mobile platform. Default: 0",
			
		),

		"katana" => array(
			"verbose" => "Activate (true) or deactivate (false) debugging messages. Default: false",

			"fast_calibration" => "Turn fast calibration on (true) or off
				(false). If fast calibration is turned on, the component will check
				whether the katana is already calibrated by moving the first joint.
				Default: false",
			
			"katana_device" => "Specifies katana connection. Values: Serial:
				Katana is connected via the serial port. Lan: Katana is connected
				over the network. Default: Serial",
			
			"katana_config_file" => "The path to the katana configuration file.
				Default: src/config/katana6M180.cfg",
			
			"goal_angle_approach" => "The goal is reached when the difference between the
				goal and current tcp angles are smaller then this value [rad]. Default: 0.1",
			
			"goal_position_approach" => "The goal is reached when the difference between the
				goal and current tcp positions are smaller then this value [mm]. Default: 3",
			
			"azimuth" => "The azimuth angle of the katana relative to the robot [rad]. Default: 0",
			
			"elevation" => "The elevation angle of the katana relative to the robot [rad]. Default: 0",

			"roll" => "The roll angle of the katana relative to the robot [rad]. Default: 0",
			
			"x" => "The x offset of the katana relative to the robot [mm]. Default: 0",
			
			"y" => "The y offset of the katana relative to the robot [mm]. Default: 0",
			
			"z" => "The z offset of the katana relative to the robot [mm]. Default: 0",
		),	

		"laser" => array(
			"laser_is_mounted" => "Specifies whether a laser is mounted (true)
				on the Katana or not (false). Default: false",
			
			"high_precision" => "In high precision the laser ranger is run in query_only state
				which ensures that the laser scan is captured when the katana
				has stopped. Need about twice the time. Default: false",
			
			"azimuth" => "The azimuth angle relative to the tcp [rad]. Default:
				0",
			
			"elevation" => "The elevation angle relative to the tcp [rad].
				Default: 0",

			"roll" => "The roll angle relative to the tcp [rad]. Default: 0",
	
			"x" => "The x offset of the laser relative to the tcp [mm].
				Default: 0",

			"y" => "The y offset of the laser relative to the tcp [mm].
				Default: 0",
			
			"z" => "The z offset of the laser relative to the tcp [mm].
				Default: 0",
		),

		"network" => array(
			"ip_address" => "The ip address where the katana is connected if on
				network. See katana_device. Default: 127.0.0.1",

			"port" => "The port where the katana is connected, if connected
				through serial port. See katana_device. Default: 5566",
		),

		"serial_port" => array(
			"device" => "The serial device where the katana is connected.
				Default: /dev/ttyS0",
		),
	), // Ini-Configuration



);
?>
