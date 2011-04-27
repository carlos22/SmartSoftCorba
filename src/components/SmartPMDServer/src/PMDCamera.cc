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

#include "PMDCamera.hh"
using namespace ToFSensor;

#include <iostream>
#include <stdio.h>
#include <math.h>

#include <smartSoft.hh>
#include <CommVisionObjects/commToFImage.hh>

PMDCamera::PMDCamera() {
	image_counter = 0;
}

PMDCamera::~PMDCamera() {
	//CHS::SmartGuard g(mutex);
	pmdClose(hnd);
}

void PMDCamera::init(const string& sourcePlugin, const string& sourceParam,
		const string& procPlugin, const string& procParam, uint32_t width,
		uint32_t height, double min_distance, double max_distance,
		double opening_angle_x_axis, double opening_angle_y_axis) {

	//CHS::SmartGuard g(mutex);

	this->imageWidth = width;
	this->imageHeight = height;
	this->numberOfPixels = width * height;

	this->min_distance = min_distance;
	this->max_distance = max_distance;
	this->opening_angle_x_axis = opening_angle_x_axis;
	this->opening_angle_y_axis = opening_angle_y_axis;

	this->integrationTime = 0;
	this->modulationFrequency = 0;

	this->initialized = true;

	// connect to camera
	int res = -1;
	res = pmdOpen(&this->hnd, sourcePlugin.c_str(), sourceParam.c_str(),
			procPlugin.c_str(), procParam.c_str());
	if (res != PMD_OK) {
		std::cerr << "Could not connect (" << sourcePlugin << ", "
				<< sourceParam << ", " << procPlugin << ", " << procParam
				<< ")\n";

		this->initialized = false;
		throw new PMDException("PMDCamera::init() >> Could not connect!");
	}

	res = pmdUpdate(hnd);
	if (res != PMD_OK) {
		std::cerr << "Could not retrieve data\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException("PMDCamera::init() >> Could not retrieve data!");
	}

	res = pmdGetSourceDataDescription(hnd, &this->dd);
	if (res != PMD_OK) {
		std::cerr << "Could not retrieve sensor description\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException(
				"PMDCamera::init() >> Could not retrieve sensor description!");
	}
}

void PMDCamera::setLenseCalibrationOn(bool activate) {
	int res = -1;

	//CHS::SmartGuard g(mutex);

	if (activate) {
		// Turn lens calibration on
		res = pmdSourceCommand(hnd, 0, 0, "SetLensCalibration On");

		if (res != PMD_OK) {
			std::cerr << "Could not turn lens calibration on\n";

			pmdClose(hnd);
			this->initialized = false;
			throw new PMDException(
					"PMDCamera::setLenseCalibrationOn() >> Could not turn lens calibration on!");
		}
	} else {
		// Turn lens calibration off
		res = pmdSourceCommand(hnd, 0, 0, "SetLensCalibration Off");

		if (res != PMD_OK) {
			std::cerr << "Could not turn lens calibration off\n";

			pmdClose(hnd);
			this->initialized = false;
			throw new PMDException(
					"PMDCamera::setLenseCalibrationOn() >> Could not turn lens calibration off!");
		}
	}
}

void PMDCamera::setSuppressMotionBlur(bool activate) {
	int res = -1;

	//CHS::SmartGuard g(mutex);

	if (activate) {
		// Turn lens calibration on
		res = pmdSourceCommand(hnd, 0, 0, "SetExposureMode SMB");

		if (res != PMD_OK) {
			std::cerr << "Could not turn suppress motion blur on\n";

			pmdClose(hnd);
			this->initialized = false;
			throw new PMDException(
					"PMDCamera::setSuppressMotionBlur() >> Could not turn suppress motion blur on!");
		}
	} else {
		// Turn lens calibration off
		res = pmdSourceCommand(hnd, 0, 0, "SetExposureMode Normal");

		if (res != PMD_OK) {
			std::cerr << "Could not turn suppress motion blur off\n";

			pmdClose(hnd);
			this->initialized = false;
			throw new PMDException(
					"PMDCamera::setSuppressMotionBlur() >> Could not turn suppress motion blur off!");
		}
	}
}

