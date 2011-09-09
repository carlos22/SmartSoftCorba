//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2.1
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this library; if not, write to the Free Software Foundation, Inc.,
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#include "ImageTask.hh"
#include "gen/SmartPMDServer.hh"

#include "EulerTransformationMatrices.hh"

#include <iostream>
#include <armadillo.hh>

ImageTask::ImageTask() {
	_ring_buffer_index = 0;
	_ring_buffer.resize(2);

}

int ImageTask::svc() {

	std::cout << "[Image Task] Starting up camera ...\n";

	// Fill ring_buffer with empty images
	for (unsigned int i = 0; i < _ring_buffer.size(); i++) {
		_ring_buffer[i] = new CommVisionObjects::CommToFImage(
				COMP->ini.hardware_properties.width,
				COMP->ini.hardware_properties.height);
	}

	CommVisionObjects::CommToFImage* image = NULL;

	std::cout << "[Image Task] Start image capturing ...\n";

	while (true) {

		if (COMP->stateServer->acquire("pushimage") == CHS::SMART_OK) {

			try {
				image = _ring_buffer[_ring_buffer_index];

				COMP->pmd.getImage(*image);

				setState(*image);

				// Save the newest image into the global pointer
				COMP->NewestImageMutex.acquire();
				COMP->newestImage = image;
				COMP->NewestImageMutex.release();

				//if (COMP->ini.component.push_newest_active) {
				COMP->imagePushNewestServer->put(*image);
				if (COMP->ini.general.debug_info) {
					std::cout << "[Image Task] Newest: Image sent.\n";
				}
				//}


				// calculate new index in the ring_buffer
				_ring_buffer_index++;
				if (_ring_buffer_index == _ring_buffer.size())
					_ring_buffer_index = 0;

			} catch (std::exception e) {
				std::cerr << "[Image Task] Error in ImageTask::svc() "
						<< e.what() << "\n";
			} catch (...) {
				std::cerr << "[Image Task] Unknown error in ImageTask::svc()\n";
			}

			COMP->stateServer->release("pushimage");

		} // if

		// wait for next period
		smart_task_wait_period();
	} //while

	return 0;
}

ImageTask::~ImageTask() {
	COMP->imageTask.close();
	for (unsigned int i = 0; i < _ring_buffer.size() - 2; i++) {
		delete _ring_buffer[i];
	}
	_ring_buffer.clear();
}

void ImageTask::setState(CommVisionObjects::CommToFImage &image) {

	arma::mat sensorMat;
	EulerTransformationMatrices::create_zyx_matrix(COMP->ini.sensor_pose.x, COMP->ini.sensor_pose.y,
			COMP->ini.sensor_pose.z, COMP->ini.sensor_pose.azimuth, COMP->ini.sensor_pose.elevation,
			COMP->ini.sensor_pose.roll, sensorMat);


	// set base state
	CommBasicObjects::CommBaseState base_state;
	if (COMP->ini.base.on_ptu) {
		CommBasicObjects::CommDevicePoseState devicePoseState;
		CHS::StatusCode status = COMP->ptuDeviceStateServer->getUpdate(devicePoseState);

		base_state = devicePoseState.get_base_state();
		CommBasicObjects::CommPose3d ptuPose = devicePoseState.get_device_pose3d_robot();
		arma::mat ptuMat = ptuPose.getHomogeneousMatrix();

		sensorMat = ptuMat * sensorMat;

		if (status != CHS::SMART_OK) {
			std::cerr << "[Image Task] WARNING: failed to get current ptu device state ("
					<< CHS::StatusCodeConversion(status) << "), pushing invalid image" << std::endl;
			image.set_data_valid(false);
		}

	}

	// read base state from base server
	else if (COMP->ini.base.on_base) {
		CHS::StatusCode status = COMP->basePushTimedClient->getUpdate(base_state);

		if (status != CHS::SMART_OK) {
			std::cerr << "[Image Task] WARNING: failed to get current base state ("
					<< CHS::StatusCodeConversion(status) << "), pushing invalid image" << std::endl;
			image.set_data_valid(false);
		}
	}

	// default base state
	else {
		CommBasicObjects::CommBasePosition default_base_position;
		default_base_position.set_x(COMP->ini.base.x);
		default_base_position.set_y(COMP->ini.base.y);
		default_base_position.set_z(COMP->ini.base.z);
		default_base_position.set_base_alpha(COMP->ini.base.base_a);
		default_base_position.set_steer_alpha(COMP->ini.base.steer_a);
		default_base_position.set_turret_alpha(COMP->ini.base.turret_a);

		CommBasicObjects::CommBaseVelocity zero_velocity;
		zero_velocity.set_v(0);
		zero_velocity.set_omega_base(0);
		zero_velocity.set_omega_steer(0);
		zero_velocity.set_omega_turret(0);


		base_state.set_time_stamp(CommBasicObjects::CommTimeStamp::now());
		base_state.set_base_position(default_base_position);
		base_state.set_base_velocity(zero_velocity);
	}

	CommBasicObjects::CommPose3d sensorPose(sensorMat);
	image.set_sensor_pose(sensorPose);
	image.set_base_state(base_state);
}
