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

#include "gen/ImageTaskCore.hh"

#include <mrpt/hwdrivers.h>
#include <mrpt/gui.h>
#include <mrpt/maps.h>
#include <mrpt/vision.h>
#include <MyCKinect.hh>

using namespace mrpt;
using namespace mrpt::hwdrivers;
using namespace mrpt::math;
using namespace mrpt::gui;
using namespace mrpt::slam;
using namespace mrpt::utils;
using namespace std;


#include "CommVisionObjects/commKinectImage.hh"

class KinectWrapper {

private:
	ulong imageCounter;

	arma::mat colorIntrinsic;
	arma::mat distIntrinsic;
	arma::mat extrinsicMat;

	MyCKinect  kinect;

	mrpt::vision::CUndistortMap undistort_map;

public:
	KinectWrapper();

	virtual ~KinectWrapper();

	void startVideo(){
		try{
			kinect.open();
		} catch (std::exception e) {
			std::cerr << "[Kinect Wrapper] Error while opening kinect dev : " << e.what() << "\n";
		}

	}

	void startDepth(){

	}


	void stopVideo(){
		try{
			kinect.close();
		} catch (std::exception e) {
				std::cerr << "[Kinect Wrapper] Error while closing kinect dev : " << e.what() << "\n";
		}

	}

	void stopDepth(){

	}

	void getImage(CommVisionObjects::CommKinectImage& image);

};

#endif /* KINECTWRAPPER_HH_ */