void PMDCamera::setIntegrationTime(uint32_t integrationTime) {
	int res = -1;

	//CHS::SmartGuard g(mutex);

	// set new integration time
	uint32_t validIntegrationTime = 2500; // PMD CamCube2 default value for integration time
	pmdGetValidIntegrationTime(hnd, &validIntegrationTime, 0, CloseTo,
			integrationTime);
	res = pmdSetIntegrationTime(hnd, 0, validIntegrationTime);

	if (res != PMD_OK) {
		std::cerr << "Could not set integration time\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException(
				"PMDCamera::setIntegrationTime() >> Could not set integration time!");
	}

	updateIntegrationTime();
}

uint32_t PMDCamera::getIntegrationTime() const {
	return this->integrationTime;
}

void PMDCamera::setModulationFrequency(uint32_t frequency) {
	int res = -1;

	//CHS::SmartGuard g(mutex);

	// convert from MHz to Hz
	frequency *= 1000000;

	// set new modulation frequency
	uint32_t validModulationFrequency = 18; // PMD CamCube2 default value for modulation frequency
	pmdGetValidModulationFrequency(hnd, &validModulationFrequency, 0, CloseTo,
			frequency);
	res = pmdSetModulationFrequency(hnd, 0, validModulationFrequency);

	if (res != PMD_OK) {
		std::cerr << "Could not set modulation frequency\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException(
				"PMDCamera::setModulationFrequency() >> Could not set modulation frequency!");
	}

	updateModulationFrequency();
}

uint32_t PMDCamera::getModulationFrequency() const {
	return this->modulationFrequency;
}

void PMDCamera::updateIntegrationTime() {
	// read set integration time
	int res = -1;

	//CHS::SmartGuard g(mutex);

	res = pmdGetIntegrationTime(hnd, &this->integrationTime, 0);
	if (res != PMD_OK) {
		std::cerr << "Could not read set integration time\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException(
				"PMDCamera::updateIntegrationTime() >> Could not read set integration time!");
	}
}

void PMDCamera::updateModulationFrequency() {
	// read set modulation frequency
	int res = -1;

	//CHS::SmartGuard g(mutex);

	res = pmdGetModulationFrequency(hnd, &this->modulationFrequency, 0);
	this->modulationFrequency /= 1000000;

	if (res != PMD_OK) {
		std::cerr << "Could not read set modulation frequency\n";

		pmdClose(hnd);
		this->initialized = false;
		throw new PMDException(
				"PMDCamera::updateModulationFrequency() >> Could not read set modulation frequency!");
	}
}

int PMDCamera::update() {
	//CHS::SmartGuard g(mutex);
	image_counter++;
	return pmdUpdate(hnd);
}

bool PMDCamera::isInitialized() {
	return this->initialized;
}

int PMDCamera::getDistances(float * data, size_t size) {
	//CHS::SmartGuard g(mutex);
	return pmdGetDistances(hnd, data, size);
}

int PMDCamera::getIntensities(float * data, size_t size) {
	//	CHS::SmartGuard g(mutex);
	return pmdGetIntensities(hnd, data, size);
}

int PMDCamera::getAmplitudes(float * data, size_t size) {
	//CHS::SmartGuard g(mutex);
	return pmdGetAmplitudes(hnd, data, size);
}

int PMDCamera::getCoordinates(float * data, size_t size) {
	//	CHS::SmartGuard g(mutex);
	return pmdGet3DCoordinates(hnd, data, size);
}

