// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft laser converter component".
//  It provides mapping services based on grid maps. The current map
//  represents the latest snapshot of the local surrounding based on
//  laserscans. The current map can be preoccupied by the longterm map.
//  The longterm map is a simple occupancy grid map.
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

#ifndef SMART_CONVERTIBLE_COMM_MOBILE_LASER_SCAN_HH
#define SMART_CONVERTIBLE_COMM_MOBILE_LASER_SCAN_HH

// includes
#include "commMobileLaserScan.hh"

#include "smartLaserXfig.hh"

namespace Smart 
{

  class SmartConvertibleMobileLaserScan : public CommMobileLaserScan
  {

  public:
    /**
     *  Save mobile laser scan (Steffens format).
     *  flag 0 -> Steffen/CHS SmartSoft
     *  flag 1 -> Steffen/ScanStudio
     */
    int save_formatSteffen( char *filename , int flag ) const;

    /**
     *  Load mobile laser scan (Steffens format).
     *  flag 0 -> Steffen/CHS SmartSoft
     *  flag 1 -> Steffen/ScanStudio
     */
    bool load_formatSteffen( char *filename, int flag );

    /**
     *  Save mobile laser scan in xFif format.
     */
    void save_xFig(char *filename ) const;

    /**
     *  Save mobile laser scan in xFif format.
     */
    void save_xFig( SmartLaserXfigClass *drawing ) const;

  private:
    double NormAngle(double a);

  }; // class


} // namespace Smart

#endif
