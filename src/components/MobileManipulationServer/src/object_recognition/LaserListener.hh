//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//	Contact:
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "Katana Mobile Manipulation component".
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


#ifndef LASERLISTENER_H_
#define LASERLISTENER_H_

#include <mrpt/slam/CObservation2DRangeScan.h>

using mrpt::slam::CObservation2DRangeScan;

namespace mmp
{
	/**
	 * Interface for all classes which want to be informed when a laserscan is taken
	 */
	class LaserListener
	{
		public:
			/**
			 * Method handles a laser scan object.
			 * @param scan The laser scan object.
			 */
			virtual void handleLaserScan(const CObservation2DRangeScan& scan) = 0;
	};
}

#endif /* LASERLISTENER_H_ */
