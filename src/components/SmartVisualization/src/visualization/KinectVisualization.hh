/*
 * KinectVisualization.h
 *
 *  Created on: Jul 6, 2011
 *      Author: wopfner
 */

#ifndef KINECTVISUALIZATION_HH_
#define KINECTVISUALIZATION_HH_

#include "AbstractVisualization.hh"

#include <CommVisionObjects/commKinectImage.hh>

class KinectVisualization: public AbstractVisualization
{
private:
	struct ColPoint3d
	{
		ColPoint3d(float x, float y, float z, float r, float g, float b)
		{
			this->x = x;
			this->y = y;
			this->z = z;

			this->r = r;
			this->g = g;
			this->b = b;
		}

		float x, y, z;
		float r, g, b;
	};

public:
	KinectVisualization(CDisplayWindow3D& window3D, const std::string& identifier);
	virtual ~KinectVisualization();

	void displayImage(CommVisionObjects::CommKinectImage& image);
	void clear();
};

#endif /* KINECTVISUALIZATION_HH_ */
