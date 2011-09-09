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

#include "features.hh"
#include "CommBasicObjects/commMobileLaserScan.hh"

double angle00(double a)
{
  if (a < -M_PI || a >= M_PI)
  {
    a = a - 2*M_PI * floor(a / (2*M_PI));
    while(a < -M_PI) a += 2*M_PI;
    while(a >= M_PI) a -= 2*M_PI;
  }

  return(a);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions to calculate features


// feature 1:  diameter
double getDiameter(CommBasicObjects::CommMobileLaserScan &scan, segment &seg)
{
  double right_x, right_y, left_x, left_y;
  scan.get_scan_cartesian_point_world( seg.begin, right_x, right_y );
  scan.get_scan_cartesian_point_world( seg.end, left_x, left_y );
  return sqrt( (left_x-right_x)*(left_x-right_x) + (left_y-right_y)*(left_y-right_y) );
}

// feature 2: circle pre
double getCirclePre(CommBasicObjects::CommMobileLaserScan &scan, segment &seg)
{
  int middle_index = (seg.end + seg.begin ) /2;
  double middle_x, middle_y;
  scan.get_scan_cartesian_point_scanner( middle_index, middle_x, middle_y );

  double right_x, right_y, left_x, left_y;
  scan.get_scan_cartesian_point_scanner( seg.begin, right_x, right_y );
  scan.get_scan_cartesian_point_scanner( seg.end, left_x, left_y );

  middle_x -= left_x;
  middle_y -= left_y;

  float angle_to_rotate = atan2( (left_x-right_x)/(left_y-right_y), 1);
  return middle_x*cos(angle_to_rotate) - middle_y*sin(angle_to_rotate);
}


// feature 3: number of points
int getNumPoints(CommBasicObjects::CommMobileLaserScan &scan, segment &seg)
{
  return seg.end - seg.begin;
}


// feature 4: get mean of surface of curvature
double getMeanSurfCurv(CommBasicObjects::CommMobileLaserScan &scan, segment &seg)
{
  int count = seg.end - seg.begin;
  double rtrn = 0;
  for( int i=1; i<count; i++)
  {
    int middle_index = seg.begin + i;
    double middle_x, middle_y;
    scan.get_scan_cartesian_point_scanner( middle_index, middle_x, middle_y );

    double right_x, right_y, left_x, left_y;
    scan.get_scan_cartesian_point_scanner( seg.begin, right_x, right_y );
    scan.get_scan_cartesian_point_scanner( seg.end, left_x, left_y );

    middle_x -= left_x;
    middle_y -= left_y;

    float angle_to_rotate = atan2( (left_x-right_x)/(left_y-right_y), 1);
    rtrn += middle_x*cos(angle_to_rotate) - middle_y*sin(angle_to_rotate);
  } // for

  return rtrn/(count-1);
}


//feature 5: mean inscribed angle
double getMeanInscribedAngle(CommBasicObjects::CommMobileLaserScan &scan, segment &seg)
{
  int count = seg.end - seg.begin -1;
  double alphaSum = 0;

  for(int i=0; i<count; i++)
  {
    double p1x, p1y, p2x, p2y, p3x, p3y;
    double alpha, alpha1, alpha2;
    scan.get_scan_cartesian_point_scanner( seg.begin+i,   p1x, p1y );
    scan.get_scan_cartesian_point_scanner( seg.begin+i+1, p2x, p2y );
    scan.get_scan_cartesian_point_scanner( seg.begin+i+2, p3x, p3y );

    p1x -= p2x;
    p1y -= p2y;
    p3x -= p2x;
    p3y -= p2y;
    p2x -= p2x;
    p2y -= p2y;

    alpha1 = atan2( p1y-p2y, p1x-p2x );
    alpha2 = atan2( p3y-p2y, p3x-p2x );
    alpha1 = angle00(alpha1);
    alpha2 = angle00(alpha2);

    alpha = alpha2 - alpha1;
    alpha = angle00(alpha);

    alphaSum += alpha;
  }

  return alphaSum/count;

}
