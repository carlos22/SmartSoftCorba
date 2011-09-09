<?php
/**********************************************************************************
  SmartSoft Component Description File

  Author:	Dennis Stampfer
  Created:	16.9.2010

  Last changed (for matching with codebase): 
  			27.4.2011 / mdsd

**********************************************************************************/


$components[] = array(
	"Name" => "SmartPioneerBaseServer",

	"Description" => "The SmartPioneerBaseServer makes P2OS-based robot
		platforms available. It handles all the communication with the hardware. It
		offers several services for controlling the robot, such as sending navigation
		commands to the base and providing access to the robot's odometry.
		Position updates can be sent to the component to overcome odometry
		failures.
	
		<p>GPL-License: includes <a
		href='http://playerstage.sourceforge.net/doc/Player-2.1.0/player/group__driver__p2os.html'>Code
		from the Player Project</a>.", 




	"Metadata" => array(
		"Purpose" => "Hardware-Driver",
		"License" => "GPL",
		"Hardware" => "P2OS-based platforms. Currently supported: P3DX, P3DXSH, P3ATSH. Others can simply be added.",

		/* Library dependencies other than SmartSoft-Framework */
		"Library Dependencies" => "Boost >=1.33.1",
		"SVN" => "http://smart-robotics.svn.sourceforge.net/viewvc/smart-robotics/trunk/smartsoft/src/components/SmartPioneerBaseServer/",
	), //Metadata



/*
	"States" => array(
		"state1" => "descr of state 1",
	), // States
*/


	"Parameters" => array(
		"RESET" => "Reset the connection to the base at runtime. The serial
			connection is closed and reopened. All estimated positions are
			set to zero.",
	), // Parameters



//	"Client-Ports" => array(
//		"basePositionUpdateSendServer" => array(
//			"Type" => "SmartSendServer",
//			"Metadata" => array(
//				"commObject" => "CommBasePositionUpdate",
//				"serviceName" => "positionUpdate",
//			),
//			"Description" => "Connected to the base server to receive the base
//			state which is included in the laser scan. Also used to
//			calculate world coordinates of laser scan.",
//			/*
//			"States" => array(
//				"state1" => "Descr state 1"
//			)
//			*/
//		), //
//	), // Client-Ports



	"Server-Ports" => array(
		"basePositionUpdateServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"commObject" => "CommBasePositionUpdate",
				"serviceName" => "positionUpdate",
			),
			"Description" => "Port to send corrections of base pose to
				overcome the odometry failure. Accepts a pair of an old
				uncorrected pose and a new corrected pose. The deviation
				between these two poses is applied to correct the current pose
				of the robot.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //
		"navigationVelocityServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"commObject" => "CommNavigationVelocity",
				"serviceName" => "navigationvelocity",
			),
			"Description" => "Send new navigation velocity commands v and omega
				to hardware base. The values are thresholded by the min and max values
				specified in the ini file before being sent.
				<br>Note that the base will perform an emergency stop if no new velocity
				command was sent within the timeout specified in firmware.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //
		"basePositionServer" => array(
			"Type" => "SmartPushTimedServer",
			"Metadata" => array(
				"commObject" => "CommBaseState",
				"serviceName" => "basestate",
			),
			"Description" => "Push the base state containing current
				information about the robot's velocity, pose, raw pose. Should
				be used when a continuous base pose is required. For example,
				the SmartLaserLMS200Server uses the latest base pose (received
				continuously) to stamp the laser scan with the robot's
				pose at the time the scan was recorded.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //
		"paramServer" => array(
			"Type" => "SmartSendServer",
			"Metadata" => array(
				"commObject" => "CommBaseParameter",
				"serviceName" => "param",
			),
			"Description" => "Accept parameters at runtime. See section Parameters.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //
		"baseQueryServer" => array(
			"Type" => "SmartQueryServer",
			"Metadata" => array(
				"commRequestObject" => "CommVoid",
				"commAnswerObject" => "CommBaseState",
				"serviceName" => "basestatequery",
			),
			"Description" => "Query port to request the base state. Analogous
				to basePositionPushTimedServer, but a query service. Should be used
				when the base pose is needed sporadically, for example by a behavior
				component which explicitly needs to query the base pose from
				time to time.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //
	), // Server-Ports


	"Ini-Configuration" => array(
		"robot" => array(
			"robotType: string" => "Type of pioneer platform. Currently supported: p3dx, p3dxsh, p3atsh.",
			"enable_motors: boolean" => "Enable (true) or disable (false)
				motors at startup. Defines the state of the base' 'motors'-button
				on startup. Default: true",
			"enable_sonar: boolean" => "Enable (true) or disable (false) sonar at startup. Default: false",
			//"verbose: boolean" => "Verbose output. Default: false",
			"serialport: int32" => "Device name to access Pioneer Base, e.g. /dev/ttyS0",
			"maxVel: int32" => "Set maximum translation valocity of robot [mm/s]. Default: 1000",
			"maxVelAcc: int32" => "Set maximum translation acceleration of robot [mm/s^2]. Default 800",
			"maxVelDecel: int32" => "Set maximum translation deceleration of robot [mm/s^2]. Negative value. Default -500",
			"maxRotVel: int32" => "Set maximum rotation velocity of robot [deg/s]. Default 50",
			"maxRotVelAcc: int32" => "Set maximum rotation acceleration of robot [deg/s^2]. Default 500",
			"maxRotVelDecel: int32" => "Set maximum rotation deceleration of robot [deg/s^2]. Negative value. Default -500",
		),
		/*
		"baseposition" => array(
			"push_interval_msec: int32" => "Interval for pushing the base state [msec]. Default 100",
		),
		*/
		/*
		"group" => array(
			"" => "",
		),
		*/
	), // Ini-Configuration

	


);
?>
