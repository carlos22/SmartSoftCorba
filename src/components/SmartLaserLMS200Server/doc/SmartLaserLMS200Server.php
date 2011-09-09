<?php
/**********************************************************************************
  SmartSoft Component Description File

  Author:	Dennis Stampfer
  Created:	16.9.2010

  Last changed (for matching with codebase): 
  			27.4.2011 / mdsd

**********************************************************************************/


$components[] = array(
	"Name" => "SmartLaserLMS200Server",

	"Description" => "The SmartLaserLMS200Server makes laser scans from SICK LMS 200 and PLS langer rangers
		available. Scans can be requested by push newest or query communication.

		<p>GPL-License: includes <a
		href='http://playerstage.sourceforge.net/doc/Player-2.1.0/player/group__driver__sicklms200.html'>Code
		from the Player Project</a>.",




	"Metadata" => array(
		"Purpose" => "Hardware-Driver",
		"License" => "GPL",
		"Hardware" => "SICK LMS 200 and PLS",

		/* Library dependencies other than SmartSoft-Framework */
		"Library Dependencies" => "smartSickInterface (included in SmartSoft's utilities)",
		"SVN" => "http://smart-robotics.svn.sourceforge.net/viewvc/smart-robotics/trunk/smartsoft/src/components/SmartLaserLMS200Server/",
	), //Metadata




/*
	"States" => array(
		"state1" => "descr of state 1",
	), // States


	"Parameters" => array(
		"PARAM1" => "descr of param1",
	), // Parameters
*/




	"Client-Ports" => array(
		"baseClient" => array(
			"Type" => "SmartPushTimedClient",
			"Metadata" => array(
				"interval" => "1",
				"commObject" => "CommBaseState",
			),
			"Description" => "Typically connected to the robot
				base (e.g. SmartPioneerBaseServer) to receive the base state which is
				included in the laser scan. Whether this port is connected or not can be
				configured via the ini file. Used to stamp every laser scan with the base state
				where the scan was recorded, if laser is mounted on a mobile platform. If
				this port is not connected, the pose where recorded will contain the values 
				configured in the ini file.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //laserQueryServer
	), // Client-Ports




	"Server-Ports" => array(
		"laserQueryServer" => array(
			"Type" => "SmartQueryServer",
			"Metadata" => array(
				"serviceName" => "scan",
				"commRequestObject" => "CommVoid",
				"commAnswerObject" => "CommMobileLaserScan",
			),
			"Description" => "Query to request the latest laser scan. Scan will be
				stamped invalid if it was not possible to get the base state but the baseClient
				port is connected. ",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //laserQueryServer

		"laserServer" => array(
			"Type" => "SmartPushNewestServer",
			"Metadata" => array(
				"serviceName" => "scan",
				"commObject" => "CommMobileLaserScan",
			),
			"Description" => "Push latest laser scan. The rate with which the server pushes depends on the
				parametrization of the laser scanner, especially baudrate and
				resolution. A typical rate is 38 Hz (500000 bps, 0.5 degree resolution). Scan
				will be stamped invalid if it was not possible to get the base state but the
				baseClient port is connected.",
			/*
			"States" => array(
				"state1" => "Descr state 1"
			)
			*/
		), //laserQueryServer
	), // Server-Ports


	"Ini-Configuration" => array(
		"laser" => array(
			"sick_type: string" => "Type of SICK Scanner. Possible values: LMS and PLS. Default: LMS",
			"on_turrent: boolean" => "Does the scanner rotate with the turrent (true) or is it fixed with the base (false)? Default: true",
			"x, y, z, azimuth: double" => "The position of the scanner on the
				robot. The azimuth is relative to the perspective mounting point
				(turrent or base) on the robot. Units in [mm] and [rad]. Default: 0",
			"device: string" => "Serial device of the laser scanner. Default: /dev/ttyS0.",
			"baudrate: int32" => "Speed for communication [bps]. Possible values: 9600, 38400 and 500000. Default: 38400",
			"resolution: int32" => "The angular resolution [0.01 degree].
				Possible Values are 25 (0.25 degree), 50 (0.5 degree) and 100 (1
				degree). However, in 0.25 degree mode, only 100 degree scans
				are reported. This value is directly passed to the SICK device. Default: 50",
			"length_unit: int32" => "Length unit of reported distances [mm].
				Possible values are 1, 10. Corresponding maximum distances are
				8m, 80m. This value is directly passed to the SICK device. Default: 1",
			"verbose: boolean" => "Used when debugging the laser interface. Default: false",
		),
		"base" => array(
			"subscribe: boolean" => "Subscribe to base server for position updates (true)? Default: true",
			"x, y, z: double" => "Use fixed values x, y, z [mm] for pose of the scanner if not subscribed to base server. See server ports. Default: 0",
			"base_a: double" => "Use fixed value base_a [rad] for pose of the scanner if not subscribed to base server. See server ports. Default: 0",
			"steer_a: double" => "Use fixed value for steer_a [rad] for pose of the scanner if not subscribed to base server. See server ports. Default: 0",
			"turrent_a: double" => "Use fixed value for turrent_a [rad] for pose of the scanner if not subscribed to base server. See server ports. Default: 0",
		),
		/*
		"group" => array(
			"" => "",
		),
		*/
	), // Ini-Configuration

	


);
?>
