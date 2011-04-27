//------------------------------------------------------------------------
//
//  Copyright (C) 2011 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of "CommKinectServer".
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//--------------------------------------------------------------------------


#ifndef KINECTWRAPPER_HH_
#define KINECTWRAPPER_HH_

#include <iostream>
#include <stdint.h>
#include <math.h>
#include <vector>

#include "libfreenect.hpp"

#include "CommVisionObjects/commKinectImage.hh"

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

	void getImage(CommVisionObjects::CommKinectImage& image);

private:
	void calculate_range2meters();

};

#endif /* KINECTWRAPPER_HH_ */
