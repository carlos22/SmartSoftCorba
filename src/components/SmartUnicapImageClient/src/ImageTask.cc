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


// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Matthias Lutz
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft SpeechOutput component".
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


#include "ImageTask.hh"
#include "gen/SmartUnicapImageClient.hh"

#include <iostream>

ImageTask::ImageTask()
{
	std::cout << "constructor ImageTask\n";
}

int ImageTask::svc()
{
	// do something -- put your code here !!!
	while (1)
	{

		CHS::StatusCode req_status;
		CommVisionObjects::CommVideoImage image;
		CommBasicObjects::CommVoid dummy;
		//static unsigned char *buffer = 0;

		std::cout << "[ImageTask] Getting Image ...\n";

		req_status = COMP->queryClient->query(dummy,image);

		//CHS::QueryId id;
		//req_status = COMP->queryClient->queryRequest(dummy,id);
		//req_status = COMP->queryClient->queryReceiveWait(id,image);

		//req_status = COMP->pushNewestClient->getUpdateWait(image);

		std::cout << "[ImageTask] status: " << CHS::StatusCodeConversion(req_status) << std::endl;

		std::cout << "Is data valid?: " << (bool) image.is_data_valid() << std::endl;
		std::cout << "Height: " << image.get_height() << std::endl;
		std::cout << "Width: " << image.get_width() << std::endl;
		std::cout << "Format: " << image.get_format() << std::endl;

		CHS::SmartGuard imgGuard(COMP->CurrentImageMutex);
		{
			cvReleaseImage(&COMP->currentImage);
			COMP->currentImage = NULL;
			COMP->currentImage = COMP->convertDataArrayToIplImage(image, cvSize(image.get_width(), image.get_height()));

			if (COMP->currentImage == NULL)
				std::cout << "[ImageTask] Current Image NOT set!" << std::endl;
			else {
				std::cout << "[ImageTask] Current Image set!" << std::endl;


				std::cout << "Sensor Pose: " << image.get_sensor_pose() << std::endl;

			}

		}
		imgGuard.release();
		//	unsigned int size = image.get_size_as_rgb32();
		//	buffer = new unsigned char[size];
		//	get_as_rgb32(buffer);

		std::cout << "[ImageTask] sleep ...\n";
		usleep(20000);

	}
	return 0;
}