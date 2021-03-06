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

#ifndef _SMARTPMDSERVER_HH
#define _SMARTPMDSERVER_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartPMDServerCore.hh"
// include communication objects
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commDevicePoseState.hh>
#include <CommBasicObjects/commVoid.hh>
#include <CommVisionObjects/commToFImage.hh>
#include <CommVisionObjects/commToFParameter.hh>
// include tasks	

#include "../ImageTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../ImageQueryHandler.hh"
#include "ImageTaskTimer.hh"
#include "../ParameterHandler.hh"
#include "../StateChangeHandler.hh"

#define COMP SmartPMDServer::instance()

class SmartPMDServer: public SmartPMDServerCore
{
private:
	static SmartPMDServer _smartPMDServer;

	// constructor
	SmartPMDServer();

	// copy-constructor
	SmartPMDServer(const SmartPMDServer& cc);

	// destructor
	~SmartPMDServer()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	ImageQueryHandler imageQueryHandler;
	ImageTaskTimer imageTaskTimer;
	ParameterHandler parameterHandler;
	StateChangeHandler stateChangeHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex
	CHS::SmartMutex ImageTaskMutex;
	CHS::SmartMutex NewestImageMutex;

	// create condition mutex
	CHS::SmartConditionMutex ImageTaskCondMutex;//(ImageTaskMutex);


	// instantiate tasks

	ImageTask imageTask;

	// ports
	CHS::PushTimedClient<CommBasicObjects::CommBaseState> *basePushTimedClient;
	CHS::PushNewestServer<CommVisionObjects::CommToFImage>
			*imagePushNewestServer;
	CHS::QueryServer<CommBasicObjects::CommVoid,
			CommVisionObjects::CommToFImage> *imageQueryServer;
	CHS::SendServer<CommVisionObjects::CommToFParameter> *paramServer;
	CHS::PushNewestClient<CommBasicObjects::CommDevicePoseState>
			*ptuDeviceStateServer;
	CHS::SmartStateServer *stateServer;
	CHS::WiringSlave *wiringSlave;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartPMDServer* instance()
	{
		return (SmartPMDServer*) &_smartPMDServer;
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

		struct ini_imagePushNewestServer
		{
			std::string serviceName;
		} imagePushNewestServer;

		struct ini_imageQueryServer
		{
			std::string serviceName;
		} imageQueryServer;

		struct ini_paramServer
		{
			std::string serviceName;
		} paramServer;

		struct ini_stateServer
		{
			std::string serviceName;
		} stateServer;

		struct ini_basePushTimedClient
		{
			std::string serverName;
			std::string serviceName;
			long interval;
		} basePushTimedClient;

		struct ini_ptuDeviceStateServer
		{
			std::string serverName;
			std::string serviceName;
		} ptuDeviceStateServer;

		struct ini_base
		{

			double base_a;

			bool on_base;

			bool on_ptu;

			double steer_a;

			double turret_a;

			long x;

			long y;

			long z;
		} base;

		struct ini_general
		{

			bool debug_info;

			bool initialize_camera;

			std::string processing_parameter;

			std::string processing_plugin;

			std::string source_parameter;

			std::string source_plugin;

			/**
			 * The time a image is available when sent by the pushNewest. [sec]
			 */
			double valid_image_time;
		} general;

		struct ini_hardware_properties
		{

			bool activate_lense_calibration;

			bool activate_suppress_motion_blur;

			unsigned long height;

			/**
			 * The PMD[vision][ CamCube supports integration times between 12 and 50000 microseconds.
			 */
			unsigned long integration_time;

			/**
			 * The maximal distance the sensor can work. [mm]
			 */
			double max_distance;

			/**
			 * The minimal distance the sensor can work. [mm]
			 */
			double min_distance;

			/**
			 * Currently, the PMD[vision] CamCube can operate at a fixed modulation frequencies of 18 MHz, 19 MHz, 20 MHz and 21 MHz.
			 */
			unsigned long modulation_frequency;

			double opening_angle_x_axis;

			double opening_angle_y_axis;

			unsigned long width;
		} hardware_properties;

		struct ini_sensor_pose
		{

			double azimuth;

			double elevation;

			double roll;

			long x;

			long y;

			long z;
		} sensor_pose;
	} ini;

};
#endif
