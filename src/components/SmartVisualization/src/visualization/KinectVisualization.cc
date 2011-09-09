/*
 * KinectVisualization.cpp
 *
 *  Created on: Jul 6, 2011
 *      Author: wopfner
 */

#include "KinectVisualization.hh"

KinectVisualization::KinectVisualization(CDisplayWindow3D& window3D, const std::string& identifier) :
	AbstractVisualization(window3D, identifier)
{
	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudColouredPtr cloud = opengl::CPointCloudColoured::Create();
		cloud->setName(identifier + "_cloud");
		cloud->setPointSize(2.0);
		ptrScene->insert(cloud);
	}
	window3D.unlockAccess3DScene();
}

KinectVisualization::~KinectVisualization()
{
}

void KinectVisualization::displayImage(CommVisionObjects::CommKinectImage& image)
{
	if (image.is_data_valid())
	{
		// calculate points and colors
		arma::vec q(4);
		q(3) = 1;

		double x, y, z;
		float r = 0, g = 0, b = 0;
		std::vector<ColPoint3d> points;
		const arma::mat& project = image.get_color_intrinsic() * image.get_camera_extrinsic();
		const uint8_t* rgbImg = image.get_rgb_image();

		for (uint32_t w = 0; w < image.get_distance_width(); w++)
		{
			for (uint32_t h = 0; h < image.get_distance_height(); h++)
			{
				if (image.get_distance(w, h) >= image.get_min_distance(1))
				{
					image.get_cartesian_point_sensor(w, h, q(0), q(1), q(2), 1);
					arma::vec p = project * q;
					p /= p(2);

					uint32_t px = p(0);
					uint32_t py = p(1);

					if (px >= 0 && px < image.get_rgb_width() && py >= 0 && py < image.get_rgb_height())
					{
						r = (rgbImg + 3* image .get_rgb_width() * py)[px * 3];
						g = (rgbImg + 3* image .get_rgb_width() * py)[px * 3 + 1];
						b = (rgbImg + 3* image .get_rgb_width() * py)[px * 3 + 2];
					}

					image.get_cartesian_point_world(w, h, x, y, z, 1);
					points.push_back(ColPoint3d(x, y, z, r / 255.0, g / 255.0, b / 255.0));
				}
			}
		}

		opengl::COpenGLScenePtr & ptrScene = window3D.get3DSceneAndLock();
		//////////////////////////////////////////
		// show coordinates
		{
			opengl::CPointCloudColouredPtr cloud = (CPointCloudColouredPtr) ptrScene->getByName(identifier + "_cloud");
			cloud->clear();
			for (uint32_t i = 0; i < points.size(); i++)
			{
				cloud->push_back(points[i].x, points[i].y, points[i].z, points[i].r, points[i].g, points[i].b);
			}

		}
		window3D.unlockAccess3DScene();
		window3D.forceRepaint();
	}
}

void KinectVisualization::clear()
{
	opengl::COpenGLScenePtr & ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudColouredPtr cloud = (CPointCloudColouredPtr) ptrScene->getByName(identifier + "_cloud");
		cloud->clear();
	}
	window3D.unlockAccess3DScene();
	window3D.forceRepaint();
}
