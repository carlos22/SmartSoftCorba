//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Manuel Wopfner
//				  2011 Dennis Stampfer
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "CommManipulatorObjects".
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

#ifndef _COMM_COMMVISIONOBJECTS_COMMKINECTIMAGE_HH
#define _COMM_COMMVISIONOBJECTS_COMMKINECTIMAGE_HH

#include <string>

// include files of classes

// include header-files of included communication objects
#include <CommBasicObjects/commBaseState.hh>
#include <CommBasicObjects/commPose3d.hh>
#include <CommBasicObjects/commTimeStamp.hh>
#include <CommVisionObjects/comm3dPointCloud.hh>

// include enums


// include client-side file generated by IDL compiler
#include "CommVisionObjects/gen/smartCommKinectImageC.hh"

namespace CommVisionObjects
{

class CommKinectImage
{
protected:
	CommVisionObjectsIDL::CommKinectImage idl_CommKinectImage;

private:
	arma::mat robot_mat;
	arma::mat world_mat;

	arma::mat color_intrinsic_mat;
	arma::mat color_inv_intrinsic_mat;
	arma::mat dist_intrinsic_mat;
	arma::mat camera_extrinsic_mat;

	/**
	 * Set the parameters of an image.
	 */
	void initialize()
	{

		// zero everything
		idl_CommKinectImage.rgb_width = 0;
		idl_CommKinectImage.rgb_height = 0;
		idl_CommKinectImage.distance_width = 0;
		idl_CommKinectImage.distance_height = 0;

		idl_CommKinectImage.min_distance = 0;
		idl_CommKinectImage.max_distance = 0;
		idl_CommKinectImage.opening_angle_x_axis = 0;
		idl_CommKinectImage.opening_angle_y_axis = 0;

		idl_CommKinectImage.is_valid = false;
		idl_CommKinectImage.seq_count = 0;

		arma::mat zero(4, 4);
		zero.zeros();

		idl_CommKinectImage.camera_extrinsic_m.length(4* 4 );
		idl_CommKinectImage.color_intrinsic_m.length(4*4);
		idl_CommKinectImage.dist_intrinsic_m.length(4*4);
		set_camera_extrinsic(zero);
		set_color_intrinsic(zero);
		set_distance_intrinsic(zero);
	}

public:
	CommKinectImage()
	{
		initialize();
	}

	CommKinectImage(const CommVisionObjectsIDL::CommKinectImage &obj) : idl_CommKinectImage(obj)
	{
		initialize();
	}

	operator CommVisionObjectsIDL::CommKinectImage() const
	{
		return idl_CommKinectImage;
	}

	virtual ~CommKinectImage()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommVisionObjects::CommKinectImage";
	}

	//
	// user interfaces
	/**
	 * Get the pose of the sensor relative to the robot coordinate system.
	 */
	inline CommBasicObjects::CommPose3d get_sensor_pose() const
	{
		return idl_CommKinectImage.sensor_pose;
	}

	/**
	 * Get the state of the base.
	 */
	inline CommBasicObjects::CommBaseState get_base_state() const
	{
		return idl_CommKinectImage.base_state;
	}

	/**
	 * Get the width of the RGB image in pixels.
	 */
	inline uint32_t get_rgb_width() const
	{
		return idl_CommKinectImage.rgb_width;
	}

	/**
	 * Get the height of the RGB image in pixels.
	 */
	inline uint32_t get_rgb_height() const
	{
		return idl_CommKinectImage.rgb_height;
	}

	/**
	 * Get the width of the distance image in pixels.
	 */
	inline uint32_t get_distance_width() const
	{
		return idl_CommKinectImage.distance_width;
	}

	/**
	 * Get the height of the distance image in pixels.
	 */
	inline uint32_t get_distance_height() const
	{
		return idl_CommKinectImage.distance_height;
	}

	/**
	 * Get the minimal distance the sensor works.
	 */
	inline double_t get_min_distance(const double_t unit = 0.001) const
	{
		return idl_CommKinectImage.min_distance * 0.001 / unit;
	}

