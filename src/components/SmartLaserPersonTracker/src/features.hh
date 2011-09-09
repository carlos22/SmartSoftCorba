// --------------------------------------------------------------------------
//
//  Copyright (C) 2009-2010 Andreas Steck, Matthias Lutz
//
//        lutz@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
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

#ifndef _FEATURES_HH
#define _FEATURES_HH

#include "CommBasicObjects/commMobileLaserScan.hh"


struct segment
{
  segment(int in_begin=0, int in_end=0, bool in_is2legs=false) : begin(in_begin),end(in_end),is2legs(in_is2legs)  {}
  int begin;
  int end;
  bool is2legs;
};

double angle00(double a);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions to calculate features

// feature 1:  diameter
double getDiameter(CommBasicObjects::CommMobileLaserScan &scan, segment &seg);

// feature 2: circle pre
double getCirclePre(CommBasicObjects::CommMobileLaserScan &scan, segment &seg);

// feature 3: number of points
int getNumPoints(CommBasicObjects::CommMobileLaserScan &scan, segment &seg);

// feature 4: get mean of surface of curvature
double getMeanSurfCurv(CommBasicObjects::CommMobileLaserScan &scan, segment &seg);

//feature 5: mean inscribed angle
double getMeanInscribedAngle(CommBasicObjects::CommMobileLaserScan &scan, segment &seg);



#endif
