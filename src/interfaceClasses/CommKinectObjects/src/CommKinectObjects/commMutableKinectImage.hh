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

#ifndef _COMM_COMMKINECTOBJECTS_COMMMUTABLEKINECTIMAGE_HH
#define _COMM_COMMKINECTOBJECTS_COMMMUTABLEKINECTIMAGE_HH

#include <string>

#include "commKinectImage.hh"

// include files of classes

// include header-files of included communication objects

// include enums


// include client-side file generated by IDL compiler

namespace CommKinectObjects {

class CommMutableKinectImage: public CommKinectImage {
public:
	CommMutableKinectImage() {
	}

	inline CommMutableKinectImage(const CommMutableKinectImage &source) :
		CommKinectImage(source) {
	}

	inline CommMutableKinectImage &operator=(const CommMutableKinectImage &source) {
		if (&source != this)
			CommKinectImage::operator=(source);
		return *this;
	}

	virtual ~CommMutableKinectImage() {
	}

	inline void get(CORBA::Any &a) const {
		Smart::CommSharedMemoryBase::get(a);
	}

	inline void set(const CORBA::Any &a) {
		Smart::CommSharedMemoryBase::set(a);
	}

	static inline std::string identifier(void) {
		return "CommKinectObjects::CommMutableKinectImage";
	}

	///////////////////////////////////////////////////////
	// user interfaces
	///////////////////////////////////////////////////////


	/**
	 * Try to lock the shared memory segment and get the reference counter.
	 * This method can be used to find out if some other process or thread is accessing
	 * this image, which is the case if you fail to get the reference counter or if the
	 * counter is greater than one.
	 *
	 * We won't allow locking of images for an extended period of time,
	 * since we have no way of deblocking the shared memory usage for example for
	 * component configuration changes or component shutdown. Use the default
	 * SmartSoft communication to synchronize access to an image instance.
	 */
	inline bool try_get_ref_count(unsigned int &n) {
		if (CommSharedMemoryBase::trylock() == 0) {
			n = CommSharedMemoryBase::get_ref_count();
			CommSharedMemoryBase::unlock();
			return true;
		}
		return false;
	}

	/**
	 * Set the pose of the sensor relative to the robot coordinate system.
	 */
	inline void set_sensor_pose(const Smart::CommPose3d& pose) {
		reinterpret_cast<KinectImageParameters*> (shm)->sensor_pose.set_idl(pose.get_idl());
	}

	/**
	 * Set the state of the base.
	 */
	inline void set_base_state(const Smart::CommBaseState& state) {
		reinterpret_cast<KinectImageParameters*> (shm)->base_state.set_idl(state.get_idl());
	}

	/**
	 * Set the minimal distance the sensor works.
	 */
	inline void set_min_distance(const double_t value, const double_t unit = 0.001) {
		reinterpret_cast<KinectImageParameters*> (shm)->min_distance = value * unit * 1000;
	}

	/**
	 * Set the maximal distance the sensor works.
	 */
	inline void set_max_distance(const double_t value, const double_t unit = 0.001) {
		reinterpret_cast<KinectImageParameters*> (shm)->max_distance = value * unit * 1000;
	}

	/**
	 * Set the opening angle of the camera in x-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline void set_opening_angle_x_axis(const double_t value) {
		reinterpret_cast<KinectImageParameters*> (shm)->opening_angle_x_axis = value;
	}

	/**
	 * Set the opening angle of the camera in y-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline void set_opening_angle_y_axis(const double_t value) {
		reinterpret_cast<KinectImageParameters*> (shm)->opening_angle_y_axis = value;
	}

	/**
	 * Get the intrinsic matrix of the color camera.
	 */
	inline void set_color_intrinsic(const arma::mat& mat) const {

		if (mat.n_cols != 4 && mat.n_rows != 4) {
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i) {
			for (size_t j = 0; j < 4; j++) {
				reinterpret_cast<KinectImageParameters*> (shm)->color_intinsic[i][j] = mat(i, j);
			}
		}
	}

	/**
	 * Get the intrinsic matrix of the distance camera.
	 */
	inline void set_distance_intrinsic(const arma::mat& mat) const {

		if (mat.n_cols != 4 && mat.n_rows != 4) {
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i) {
			for (size_t j = 0; j < 4; j++) {
				reinterpret_cast<KinectImageParameters*> (shm)->dist_intinsic[i][j] = mat(i, j);
			}
		}

	}

	/**
	 * Get the extrinsic matrix between the cameras.
	 */
	inline void set_camera_extrinsic(const arma::mat& mat) const {

		if (mat.n_cols != 4 && mat.n_rows != 4) {
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i) {
			for (size_t j = 0; j < 4; j++) {
				reinterpret_cast<KinectImageParameters*> (shm)->camera_extrinsic[i][j] = mat(i, j);
			}
		}

	}

	/**
	 * Set the valid flag of the data.
	 * The flag is automatically set to true if data is written to the image.
	 * Thus the server either writes data to the image or sets the valid.
	 */
	void set_data_valid(bool value) {
		if (shm != 0) {
			reinterpret_cast<KinectImageParameters*> (shm)->data_valid = value;
		}
	}

	/**
	 * Set the sequence counter of the image.
	 */
	void set_sequence_counter(ulong n) {
		if (shm != 0) {
			reinterpret_cast<KinectImageParameters*> (shm)->seq_count = n;
		}
	}

	/**
	 * Returns a pointer to rgb image
	 * The image has the size return from get_size().
	 */
	inline uint8_t* get_rgb_image() const {
		return reinterpret_cast<uint8_t*> (shm) + sizeof(KinectImageParameters) / sizeof(uint8_t);
	}

	/**
	 * Returns a pointer to the first element of the coordinate image.
	 * The image has the size return from get_image_size().
	 */
	inline float* get_distances() const {
		return reinterpret_cast<float*> (shm) + (sizeof(KinectImageParameters) + get_color_image_size())
				/ sizeof(float);
	}

	/**
	 * Copy data from the memory chunk at \a data to the internal shared memory.
	 * The number of bytes to copy is derived from the internal format and size descriptions.
	 */
	void set_data(const void* data) {
		if (shm != 0 && data != 0) {
			memcpy(shm, data, get_size());
		}
	}

};

}
#endif