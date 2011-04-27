//--------------------------------------------------------------------------
//
//  Copyright (C) 2009 Siegfried Hochdorfer
//
//        hochdorfer@hs-ulm.de
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

#ifndef PMD_CAMERA_HH_
#define PMD_CAMERA_HH_

#include <stdint.h>
#include <iostream>
#include <string>

#include <CommVisionObjects/commToFImage.hh>
#include "pmdsdk2.h"

#include "smartSoft.hh"

using namespace std;

namespace ToFSensor {

class PMDException: std::exception {
private:
	std::string message;

public:
	PMDException(const std::string& message) throw() {
		this->message = message;
	}

	virtual ~PMDException() throw() {
	}

	virtual const char* what() const throw() {
		return this->message.c_str();
	}

};

class PMDCamera {

private:
	PMDHandle hnd; // connection handle
	PMDDataDescription dd;

	uint32_t imageWidth;
	uint32_t imageHeight;
	uint32_t numberOfPixels;

	uint32_t integrationTime;
	uint32_t modulationFrequency;

	double min_distance;
	double max_distance;
	double opening_angle_x_axis;
	double opening_angle_y_axis;

	bool initialized;

	uint32_t image_counter;

	CHS::SmartMutex mutex;


public:
	PMDCamera();
	~PMDCamera();

	void init(const string& sourcePlugin, const string& sourceParam, const string& procPlugin, const string& procParam,
			uint32_t width, uint32_t height, double min_distance, double max_distance, double opening_angle_x_axis,
			double opening_angle_y_axis);

	void setIntegrationTime(uint32_t integrationTime);
	uint32_t getIntegrationTime() const;

	void setModulationFrequency(uint32_t frequency);
	uint32_t getModulationFrequency() const;

	void setLenseCalibrationOn(bool activate);
	void setSuppressMotionBlur(bool activate);

	bool isInitialized();

	void getImage(CommVisionObjects::CommToFImage& image);

private:
	int getDistances(float * data, size_t size);
	int getIntensities(float * data, size_t size);
	int getAmplitudes(float * data, size_t size);
	int getCoordinates(float * data, size_t size);
	int update();

	void updateIntegrationTime();
	void updateModulationFrequency();

};
}

#endif /*PMD_CAMERA_HH_*/
