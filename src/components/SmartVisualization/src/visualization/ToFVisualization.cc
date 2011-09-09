// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Manuel Wopfner
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
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
// --------------------------------------------------------------------------

#include "ToFVisualization.hh"

ToFVisualization::ToFVisualization(CDisplayWindow3D& window3D, const std::string& identifier) :
	AbstractVisualization(window3D, identifier)
{
	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = opengl::CPointCloud::Create();
		cloud->setName(identifier + "_cloud");
		cloud->setColor(mrpt::utils::TColorf(1));
		cloud->setPointSize(2.0);
		ptrScene->insert(cloud);
	}
	window3D.unlockAccess3DScene();
}

ToFVisualization::~ToFVisualization()
{
}

void ToFVisualization::displayToFImage(CommVisionObjects::CommToFImage& image)
{

	double x, y, z;
	std::vector<float> vx, vy, vz;

	for (uint32_t i = 0; i < image.get_width(); i++)
	{
		for (uint32_t j = 0; j < image.get_height(); j++)
		{
			image.get_cartesian_point_world(i, j, x, y, z, 1);

			if (0.4 > image.get_measurement_accuracy(i, j))
			{
				vx.push_back(x);
				vy.push_back(y);
				vz.push_back(z);
			}
		}
	}

	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = (CPointCloudPtr) ptrScene->getByName(identifier + "_cloud");
		cloud->setAllPoints(vx, vy, vz);
	}
	window3D.unlockAccess3DScene();
	window3D.forceRepaint();
}

void ToFVisualization::clear()
{
	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = (CPointCloudPtr) ptrScene->getByName(identifier + "_cloud");
		cloud->clear();
	}
	window3D.unlockAccess3DScene();
	window3D.forceRepaint();
}