	/**
	 * Get the maximal distance the sensor works.
	 */
	inline double_t get_max_distance(const double_t unit = 0.001) const
	{
		return idl_CommKinectImage.max_distance * 0.001 / unit;
	}

	/**
	 * Get the opening angle of the rgb camera in x-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline double_t get_rgb_opening_angle_x_axis()
	{
		// look "learning opencv" book (page 372)
		return atan((get_rgb_width() / 2) /get_color_intrinsic()(0,0)) * 2;
	}

	/**
	 * Get the opening angle of the rgb camera in y-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline double_t get_rgb_opening_angle_y_axis()
	{
		// look "learning opencv" book (page 372)
		return atan((get_rgb_height() / 2) / get_color_intrinsic()(1,1)) * 2;
	}

	/**
	 * Get the opening angle of the distance camera in x-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline double_t get_distance_opening_angle_x_axis()
	{
		// look "learning opencv" book (page 372)
		return atan((get_distance_width() / 2) / get_distance_intrinsic()(0,0)) * 2;
	}

	/**
	 * Get the opening angle of the distance camera in y-axis [rad].
	 * It is expected that the opening angle in positive and negative direction is the same.
	 */
	inline double_t get_distance_opening_angle_y_axis()
	{
		// look "learning opencv" book (page 372)
		return atan(( get_distance_height() / 2) / get_distance_intrinsic()(1,1)) * 2;
	}

