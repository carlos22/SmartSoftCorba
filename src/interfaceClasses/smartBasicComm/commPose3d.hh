// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------

#ifndef SMART_COMM_POSE_3D_HH
#define SMART_COMM_POSE_3D_HH

#include <string>
#include <iostream>
#include "math.h"

#include "smartPose3dC.hh"

#include "commPosition3d.hh"
#include "commOrientation3d.hh"

#include "armadillo.hh"
#include "EulerTransformationMatrices.hh"

namespace Smart {

/**
 The pose of a body in 3d space consists of its position and its orientation.
 \sa Smart::CommPosition3d Smart::CommOrientation3d
 */
class CommPose3d {
protected:
	SmartIDL::Pose3d _pose;

public:

	CommPose3d() {
	}
	CommPose3d(const CommPosition3d &pos, const CommOrientation3d &ori) {
		set_position(pos);
		set_orientation(ori);
	}
	CommPose3d(double x, double y, double z, double azimuth, double elevation, double roll, const double unit = 0.001) {
		set_x(x, unit);
		set_y(y, unit);
		set_z(z, unit);
		set_azimuth(azimuth);
		set_elevation(elevation);
		set_roll(roll);
	}

	/**
	 Creates a Pose3d object from a given homogenous matrix (4x4)
	 The translation is in the form of the given unit.
	 */
	CommPose3d(arma::mat &matrix, const double unit = 0.001) {
		double x, y, z, azimuth, elevation, roll;

        EulerTransformationMatrices::zyx_from_matrix(matrix, x, y, z, azimuth, elevation, roll);
		set_x(x, unit);
		set_y(y, unit);
		set_z(z, unit);
		set_azimuth(azimuth);
		set_elevation(elevation);
		set_roll(roll);
	}
	CommPose3d(const SmartIDL::Pose3d &pose) :
		_pose(pose) {
	}
	virtual ~CommPose3d() {
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);
	static inline std::string identifier() {
		return "Smart::CommPose3d";
	}
	;

	/// internal use only
	const SmartIDL::Pose3d &get_idl() const {
		return _pose;
	}
	void set_idl(const SmartIDL::Pose3d &pose) {
		_pose = pose;
	}

	/**
	 Set the position.
	 */
	inline void set_position(const CommPosition3d &pos) {
		_pose.position = pos.get_idl();
	}
	/**
	 Set the orientation.
	 */
	inline void set_orientation(const CommOrientation3d &ori) {
		_pose.orientation = ori.get_idl();
	}

	/**
	 Get the position.
	 */
	inline CommPosition3d get_position() const {
		return CommPosition3d(_pose.position);
	}

	/**
	 Get the orientation.
	 */
	inline CommOrientation3d get_orientation() const {
		return CommOrientation3d(_pose.orientation);
	}

	/**
	 Get the x coordinate of the position.
	 */
	inline double get_x(const double unit = 0.001) const {
		return _pose.position.x * 0.001 / unit;
	}
	/**
	 Get the y coordinate of the position.
	 */
	inline double get_y(const double unit = 0.001) const {
		return _pose.position.y * 0.001 / unit;
	}
	/**
	 Get the z coordinate of the position.
	 */
	inline double get_z(const double unit = 0.001) const {
		return _pose.position.z * 0.001 / unit;
	}

	/**
	 Set the x coordinate of the position.
	 */
	inline void set_x(double x, const double unit = 0.001) {
		_pose.position.x = x * unit * 1000;
	}
	/**
	 Set the y coordinate of the position.
	 */
	inline void set_y(double y, const double unit = 0.001) {
		_pose.position.y = y * unit * 1000;
	}
	/**
	 Set the z coordinate of the position.
	 */
	inline void set_z(double z, const double unit = 0.001) {
		_pose.position.z = z * unit * 1000;
	}

	/**
	 Get the azimuth in radians.
	 */
	inline double get_azimuth() const {
		return _pose.orientation.azimuth;
	}
	/**
	 Get the elevation in radians.
	 */
	inline double get_elevation() const {
		return _pose.orientation.elevation;
	}
	/**
	 Get the roll in radians.
	 */
	inline double get_roll() const {
		return _pose.orientation.roll;
	}

	/**
	 Set the azimuth in radians.
	 */
	inline void set_azimuth(double a) {
		_pose.orientation.azimuth = a;
	}
	/**
	 Set the elevation in radians.
	 */
	inline void set_elevation(double e) {
		_pose.orientation.elevation = e;
	}
	/**
	 Set the roll in radians.
	 */
	inline void set_roll(double r) {
		_pose.orientation.roll = r;
	}

	/**
	 Returns a homogenous Matrix with azimuth, elevation, roll and the
	 translation in the given unit.
	 */
	inline arma::mat getHomogeneousMatrix(const double unit = 0.001) const {
		arma::mat matrix(4, 4);
        EulerTransformationMatrices::create_zyx_matrix(get_x(unit), get_y(unit), get_z(unit), get_azimuth(), get_elevation(), get_roll(), matrix);
		return matrix;
	}

	/**
	 Write information about this pose to the given output stream.
	 */
	void print(std::ostream &os = std::cout) const;

	/**
	 Save an XML like representation of this 3d pose to the given output stream.
	 */
	void save_xml(std::ostream &os, const std::string &indent = "") const;

	/**
	 Load from XML represenation on an istream.
	 Be warned, no error checking is done.
	 For details, see \ref smart_basic_com-xml.
	 */
	void load_xml(std::istream &is);
};

inline std::ostream &operator<<(std::ostream &os, const CommPose3d &pose) {
	pose.print(os);
	return os;
}

} // namespace Smart

#endif
