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
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// --------------------------------------------------------------------------

#include "GuiTask.hh"
#include "gen/SmartUnicapImageClient.hh"

#include <iostream>

GuiTask::GuiTask()
{
	std::cout << "constructor GuiTask\n";
}

int GuiTask::svc()
{
	// do something -- put your code here !!!
		cvNamedWindow("out");
 while(true)
 {
		//std::cout<<"Is data valid?: "<<image.is_data_valid()<<std::endl;
		//std::cout<<"Height: "<<image.get_height()<<std::endl;

	    CHS::SmartGuard imgGuard(COMP->CurrentImageMutex);
	 	{
	 		if(COMP->currentImage!=NULL){
				cvShowImage("out",COMP->currentImage);
				//std::cout << "[GuiTask] sleep \n";
				//cvSaveImage("curr_img.ppm", currentImage);
					//usleep(200000);
	 		}else
	 		{
	 			std::cout<<"[GuiTask] Error no image to show!"<<std::endl;
	 		}
	 	}
	 	imgGuard.release();
		std::cout << "[GuiTask] sleep ...\n";
		cvWaitKey(10);
 }

	return 0;
}