	/**
	 * Get the intrinsic matrix of the color camera.
	 */
	inline const arma::mat& get_color_intrinsic()
	{

		if (color_intrinsic_mat.n_elem == 0)
		{
			arma::mat m(4, 4);
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; j++)
				{
					m(i, j) = idl_CommKinectImage.color_intrinsic_m[i*4 + j]; // _m is 1-dimension!
				}
			}
			color_intrinsic_mat = m;
		}

		return color_intrinsic_mat;
	}

	/**
	 * Get the intrinsic matrix of the distance camera.
	 */
	inline const arma::mat& get_distance_intrinsic()
	{

		if (dist_intrinsic_mat.n_elem == 0)
		{
			arma::mat m(4, 4);
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; j++)
				{
					m(i, j) = idl_CommKinectImage.dist_intrinsic_m[i*4 + j]; // _m is 1-dimension!
				}
			}

			dist_intrinsic_mat = m;
		}

		return dist_intrinsic_mat;
	}

	/**
	 * Get the extrinsic matrix between the cameras.
	 */
	inline const arma::mat& get_camera_extrinsic()
	{

		if (camera_extrinsic_mat.n_elem == 0)
		{
			arma::mat m(4, 4);
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; j++)
				{
					m(i, j) = idl_CommKinectImage.camera_extrinsic_m[i*4 + j]; // _m is 1-dimension!
				}
			}
			camera_extrinsic_mat = m;
		}

		return camera_extrinsic_mat;
	}

	/**
	 * Check if the image data is valid.
	 */
	inline bool is_data_valid() const
	{
		return idl_CommKinectImage.is_valid;
	}

	/**
	 * Returns the server sided sequence counter of this image.
	 * Use it to see if your client drops frames.
	 */
	inline ulong get_sequence_counter() const
	{
		return idl_CommKinectImage.seq_count;
	}

	/**
	 * Returns a pointer to rgb image
	 * The image has the size return from get_size().
	 */
	inline const uint8_t* get_rgb_image() const
	{
		return idl_CommKinectImage.rgb_data.get_buffer();
	}

	inline void set_rgb_image(const uint8_t* data, unsigned int width, unsigned int height)
	{
		idl_CommKinectImage.rgb_data.length(width * height * 3);
		idl_CommKinectImage.rgb_width = width;
		idl_CommKinectImage.rgb_height = height;

		memcpy(idl_CommKinectImage.rgb_data.get_buffer(), data, width * height * sizeof(uint8_t) * 3);
		idl_CommKinectImage.is_valid = true;
	}

	inline void set_rgb_image(const std::vector<uint8_t> &data, unsigned int width, unsigned int height)
	{
		idl_CommKinectImage.rgb_data.length(width * height * 3);
		idl_CommKinectImage.rgb_width = width;
		idl_CommKinectImage.rgb_height = height;

		for (uint32_t i = 0; i < idl_CommKinectImage.rgb_data.length(); i++)
		{
			idl_CommKinectImage.rgb_data[i] = data[i];
		}

		idl_CommKinectImage.is_valid = true;
	}

	/**
	 * Returns a pointer to the first element of the coordinate image.
	 * The image has the size return from get_distance_width()*get_distance_height().
     * items measured in meter.
	 */
	inline const float* get_distances() const {
		return idl_CommKinectImage.distance_data.get_buffer();
	}

	/**
	 * Return distance of a single pixel in the depth image. Error measures must be handled
     * by the caller. The distance of a error pixel is 0.
     * distances in meter.
     */
	inline float get_distance(const uint32_t posX, const uint32_t posY)
	{
		return *(get_distances() + (posY * get_distance_width()) + posX);
	}

	/**
	 * items measured in meter.
	 */
	inline void set_distances(const float* data, unsigned int width, unsigned int height)
	{
		idl_CommKinectImage.distance_data.length(width * height * 1);
		idl_CommKinectImage.distance_width = width;
		idl_CommKinectImage.distance_height = height;

		memcpy(idl_CommKinectImage.distance_data.get_buffer(), data, width * height * sizeof(float));
		idl_CommKinectImage.is_valid = true;
	}

	/**
         * items measured in meter.
	*/
	inline void set_distances(const std::vector<float> &data, unsigned int width, unsigned int height) {
		idl_CommKinectImage.distance_data.length(width * height * 1);
		idl_CommKinectImage.distance_width = width;
		idl_CommKinectImage.distance_height = height;

		for (uint32_t i = 0; i < idl_CommKinectImage.distance_data.length(); i++)
		{
			idl_CommKinectImage.distance_data[i] = data[i];
		}

		idl_CommKinectImage.is_valid = true;
	}

	/**
	 * Get the Cartesian 3d point in the sensor coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_cartesian_point_sensor(const uint32_t posX, const uint32_t posY, double& x, double& y, double& z,
			const double unit = 0.001) {

		if (color_inv_intrinsic_mat.n_elem == 0) {
			color_inv_intrinsic_mat = arma::inv(get_distance_intrinsic());
		}

		// get pointer to source position
		const float dist = *(get_distances() + (posY * get_distance_width()) + posX);

		// check if point in valid range, otherwise return false. values in dist in meter
		if(dist < get_min_distance(1) || dist > get_max_distance(1))
			return false;

		arma::vec q(4);
		q(0) = posX * dist;
		q(1) = posY * dist;
		q(2) = dist; // look "learning opencv" book (page 374)
		q(3) = 1;

		q = color_inv_intrinsic_mat * q; // inverse calculation of how to project a 3d point onto the image plane (page 374)

		x = q(0) / unit;
		y = q(1) / unit;
		z = q(2) / unit;

		return true;
	}

	/**
	 * Get the Cartesian 3d point in the robot coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_cartesian_point_robot(const uint32_t posX, const uint32_t posY, double& x, double& y, double& z,
			const double unit = 0.001) {

		if (robot_mat.n_elem == 0) {
			robot_mat = get_sensor_pose().getHomogeneousMatrix(1);
		}

		arma::vec q(4);
		if(get_cartesian_point_sensor(posX, posY, q(0), q(1), q(2), 1) == false)
			return false;

		q(3) = 1;

		arma::vec point = robot_mat * q;

		x = point(0) / unit;
		y = point(1) / unit;
		z = point(2) / unit;

		return true;
	}

	/**
	 * Get the Cartesian 3d point in the world coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_cartesian_point_world(const uint32_t posX, const uint32_t posY, double& x, double& y, double& z,
			const double unit = 0.001) {

		if (world_mat.n_elem == 0) {
			arma::mat pose_robot = get_base_state().get_base_position().get_base_pose3d().getHomogeneousMatrix(1);
			arma::mat pose = get_sensor_pose().getHomogeneousMatrix(1);

			world_mat = pose_robot * pose;
		}

		arma::vec q(4);
		if(get_cartesian_point_sensor(posX, posY, q(0), q(1), q(2), 1) == false)
			return false;

		q(3) = 1;

		arma::vec point = world_mat * q;

		x = point(0) / unit;
		y = point(1) / unit;
		z = point(2) / unit;
		return true;
	}

	/**
	 *  Get the spherical coordinate of a 3d point in the sensor coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_spherical_point_sensor(const uint32_t posX, const uint32_t posY, double &radius, double &phi,
			double &theta, const double unit = 0.001) {

		double x, y, z;
		if(get_cartesian_point_sensor(posX, posY, x, y, z, unit) == false)
			return false;

		// calculate the spherical coordinates
		radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		phi = atan2(y, x);
		theta = acos(z / radius);

		return true;
	}

	/**
	 *  Get the spherical coordinate of a 3d point in the robot coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_spherical_point_robot(const uint32_t posX, const uint32_t posY, double &radius, double &phi,
			double &theta, const double unit = 0.001) {

		double x, y, z;
		if(get_cartesian_point_robot(posX, posY, x, y, z, unit) == false)
			return false;

		// calculate the spherical coordinates
		radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		phi = atan2(y, x);
		theta = acos(z / radius);

		return true;
	}

	/**
	 *  Get the spherical coordinate of a 3d point in the world coordinate system.
	 * returns false if there is no valid point.
	 */
	inline bool get_spherical_point_world(const uint32_t posX, const uint32_t posY, double &radius, double &phi,
			double &theta, const double unit = 0.001) {

		double x, y, z;
		if(get_cartesian_point_world(posX, posY, x, y, z, unit) == false)
			return false;

		// calculate the spherical coordinates
		radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		phi = atan2(y, x);
		theta = acos(z / radius);

		return true;
	}

	/**
	 * Set the pose of the sensor relative to the robot coordinate system.
	 */
	inline void set_sensor_pose(const CommBasicObjects::CommPose3d& pose)
	{
		idl_CommKinectImage.sensor_pose = pose;

	}

	/**
	 * Set the state of the base.
	 */
	inline void set_base_state(const CommBasicObjects::CommBaseState& state)
	{
		idl_CommKinectImage.base_state = state;
	}

	/**
	 * Set the minimal distance the sensor works.
	 */
	inline void set_min_distance(const double_t value, const double_t unit = 0.001)
	{
		idl_CommKinectImage.min_distance = value * unit * 1000;
	}

	/**
	 * Set the maximal distance the sensor works.
	 */
	inline void set_max_distance(const double_t value, const double_t unit = 0.001)
	{
		idl_CommKinectImage.max_distance = value * unit * 1000;
	}

	/**
	 * Get the intrinsic matrix of the color camera.
	 */
	inline void set_color_intrinsic(const arma::mat& mat)
	{

		if (mat.n_cols != 4 && mat.n_rows != 4)
		{
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; j++)
			{
				idl_CommKinectImage.color_intrinsic_m[i*4 + j] = mat(i, j);
			}
		}
	}

	/**
	 * Get the intrinsic matrix of the distance camera.
	 */
	inline void set_distance_intrinsic(const arma::mat& mat)
	{

		if (mat.n_cols != 4 && mat.n_rows != 4)
		{
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; j++)
			{
				idl_CommKinectImage.dist_intrinsic_m[i*4 + j] = mat(i, j);
			}
		}

	}

	/**
	 * Get the extrinsic matrix between the cameras.
	 */
	inline void set_camera_extrinsic(const arma::mat& mat)
	{

		if (mat.n_cols != 4 && mat.n_rows != 4)
		{
			return;
		}

		arma::mat m(4, 4);
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; j++)
			{
				idl_CommKinectImage.camera_extrinsic_m[i*4 + j] = mat(i, j);
			}
		}

	}

	/**
	 * Set the valid flag of the data.
	 * The flag is automatically set to true if data is written to the image.
	 * Thus the server either writes data to the image or sets the valid.
	 */
	void set_data_valid(bool value)
	{
		idl_CommKinectImage.is_valid = value;
	}

	/**
	 * Set the sequence counter of the image.
	 */
	void set_sequence_counter(ulong n)
	{
		idl_CommKinectImage.seq_count = n;
	}

	/**
	 Return the distance image as a Comm3dPointCloud in robot coordinate system
	 */
	CommVisionObjects::Comm3dPointCloud get_as_3dPointCloud_robot()
	{
		CommVisionObjects::Comm3dPointCloud pointCloud;
		if(!is_data_valid())
		{
			pointCloud.set_valid(0);
			return pointCloud;
		} else
		{
			pointCloud.set_valid(1);
		}

		std::vector<double> cloudX;
		std::vector<double> cloudY;
		std::vector<double> cloudZ;

//		std::cout << "width:  " << get_width() << std::endl;
//		std::cout << "height: " << get_height() << std::endl;

		double x, y, z;


		for (uint32_t w = 0; w < get_distance_width(); w++) {
			for (uint32_t h = 0; h < get_distance_height(); h++) {
				//std::cout << "[" << i << "]: ";
				if(get_cartesian_point_robot(w, h, x, y, z, 1) == true) {
					//pointCloud.set_point(w * get_width() + h, x, y, z, 1);
					//std::cout << x << " " << y << " " << z << std::endl;
					//pointCloud.set_point(i++, x, y, z, 1);
					cloudX.push_back(x);
					cloudY.push_back(y);
					cloudZ.push_back(z);
				}
			}
		}

		pointCloud.set_size(cloudX.size());
		for(unsigned int i = 0; i < cloudX.size(); i++) {
			pointCloud.set_point(i, cloudX[i], cloudY[i], cloudZ[i], 1);
		}


		return pointCloud;
	}

	/**
	 Return the distance image as a Comm3dPointCloud in sensor coordinate system
	 */
	CommVisionObjects::Comm3dPointCloud get_as_3dPointCloud_sensor()
	{
		CommVisionObjects::Comm3dPointCloud pointCloud;
		if(!is_data_valid())
		{
			pointCloud.set_valid(0);
			return pointCloud;
		} else
		{
			pointCloud.set_valid(1);
		}

		std::vector<double> cloudX;
		std::vector<double> cloudY;
		std::vector<double> cloudZ;

//		std::cout << "width:  " << get_width() << std::endl;
//		std::cout << "height: " << get_height() << std::endl;

		double x, y, z;


		for (uint32_t w = 0; w < get_distance_width(); w++) {
			for (uint32_t h = 0; h < get_distance_height(); h++) {
				//std::cout << "[" << i << "]: ";
				if(get_cartesian_point_sensor(w, h, x, y, z, 1) == true) {
					//pointCloud.set_point(w * get_width() + h, x, y, z, 1);
					//std::cout << x << " " << y << " " << z << std::endl;
					//pointCloud.set_point(i++, x, y, z, 1);
					cloudX.push_back(x);
					cloudY.push_back(y);
					cloudZ.push_back(z);
				}
			}
		}

		pointCloud.set_size(cloudX.size());
		for(unsigned int i = 0; i < cloudX.size(); i++) {
			pointCloud.set_point(i, cloudX[i], cloudY[i], cloudZ[i], 1);
		}


		return pointCloud;
	}

	/**
	 * Save an XML like representation of this image to the given output stream.
	 */
	void save_xml(std::ostream &os, const std::string &indent = "");

	/**
	 * Load from XML represenation on an istream.
	 * Be warned, no error checking is done.
	 * For details, see \ref smart_basic_com-xml.
	 */
	void load_xml(std::istream &is);

};

}
#endif
