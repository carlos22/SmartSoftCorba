//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Johannes König, Alex Lotz, Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartLaserHokuyoURGServer component".
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

#ifndef SCIPWRAPPER_HH_
#define SCIPWRAPPER_HH_

#include <stdexcept>
#include <sys/time.h>

#include "scipBase.h"
#include "scip1.h"
#include "scip2.h"
#include "scipUtil.h"

class ScipWrapper {

public:
	enum RangerType {
		URG_04LX, URG_04LX_UG01, UBG_05LX_F01, UHG_08LX, UTM_30LX
	};

private:
	tPort* port;
	timeval recive_timestamp;
	int** scan2;
	int step_num;

	int start_step;
	int end_step;
	int step_cluster;
	int scan_interval;
	int scan_num;

	double opening_angle; // 1/100 degree
	double resolution; // 1/100 degree
	uint32_t min_distance;
	uint32_t max_distance;

	RangerType type;

private:
	void set_attributes(RangerType type) {
		scan2 = NULL;

		this->type = type;

		switch (type) {
		case URG_04LX: {
			opening_angle = 239.765625;
			resolution = 0.3515625;
			min_distance = 20;
			max_distance = 5600;

			start_step = 44;
			end_step = 725;
			step_cluster = 1;
			scan_interval = 0;
			scan_num = 1;
			break;
		}
		case URG_04LX_UG01: {
			opening_angle = 24000;
			resolution = 36;
			min_distance = 20;
			max_distance = 4000;

			start_step = 44;
			end_step = 725;
			step_cluster = 1;
			scan_interval = 0;
			scan_num = 1;
			break;
		}
		case UBG_05LX_F01: {
			opening_angle = 24000;
			resolution = 36;
			min_distance = 60;
			max_distance = 4095;

			start_step = 44;
			end_step = 725;
			step_cluster = 1;
			scan_interval = 0;
			scan_num = 1;
			break;
		}
		case UHG_08LX: {
			opening_angle = 27000;
			resolution = 36;
			min_distance = 100;
			max_distance = 8000;

			start_step = 0;
			end_step = 768;
			step_cluster = 1;
			scan_interval = 0;
			scan_num = 1;
			break;
		}
		case UTM_30LX: {
			opening_angle = 27000;
			resolution = 25;
			min_distance = 100;
			max_distance = 30000;

			start_step = 0;
			end_step = 1080;
			step_cluster = 1;
			scan_interval = 0;
			scan_num = 1;
			break;
		}
		}

	}

public:
	ScipWrapper() :
		port(NULL) {
	}

	~ScipWrapper() {
		if (scan2 != NULL)
			scip2FreeScan(scan2, scan_num);

		close_device();
	}

	bool is_open() const {
		return port != NULL;
	}

	void open_device(RangerType type, const char* dev, int speed = 115200) {
		if (is_open()) {
			close_device();
		}

		set_attributes(type);

		port = scipConnect(dev);
		if (port == NULL) {
			throw std::runtime_error("Could not connect to the sensor!");
		}

		if (type == URG_04LX) {
			switchToScip2(port);
		}

		if (scip2SetComSpeed(port, speed) != 0) {
			throw std::runtime_error("Could not change speed!");
		}
	}

	void close_device() {
		if (is_open())
			closePort(port);

	}

	bool read_laser_data() {
		if (scan2 != 0)
			scip2FreeScan(scan2, scan_num);

		scan2 = scip2MeasureScan(port, start_step, end_step, step_cluster, scan_interval, scan_num, ENC_3BYTE,
				&step_num);

		if (scan2 != NULL) {
			gettimeofday(&recive_timestamp, 0);
		}

		return scan2 != NULL;
	}

	uint32_t extract_distance(uint32_t i) const {

		// if there are scan values, return the values
		if (scan2 != NULL && scan2[0] != NULL)
			return scan2[0][i];
		// otherwise return an invalid value
		else
			return get_max_distance() + 1;
	}

	timeval get_receive_timestamp() const {
		return recive_timestamp;
	}

	uint32_t extract_num_points() const {
		return step_num;
	}

	uint32_t extract_intensity(uint32_t i) const {
		return 0;
	}

	uint32_t get_min_distance() const {
		return this->min_distance;
	}

	uint32_t get_max_distance() const {
		return this->max_distance;
	}

	double get_opening_angle() const {
		return this->opening_angle;
	}

	double get_resolution() const {
		return this->resolution;
	}
};

#endif /* SCIP_HH_ */
