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

#include "Ueye.hh"
#include "gen/SmartIDSuEyeImageServer.hh"

Ueye::Ueye()
{
	camera = (HIDS) 1;
	frameId = 0;
	imageMemory = NULL;

	debug_on = false;
	undistort_image = false;
}

Ueye::~Ueye()
{
}

bool Ueye::openCamera()
{
	// open camera
	if (is_InitCamera(&camera, NULL) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not open camera!" << std::endl;
		return false;
	}

	// sensor info includes maximum image size
	SENSORINFO SensorInfo;
	if (is_GetSensorInfo(camera, &SensorInfo) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not get sensor info!" << std::endl;
		return false;
	}

	std::cout << "[Ueye] max width: " << SensorInfo.nMaxWidth << ", max height: " << SensorInfo.nMaxHeight << "\n";
	std::cout << "[Ueye] Camera opened" << std::endl;

	return true;
}

bool Ueye::loadParameters(const std::string& paramFile)
{
	// load parameters stored in firmware
	if (is_LoadParameters(camera, paramFile.c_str()) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not load camera parameters!" << std::endl;
		return false;
	}
	std::cout << "[Ueye] pram file: " << paramFile << "\n";

	updateImageSize();

	colorMode = is_SetColorMode(camera, IS_GET_COLOR_MODE);
	;
	std::cout << "[Ueye] color mode: " << colorMode << "\n";

	// free image memory to avoid memory leaks
	freeMemory();

	// allocate memory for image size and color mode
	if (is_AllocImageMem(camera, this->width, this->height, colorMode.getBits(), &imageMemory, &memoryId) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not allocate memory for camera image!" << std::endl;
		return false;
	}

	if (is_SetImageMem(camera, imageMemory, memoryId) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not set image memory!" << std::endl;
		return false;
	}

	updateFrameId();

	std::cout << "[Ueye] Camera parameters loaded" << std::endl;
	return true;
}

bool Ueye::closeCamera()
{

	// free image memory to avoid memory leaks
	freeMemory();

	// close camera
	if (is_ExitCamera(camera) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not close camera!" << std::endl;
		return false;
	}

	std::cout << "[Ueye] Camera closed" << std::endl;
	return true;
}

void Ueye::setCameraParmas(double fx, double fy, double cx, double cy, double k1, double k2, double p1, double p2, double k3)
{
	mrpt::utils::TCamera cam_params;
	cam_params.setIntrinsicParamsFromValues(fx, fy, cx, cy);
	cam_params.setDistortionParamsFromValues(k1, k2, p1, p2, k3);
	undistort_map.setFromCamParams(cam_params);
}

bool Ueye::getImage(CommVisionObjects::CommVideoImage& image)
{
	// capture new frame
	if (is_FreezeVideo(camera, IS_WAIT) != IS_SUCCESS)
	{
		std::cerr << "[Ueye] Could not get image from camera!\n";
		return false;
	}

	updateFrameId();

	if (undistort_image)
	{
		mrpt::utils::CImage img;
		img.loadFromMemoryBuffer(width, height, true, (unsigned char*) imageMemory);
		undistort_map.undistort(img); // output in place
		imageMemory = (char*) img.getAs<unsigned char>();
	}

	if (image.get_width() == width && image.get_height() == height)
	{
		image.set_data((unsigned char*) imageMemory);
		image.set_sequence_counter(frameId);
		if (debug_on)
			std::cout << "[Ueye] Image: DATA VALID" << std::endl;
	} else
	{
		image.set_data_invalid();
		if (debug_on)
			std::cout << "[Ueye] Image: DATA INVALID" << std::endl;

		return false;
	}

	return true;
}

int Ueye::getHeight()
{
	return height;
}

int Ueye::getWidth()
{
	return width;
}

long Ueye::getFrameId()
{
	return frameId;
}

ColorMode Ueye::getColorMode()
{
	return colorMode;
}

/////////////////////////////////////////////////////7
//
// private methods
//
/////////////////////////////////////////////////////7

void Ueye::updateFrameId()
{
	long vsync, frameSync;
	if (is_GetVsyncCount(camera, &vsync, &frameSync) == IS_SUCCESS)
	{
		frameId = frameSync;
	}
}

void Ueye::updateImageSize()
{
	int xPos, yPos, width, height;
	if (is_SetAOI(camera, IS_GET_IMAGE_AOI, &xPos, &yPos, &width, &height) == IS_SUCCESS)
	{
		this->width = width;
		this->height = height;

		std::cout << "[Ueye] xPos: " << xPos << ", yPos: " << yPos << ", width: " << width << ", height: " << height << "\n";
	}

}

void Ueye::freeMemory()
{
	if (imageMemory != NULL)
	{
		if (is_FreeImageMem(camera, imageMemory, memoryId) != IS_SUCCESS)
		{
			std::cerr << "[Ueye] Could not free camera memory!" << std::endl;
		}
	}
}
