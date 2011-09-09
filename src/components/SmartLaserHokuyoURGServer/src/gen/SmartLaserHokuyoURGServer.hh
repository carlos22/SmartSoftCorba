//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.4
// The SmartSoft Toolchain has been developed by:
//  
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
// 
// Please do not modify this file. It will be re-generated
// running the code generator.
//--------------------------------------------------------------------------

#ifndef _SMARTLASERHOKUYOURGSERVER_HH
#define _SMARTLASERHOKUYOURGSERVER_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartLaserHokuyoURGServerCore.hh"
// include communication objects
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commMobileLaserScan.hh>
#include <CommBasicObjects/commVoid.hh>
#include <CommManipulatorObjects/commMobileManipulatorState.hh>
// include tasks	

#include "../LaserTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../LaserQueryHandler.hh"
#include "../PushTimedHandler.hh"
#include "../StateChangeHandler.hh"

#define COMP SmartLaserHokuyoURGServer::instance()

class SmartLaserHokuyoURGServer: public SmartLaserHokuyoURGServerCore
{
private:
	static SmartLaserHokuyoURGServer _smartLaserHokuyoURGServer;

	// constructor
	SmartLaserHokuyoURGServer();

	// copy-constructor
	SmartLaserHokuyoURGServer(const SmartLaserHokuyoURGServer& cc);

	// destructor
	~SmartLaserHokuyoURGServer()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	LaserQueryHandler laserQueryHandler;
	PushTimedHandler pushTimedHandler;
	StateChangeHandler stateChangeHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex ScanLock;

	// create condition mutex


	// instantiate tasks

	LaserTask laserTask;

	// ports
	CHS::PushTimedClient<CommBasicObjects::CommBaseState> *baseTimedClient;
	CHS::PushNewestServer<CommBasicObjects::CommMobileLaserScan>
			*laserPushNewestServer;
	CHS::PushTimedServer<CommBasicObjects::CommMobileLaserScan>
			*laserPushTimedServer;
	CHS::QueryServer<CommBasicObjects::CommVoid,
			CommBasicObjects::CommMobileLaserScan> *laserQueryServer;
	CHS::PushTimedClient<CommManipulatorObjects::CommMobileManipulatorState>
			*manipulatorTimedClient;
	CHS::SmartStateServer *stateServer;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartLaserHokuyoURGServer* instance()
	{
		return (SmartLaserHokuyoURGServer*) &_smartLaserHokuyoURGServer;
	}

	// ini parameter
	struct ini_ini
	{

		// component struct
		struct ini_component
		{
			// the name of the component
			std::string name;
		} component;

		struct ini_laserPushNewestServer
		{
			std::string serviceName;
		} laserPushNewestServer;

		struct ini_laserPushTimedServer
		{
			std::string serviceName;
			double cycle;
		} laserPushTimedServer;

		struct ini_laserQueryServer
		{
			std::string serviceName;
		} laserQueryServer;

		struct ini_stateServer
		{
			std::string serviceName;
		} stateServer;

		struct ini_baseTimedClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} baseTimedClient;

		struct ini_manipulatorTimedClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} manipulatorTimedClient;

		struct ini_base_manipulator
		{

			/**
			 * The default base azimuth angle [rad] if on_base = false. Default = 0
			 */
			double base_a;

			/**
			 * If the laser ranger is mounted on a robot set it to true. Default = true
			 */
			bool on_base;

			/**
			 * If the laser ranger is mounted on a manipulator set it to true. Default = false
			 */
			bool on_manipulator;

			/**
			 * If the laser ranger is mounted on a robot turret set it to true. Default = false
			 */
			bool on_turret;

			/**
			 * The default base steer angle [rad] if on_base = false. Default = 0
			 */
			double steer_a;

			/**
			 * The default turret angle [rad] if on_base = false. Default = 0
			 */
			double turret_a;

			/**
			 * The default base x position [mm] if on_base = false. Default = 0
			 */
			long x;

			/**
			 * The default base y position [mm] if on_base = false. Default = 0
			 */
			long y;

			/**
			 * The default base z position [mm] if on_base = false. Default = 0
			 */
			long z;
		} base_manipulator;

		struct ini_scanner
		{

			/**
			 * The azimuth angle [rad] of the scanner on the robot, etc. Default = 0
			 */
			double azimuth;

			/**
			 * The device for the laser ranger.
			 */
			std::string device;

			/**
			 * The elevation angle [rad] of the scanner on the robot, etc. Default = 0
			 */
			double elevation;

			/**
			 * The max distance [mm] of allowed measurements. Default = 5600
			 */
			unsigned long max_range;

			/**
			 * The min distance [mm] of allowed measurements. Default = 20
			 */
			unsigned long min_range;

			/**
			 * The opening angle [1/100 deg] which should be used. It is possible to give a smaller value than the laser ranger has. Default = 24000
			 */
			unsigned long opening_angle;

			/**
			 * The roll angle [rad] of the scanner on the robot, etc. Default = 0
			 */
			double roll;

			/**
			 * Speed for communication (bps). Default = 115200
			 */
			unsigned long speed;

			/**
			 * The type of the connected laser ranger. Supported types are URG-04LX, URG-04LX-UG01,  UBG-05LX-F01, UHG-08LX, UTM-30LX .Default = URG-04LX.
			 */
			std::string type;

			/**
			 * Set the component to verbose. Default = false
			 */
			bool verbose;

			/**
			 * The x position [mm] of the scanner on the robot, etc. Default = 0
			 */
			long x;

			/**
			 * The y position [mm] of the scanner on the robot, etc. Default = 0
			 */
			long y;

			/**
			 * The z position [mm] of the scanner on the robot, etc. Default = 0
			 */
			long z;
		} scanner;

		struct ini_services
		{

			/**
			 * If true, the push newest service is activated. Default = true
			 */
			bool activate_push_newest;

			/**
			 * If true, the push timed service is activated. Default = true
			 */
			bool activate_push_timed;
		} services;
	} ini;

};
#endif