void PMDCamera::getImage(CommVisionObjects::CommToFImage& image) {

	// first invalidate image
	image.set_data_valid(false);

	if (!this->initialized) {
		throw new PMDException("PMDCamera not initialized!");
	}

	if (image.get_width() != this->imageWidth || image.get_height()
			!= this->imageHeight) {
		throw new PMDException(
				"PMDCamera::getImage >> Image size does not fit!");
	}

	int res = 0;
	float temp = 0;

	// get next image from camera
	this->update();

	// read distances
	//res = this->getDistances(image.get_distances(), image.get_size());
	float tmp_img[image.get_image_size()];

	res = this->getDistances(tmp_img, image.get_image_size() * sizeof(float));

	if (res == PMD_OK) {
		// flip around y-axis
		for (uint32_t i = 0; i < this->imageHeight; i++) {
			for (uint32_t j = 0; j < this->imageWidth / 2; j++) {
				//				float *leftPtr = image.get_distances() + (i * this->imageWidth) + j;
				//				float *rightPtr = image.get_distances() + (i * this->imageWidth) + ((this->imageWidth - 1) - j);
				float *leftPtr = tmp_img + (i * this->imageWidth) + j;
				float *rightPtr = tmp_img + (i * this->imageWidth)
						+ ((this->imageWidth - 1) - j);

				temp = *leftPtr;
				*leftPtr = *rightPtr;
				*rightPtr = temp;
			}
		}
		image.set_distances(tmp_img);
	} else {
		throw new PMDException(
				"PMDCamera::getImage >> could not read distances!");
	}

	// read amplitudes
	//res = this->getAmplitudes(image.get_amplitudes(), image.get_size());
	res = this->getAmplitudes(tmp_img, image.get_image_size() * sizeof(float));
	if (res == PMD_OK) {
		// flip around y-axis
		for (uint32_t i = 0; i < this->imageHeight; i++) {
			for (uint32_t j = 0; j < this->imageWidth / 2; j++) {
				//				float *leftPtr = image.get_amplitudes() + (i * this->imageWidth) + j;
				//				float *rightPtr = image.get_amplitudes() + (i * this->imageWidth) + ((this->imageWidth - 1) - j);
				float *leftPtr = tmp_img + (i * this->imageWidth) + j;
				float *rightPtr = tmp_img + (i * this->imageWidth)
						+ ((this->imageWidth - 1) - j);
				temp = *leftPtr;
				*leftPtr = *rightPtr;
				*rightPtr = temp;
			}
		}
		image.set_amplitudes(tmp_img);
	} else {
		throw new PMDException(
				"PMDCamera::getImage >> could not read amplitudes!");
	}

	// read intensities
	//res = this->getIntensities(image.get_intensities(), image.get_size());
	res = this->getIntensities(tmp_img, image.get_image_size() * sizeof(float));
	if (res == PMD_OK) {
		// flip around y-axis
		for (uint32_t i = 0; i < this->imageHeight; i++) {
			for (uint32_t j = 0; j < this->imageWidth / 2; j++) {
				//				float *leftPtr = image.get_intensities() + (i * this->imageWidth) + j;
				//				float *rightPtr = image.get_intensities() + (i * this->imageWidth) + ((this->imageWidth - 1) - j);
				float *leftPtr = tmp_img + (i * this->imageWidth) + j;
				float *rightPtr = tmp_img + (i * this->imageWidth)
						+ ((this->imageWidth - 1) - j);
				temp = *leftPtr;
				*leftPtr = *rightPtr;
				*rightPtr = temp;
			}
		}
		image.set_intensities(tmp_img);
	} else {
		throw new PMDException(
				"PMDCamera::getImage >> could not read intensities!");
	}

	// read coordinates
	float tmp_coords[image.get_image_size() * 3];
	//res = this->getCoordinates(image.get_coordinates(), 3 * image.get_size());
	res = this->getCoordinates(tmp_coords, 3 * image.get_image_size()
			* sizeof(float));
	if (res == PMD_OK) {
		// flip around y-axis
		for (uint32_t i = 0; i < this->imageHeight; i++) {
			for (uint32_t j = 0; j < this->imageWidth / 2; j++) {
				//float *leftPtr = image.get_coordinates() + (i * this->imageWidth * 3) + (j * 3);
				//float *rightPtr = image.get_coordinates() + (i * this->imageWidth * 3) + (((this->imageWidth - 1) * 3)- ((j + 1) * 3));
				float *leftPtr = tmp_coords + (i * this->imageWidth * 3) + (j
						* 3);
				float *rightPtr = tmp_coords + (i * this->imageWidth * 3)
						+ (((this->imageWidth - 1) * 3) - ((j + 1) * 3));

				temp = -1* (* leftPtr);
				*leftPtr = -1* (*rightPtr);
				*rightPtr = temp;

				temp = *(leftPtr+1);
				*(leftPtr+1) = *(rightPtr+1);
				*(rightPtr+1) = temp;

				temp = *(leftPtr+2);
				*(leftPtr+2) = *(rightPtr+2);
				*(rightPtr+2) = temp;
			}
		}
		image.set_coordinates(tmp_coords);
	} else {
		throw new PMDException("PMDCamera::getImage >> could not read coordinates!");
	}

				image.set_sequence_counter(image_counter);
				image.set_min_distance(this->min_distance);
				image.set_max_distance(this->max_distance);
				image.set_opening_angle_x_axis(this->opening_angle_x_axis);
				image.set_opening_angle_y_axis(this->opening_angle_y_axis);
				image.set_integration_time(this->integrationTime);
				image.set_modulation_frequency(this->modulationFrequency);
				image.set_data_valid(true);
			}
