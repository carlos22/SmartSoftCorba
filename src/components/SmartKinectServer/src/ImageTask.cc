//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.8.0
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

#include "ImageTask.hh"
#include "gen/SmartKinectServer.hh"

#include "EulerTransformationMatrices.hh"

#include <iostream>
#include <armadillo.hh>

ImageTask::ImageTask() {
	_ring_buffer_index = 0;
	_cleanupThread = new ImageCleanupThread();
	_cleanupThread->open();
	device = NULL;
}

ImageTask::~ImageTask() {
	COMP->imageTask.close();
	_cleanupThread->stopThread();
	for (unsigned int i = 0; i < _ring_buffer.size() - 2; i++) {
		delete _ring_buffer[i];
	}
	_ring_buffer.clear();
	delete _cleanupThread;
}

void ImageTask::startCapturing() {
	CHS::SmartGuard guard(kinectMutex);
	if (device != NULL) {
		device->startVideo();
		device->startDepth();
	}
}
void ImageTask::stopCapturing() {
	CHS::SmartGuard guard(kinectMutex);
	if (device != NULL) {
		device->stopVideo();
		device->stopDepth();
	}
}

int ImageTask::svc() {

	std::cout << "[Image Task] Starting up camera ...\n";
	Freenect::Freenect<KinectWrapper> freenect;
	device = &freenect.createDevice(0);

	Smart::CommBasePosition default_base_position;
	default_base_position.set_x(COMP->ini.base.x);
	default_base_position.set_y(COMP->ini.base.y);
	default_base_position.set_z(COMP->ini.base.z);
	default_base_position.set_base_alpha(COMP->ini.base.base_a);
	default_base_position.set_steer_alpha(COMP->ini.base.steer_a);
	default_base_position.set_turret_alpha(COMP->ini.base.turret_a);

	Smart::CommBaseVelocity zero_velocity;
	zero_velocity.set_v(0);
	zero_velocity.set_omega_base(0);
	zero_velocity.set_omega_steer(0);
	zero_velocity.set_omega_turret(0);

	// Calculate size of the ring_buffer
	int size = (int) (COMP->ini.component.valid_image_time + 1);

	_ring_buffer.resize(size);

	if (COMP->ini.component.debug_info) {
		std::cout << "[Image Task] Newest: Size of Ring Buffer " << size << std::endl;
	}

	// Fill ring_buffer with empty images
	for (unsigned int i = 0; i < _ring_buffer.size(); i++) {
		_ring_buffer[i] = new CommKinectObjects::CommResizeableKinectImage(COMP->ini.hardware_properties.width,
				COMP->ini.hardware_properties.height);
	}

	CommKinectObjects::CommMutableKinectImage* image = NULL;
	unsigned int ref_count = 1;

	std::cout << "[Image Task] Start image capturing ...\n";

	while (true) {
		try {
			COMP->stateServer->acquire("active");
			COMP->stateServer->release("active");

			image = _ring_buffer[_ring_buffer_index];

			ref_count = 1;
			if ((!image->try_get_ref_count(ref_count)) || (ref_count > 1)) {

				// image is still accessed by some client
				_cleanupThread->remove_reference_to(image); // directly removing could block!
				image = new CommKinectObjects::CommResizeableKinectImage(COMP->ini.hardware_properties.width,
						COMP->ini.hardware_properties.height);
				_ring_buffer[_ring_buffer_index] = image;
			}

			// get newest image from Kinect
			if (COMP->componentActive) {
				CHS::SmartGuard guard(kinectMutex);
				device->getImage(*image);
				guard.release();

				image->set_min_distance(COMP->ini.hardware_properties.min_distance);
				image->set_max_distance(COMP->ini.hardware_properties.max_distance);

				arma::mat sensorMat;
				EulerTransformationMatrices::create_zyx_matrix(COMP->ini.sensor_pose.x, COMP->ini.sensor_pose.y,
						COMP->ini.sensor_pose.z, COMP->ini.sensor_pose.azimuth, COMP->ini.sensor_pose.elevation,
						COMP->ini.sensor_pose.roll, sensorMat);

				// set base state
				Smart::CommBaseState base_state;

				// read base state from base server
				if (COMP->ini.base.on_base) {
					CHS::StatusCode status = COMP->basePushTimedClient->getUpdate(base_state);

					if (status != CHS::SMART_OK) {
						std::cerr << "[Image Task] WARNING: failed to get current base state ("
								<< CHS::StatusCodeConversion(status) << "), pushing invalid image" << std::endl;
						image->set_data_valid(false);
					}
				}
				// default base state
				else {
					base_state.set_time_stamp(Smart::CommTimeStamp::now());
					base_state.set_base_position(default_base_position);
					base_state.set_base_velocity(zero_velocity);
				}

				Smart::CommPose3d sensorPose(sensorMat);
				image->set_sensor_pose(sensorPose);
				image->set_base_state(base_state);
			}

			// Save the newest image into the global pointer
			COMP->NewestImageMutex.acquire();
			COMP->newestImage = image;
			COMP->NewestImageMutex.release();

			if (COMP->ini.component.push_newest_active) {
				COMP->imagePushNewestServer->put(*image);
				if (COMP->ini.component.debug_info) {
					std::cout << "[Image Task] Newest: Image sent.\n";
				}
			}

			// calculate new index in the ring_buffer
			_ring_buffer_index++;
			if (_ring_buffer_index == _ring_buffer.size())
				_ring_buffer_index = 0;
		} catch (std::exception e) {
			std::cerr << "[Image Task] Error in ImageTask::svc() " << e.what() << "\n";
		} catch (...) {
			std::cerr << "[Image Task] Unknown error in ImageTask::svc()\n";
		}

		smart_task_wait_period();
	}

	return 0;
}

//----------------------------------------------------------------
// ImagePushNewestCleanupThread - Methods
//----------------------------------------------------------------

ImageTask::ImageCleanupThread::ImageCleanupThread() {
	_queue_sem.acquire();
}

int ImageTask::ImageCleanupThread::svc() {
	CommKinectObjects::CommKinectImage *img = 0;
	while (true) {
		_queue_sem.acquire();
		_queue_mutex.acquire();
		img = _queue.front();
		_queue.pop();
		_queue_mutex.release();
		if (img == 0) {
			if (COMP->ini.component.debug_info)
				std::cout << "[Image Task] Newest-CleanUpThread: Stopping SVC\n";
			break; // "0" means stop, can be done over the method stopThread()
		}
		delete img; // may block on semaphore protecting shm
		if (COMP->ini.component.debug_info)
			std::cout << "[Image Task] Newest-CleanUpThread: Image deleted\n";
	}
	return 0;
}

void ImageTask::ImageCleanupThread::_enqueue(CommKinectObjects::CommKinectImage *image) {
	_queue_mutex.acquire();
	_queue.push(image);
	_queue_mutex.release();
	_queue_sem.release();
}

ImageTask::ImageCleanupThread::~ImageCleanupThread() {
}