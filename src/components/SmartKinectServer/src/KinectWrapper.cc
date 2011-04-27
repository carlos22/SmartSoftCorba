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

#include "KinectWrapper.hh"

KinectWrapper::KinectWrapper(freenect_context *_ctx, int _index) :
	Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_FRAME_PIX),
			m_buffer_video(FREENECT_VIDEO_RGB_SIZE), m_range2meters(2048), m_new_rgb_frame(false), m_new_depth_frame(
					false) {

	debug = false;

	calculate_range2meters();
	imageCounter = 0;

	// create matrixes
	colorIntrinsic.set_size(4, 4);
	colorIntrinsic.zeros();

	distIntrinsic.set_size(4, 4);
	distIntrinsic.zeros();

	distIntrinsic(0, 0) = 5.9421434211923247e+02;
	distIntrinsic(1, 1) = 5.9104053696870778e+02;
	distIntrinsic(0, 2) = 3.3930780975300314e+02;
	distIntrinsic(1, 2) = 2.4273913761751615e+02;
	distIntrinsic(2, 2) = 1;
	distIntrinsic(3, 3) = 1;

	extrinsicMat.set_size(4, 4);
	extrinsicMat.zeros();
	//	extrinsicMat << 5.34866271e+02 << 3.89654806e+00 << 0.00000000e+00 << 1.74704200e-02 << arma::endr;
	//	extrinsicMat << -4.70724694e+00 << -5.28843603e+02 << 0.00000000e+00 << -1.22753400e-02 << arma::endr;
	//	extrinsicMat << -3.19670762e+02 << -2.60999685e+02 << 0.00000000e+00 << -9.99772000e-01 << arma::endr;
	//	extrinsicMat << -6.98445586e+00 << 3.31139785e+00 << 0.00000000e+00 << 1.09167360e-02 << arma::endr;
}

KinectWrapper::~KinectWrapper() {
}

void KinectWrapper::calculate_range2meters() {

	const float k1 = 1.1863f;
	const float k2 = 2842.5f;
	const float k3 = 0.1236f;

	for (size_t i = 0; i < m_range2meters.size(); i++) {
		m_range2meters[i] = k3 * tanf(i / k2 + k1);
	}

	// Minimum/Maximum range means error:
	m_range2meters[0] = 0;
	m_range2meters[m_range2meters.size() - 1] = 0;
}

void KinectWrapper::VideoCallback(void* _rgb, uint32_t timestamp) {
	if (debug) {
		std::cout << "RGB callback" << std::endl;
	}

	m_rgb_mutex.lock();
	uint8_t* rgb = static_cast<uint8_t*> (_rgb);
	std::copy(rgb, rgb + FREENECT_VIDEO_RGB_SIZE, m_buffer_video.begin());
	m_new_rgb_frame = true;
	m_rgb_mutex.unlock();
}

void KinectWrapper::DepthCallback(void* _depth, uint32_t timestamp) {
	if (debug) {
		std::cout << "Depth callback" << std::endl;
	}

	m_depth_mutex.lock();
	uint16_t* depth = static_cast<uint16_t*> (_depth);
	for (unsigned int i = 0; i < FREENECT_FRAME_PIX; i++) {
		m_buffer_depth[i] = m_range2meters[depth[i]];
	}
	m_new_depth_frame = true;
	m_depth_mutex.unlock();
}

bool KinectWrapper::getRGB(std::vector<uint8_t> &buffer) {
	m_rgb_mutex.lock();
	if (m_new_rgb_frame) {
		buffer.swap(m_buffer_video);
		m_new_rgb_frame = false;
		m_rgb_mutex.unlock();
		return true;
	} else {
		m_rgb_mutex.unlock();
		return false;
	}
}

bool KinectWrapper::getDepth(std::vector<float> &buffer) {
	m_depth_mutex.lock();
	if (m_new_depth_frame) {
		buffer.swap(m_buffer_depth);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	} else {
		m_depth_mutex.unlock();
		return false;
	}
}

void KinectWrapper::getImage(CommVisionObjects::CommKinectImage& image) {

	///////////////////////////////////////
	// set images
	///////////////////////////////////////
	std::vector<float> depth(FREENECT_FRAME_PIX);
	std::vector<uint8_t> rgb(FREENECT_VIDEO_RGB_SIZE);

	if (getDepth(depth) && getRGB(rgb)) {
		image.set_distances(depth);
		image.set_rgb_image(rgb);

		image.set_color_intrinsic(colorIntrinsic);
		image.set_distance_intrinsic(distIntrinsic);
		image.set_camera_extrinsic(extrinsicMat);

		image.set_sequence_counter(imageCounter++);
		image.set_data_valid(true);
	} else {
		image.set_sequence_counter(imageCounter++);
		image.set_data_valid(false);
	}
}
