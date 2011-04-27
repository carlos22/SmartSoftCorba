//--------------------------------------------------------------------------
//
//  Copyright (C) 1997 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Xfig Tools".
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------


#ifndef _LASERTYPES_H
#define _LASERTYPES_H

#include <sys/time.h>

/*
 * These types are used for communication purposes
 */

#define MAX_NUMBER_OF_SCAN_POINTS	721

typedef struct {
  double angle;
  short  distance;
  short  blendung;
  short  feld_a;
  short  feld_b;
} LaserScanPointType;

typedef struct {
  int                   dimension;
  LaserScanPointType    point[MAX_NUMBER_OF_SCAN_POINTS];
} LaserScanArrayType;

typedef struct {
  int			valid;              // <>0 data not valid, 0 data valid
  double		robotPosX;          // robot position in world coordsys
  double		robotPosY;          // mm,mm,mm,rad
  double		robotPosZ; 
  double		robotPosA;
  double                scanPosX;           // Scannerposition in world coordsys
  double                scanPosY;           // mm,mm,mm,rad
  double		scanPosZ;
  double                scanPosA;
  double		posSigma[3][3];
  unsigned long         updateCnt;
  struct timeval	time;                
  LaserScanArrayType    scan;
} LaserScanType;

/*
 * These types are used for the C-Interface-Functions.
 */

typedef struct {
  double  		distance;		// distance in mm
  double  		angle;			// angle relative to scanner
} LaserScanPolarPointType;

typedef struct {
  double 		x;			// x-ccordinate
  double		y;			// y-coordinate
  double		z;			// z-coordinate
} LaserScanCartesianPointType;

typedef struct {
  double         robotPosX;			// robot position in mm
  double         robotPosY;
  double         robotPosZ;
  double         robotPosA;			// robot heading in rad
  double         scanPosX;			// scanner position in
  double         scanPosY;			// world coordinate system
  double         scanPosZ;			// [mm,mm,mm,rad]
  double         scanPosA;
  double         posSigma[3][3];
  unsigned long  updateCnt;
  int            numberOfPoints;
  struct timeval time;
  LaserScanPolarPointType scan[MAX_NUMBER_OF_SCAN_POINTS];
} LaserScanPolarType;

typedef struct {
  double         robotPosX;			// robot position in mm
  double         robotPosY;
  double         robotPosZ;
  double         robotPosA;			// robot heading in rad
  double         scanPosX;			// scanner position in
  double         scanPosY;			// world coordinate system
  double         scanPosZ;			// [mm,mm,mm,rad]
  double         scanPosA;
  double         posSigma[3][3];
  unsigned long  updateCnt;
  int            numberOfPoints;
  struct timeval time;
  LaserScanCartesianPointType scan[MAX_NUMBER_OF_SCAN_POINTS];
} LaserScanCartesianType;
  
#endif

