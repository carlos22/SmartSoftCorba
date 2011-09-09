/*
 * PersonDetectionVisualization.cc
 *
 *  Created on: Jul 6, 2011
 *      Author: wopfner
 */

#include "PersonDetectionVisualization.hh"

PersonDetectionVisualization::PersonDetectionVisualization(CDisplayWindow3D& window3D, const std::string& identifier) :
	AbstractVisualization(window3D, identifier)
{
	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = opengl::CPointCloud::Create();
		cloud->setName(identifier + "_cloud");
		cloud->setColor(mrpt::utils::TColorf(0.83, 0.33));
		cloud->setPointSize(5.0);
		ptrScene->insert(cloud);

		opengl::CSetOfObjectsPtr text = opengl::CSetOfObjects::Create();
		text->setName(identifier + "_text");
		ptrScene->insert(text);
	}
	window3D.unlockAccess3DScene();
}

PersonDetectionVisualization::~PersonDetectionVisualization()
{
}

void PersonDetectionVisualization::displayPersons(std::vector<CommPersonDetectionObjects::CommDetectedPerson>& persons)
{

	opengl::COpenGLScenePtr &ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = (CPointCloudPtr) ptrScene->getByName(identifier + "_cloud");
		opengl::CSetOfObjectsPtr textObj = (CSetOfObjectsPtr) ptrScene->getByName(identifier + "_text");

		for (size_t i = 0; i < persons.size(); ++i)
		{
			CommBasicObjects::CommPose3d p = persons[i].get_pose_world();

			// add point
			CommBasicObjects::CommPose3d pose = persons[i].get_pose_world();
			cloud->insertPoint(pose.get_x(1.0), pose.get_y(1.0), pose.get_z(1.0));

			// add label
			CPose3D textPose(pose.get_x(1.0) - 0.1, pose.get_y(1.0), pose.get_z(1.0) - 0.1, 0, 0, 0);
			std::stringstream label;
			label << "id=" << persons[i].get_id() << "(" << p.get_x(1) << ", " << p.get_y(1) << ", " << p.get_z(1) << ")";

			opengl::CTextPtr text = opengl::CText::Create();
			text->setPose(textPose);
			text->setString(label.str());
			textObj->insert(text);
		}
	}
	window3D.unlockAccess3DScene();
	window3D.forceRepaint();
}

void PersonDetectionVisualization::clear()
{
	opengl::COpenGLScenePtr & ptrScene = window3D.get3DSceneAndLock();
	{
		opengl::CPointCloudPtr cloud = (CPointCloudPtr) ptrScene->getByName(identifier + "_cloud");
		cloud->clear();

		opengl::CSetOfObjectsPtr text = (CSetOfObjectsPtr) ptrScene->getByName(identifier + "_text");
		text->clear();
	}
	window3D.unlockAccess3DScene();
	window3D.forceRepaint();
}
