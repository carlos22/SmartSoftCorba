//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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
//
//  Copyright (C) 2011 Jonas Brich, Manuel Wopfner
//
//        brich@mail.hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "IDS uEye Video Server component".
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
#ifndef _IMAGETASK_HH
#define _IMAGETASK_HH

#include "gen/ImageTaskCore.hh"
#include <CommBasicObjects/commPose3d.hh>
#include <CommBasicObjects/commBaseState.hh>
#include <CommVisionObjects/commVideoImage.hh>

class ImageTask: public ImageTaskCore
{

public:
	ImageTask();
	virtual ~ImageTask();

	int svc();

	/**
	 * creates the needed image buffer for the current camera parameter set.
	 */
	void createImageBuffer();


private:
	unsigned int _image_buffer_index;
	std::vector<CommVisionObjects::CommVideoImage*> _image_buffer;
};

#endif
