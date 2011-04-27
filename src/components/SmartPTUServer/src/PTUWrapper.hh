//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
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


#ifndef PTUWRAPPER_HH
#define PTUWRAPPER_HH

#include <CommPTUObjects/commPTUMoveRequest.hh>
#include <CommPTUObjects/commPTUMoveResponse.hh>
#include <CommPTUObjects/commPTUParameter.hh>
#include <CommBasicObjects/commPose3d.hh>

#include <armadillo.hh>
#include <mrpt/hwdrivers/CPtuDPerception.h>

#include <string>

using namespace mrpt::hwdrivers;

class PTUWrapper {

private:
	typedef CommPTUObjects::PTUMoveStatus PTUStatus;

	enum PTUAxis {
		PAN, TILT, NONE
	};

	struct PTUParameters {
		CommBasicObjects::CommPose3d sensorOffset;

		bool reset;

		double start_up_speed_pan;
		double start_up_speed_tilt;
		double speed_pan;
		double speed_tilt;
		double acceleration_pan;
		double acceleration_tilt;
		double max_speed_pan;
		double max_speed_tilt;
		double min_speed_pan;
		double min_speed_tilt;

		bool modified;
	};

	static const uint32_t waitTries = 6;

	mrpt::hwdrivers::CPtuDPerception ptu;
	bool ptuActive;

	bool posChanged;
	double currentPan;
	double currentTilt;

	double commandedPan;
	double commandedTilt;

	PTUParameters localParam;
	PTUParameters globalParam;

	arma::mat ptuRobotOffset;
	CommBasicObjects::CommBaseVelocity zero_velocity;
	CommBasicObjects::CommBasePosition default_base_position;

public:
	PTUWrapper();
	virtual ~PTUWrapper();

	bool init(const std::string& device);

	void setParameter(const CommPTUObjects::CommPTUParameter& param);
	void move(const CommPTUObjects::CommPTUMoveRequest& request, CommPTUObjects::CommPTUMoveResponse& answer);
	void getPos(double& pan, double& tilt);

	void start();
	void stop();

private:
	PTUStatus lookPointRobot(const CommPTUObjects::CommPTUMoveRequest& request);
	PTUStatus lookPointWorld(const CommPTUObjects::CommPTUMoveRequest& request);

	PTUStatus movePanAbs(double pan);
	PTUStatus movePanOff(double pan);

	PTUStatus moveTiltAbs(double tilt);
	PTUStatus moveTiltOff(double tilt);

	PTUStatus moveToAbsPos(double pan, double tilt);
	PTUStatus moveToOffPos(double pan, double tilt);

	void waitTillPosReached(PTUStatus& status);

	void checkForErros(PTUStatus& status, PTUAxis axis);
};

#endif
