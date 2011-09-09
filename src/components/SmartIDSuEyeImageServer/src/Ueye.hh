//--------------------------------------------------------------------------
//
//  Copyright (C) 2011 Tobias Fromm, Manuel Wopfner
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "IDS uEye Video Server component".
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

#ifndef UEYE_H_
#define UEYE_H_

#include <CommVisionObjects/commVideoImage.hh>
#include "ColorMode.hh"

#include <mrpt/vision.h>

// standard includes
#include <iostream>

// define configures ueye.h
#define __LINUX__

// ueye includes
#include <ueye.h>

class Ueye
{

private:
	HIDS camera;
	int memoryId; // camera memory - buffer ID
	char* imageMemory; // camera memory - pointer to buffer

	uint32_t width; // actual width of captured images
	uint32_t height; // actual height of captured images

	int currentError; // last error is stored here - just for compatibility reaons with other camera drivers
	long frameId;

	ColorMode colorMode;
	bool debug_on;

	bool undistort_image;
	mrpt::vision::CUndistortMap undistort_map;

public:
	Ueye();
	virtual ~Ueye();

	bool openCamera();
	bool loadParameters(const std::string& paramFile);
	bool closeCamera();

	void setCameraParmas(double fx, double fy, double cx, double cy, double k1, double k2, double p1, double p2, double k3);

	void setDebug(bool debug = true)
	{
		debug_on = debug;
	}
	void undistortImage(bool undistort = true)
	{
		undistort_image = undistort;
	}

	int getWidth();
	int getHeight();

	long getFrameId();
	ColorMode getColorMode();

	bool getImage(CommVisionObjects::CommVideoImage& image);

private:
	inline void updateFrameId();
	inline void updateImageSize();
	inline void freeMemory();

};

#endif /* UEYE_H_ */
