/*
 * KinectWrapper.h
 *
 *  Created on: Dec 8, 2010
 *      Author: wopfner
 */

#ifndef KINECTWRAPPER_HH_
#define KINECTWRAPPER_HH_

#include <iostream>
#include <stdint.h>
#include <math.h>
#include <vector>

#include "libfreenect.hpp"

#include "CommKinectObjects/commMutableKinectImage.hh"

class KinectWrapper: public Freenect::FreenectDevice {

private:
	class Mutex {
	public:
		Mutex() {
			pthread_mutex_init(&m_mutex, NULL);
		}

		void lock() {
			pthread_mutex_lock(&m_mutex);
		}

		void unlock() {
			pthread_mutex_unlock(&m_mutex);
		}
	private:
		pthread_mutex_t m_mutex;
	};

private:
	std::vector<float> m_buffer_depth;
	std::vector<uint8_t> m_buffer_video;
	std::vector<float> m_range2meters;

	Mutex m_rgb_mutex;
	Mutex m_depth_mutex;

	bool m_new_rgb_frame;
	bool m_new_depth_frame;

	bool debug;

	ulong imageCounter;

	arma::mat colorIntrinsic;
	arma::mat distIntrinsic;
	arma::mat extrinsicMat;

public:
	KinectWrapper(freenect_context *_ctx, int _index);

	virtual ~KinectWrapper();

	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);

	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp);

	bool getRGB(std::vector<uint8_t> &buffer);
	bool getDepth(std::vector<float> &buffer);

	void getImage(CommKinectObjects::CommMutableKinectImage& image);

private:
	void calculate_range2meters();

};

#endif /* KINECTWRAPPER_HH_ */
