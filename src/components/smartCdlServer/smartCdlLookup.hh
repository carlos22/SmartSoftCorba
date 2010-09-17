// --------------------------------------------------------------------------
//
//  Copyright (C) 1998 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft CDL component".
//  It provides navigation services based on the CDL
//  Curvature Distance Lookup approach.
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

#ifndef _SMARTCDLLOOKUP_HH
#define _SMARTCDLLOOKUP_HH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include "smartCdlDebug.h"

//#include "smartLaserClient.hh"
#include "commMobileLaserScan.hh"

#include "smartCdlCons.h"
#include "smartCdlTypes.h"
//#include "smartCdlTags.h"
#include "smartCdlDefine.h"

class CdlLookupClass
  // = TITLE
  //     This is the online part of the new steer angle field
  //     algorithm which needs access to the precalculated
  //     values of the curvature index and the distance values.
{
protected:

#if DEBUG_VW_WINDOW
//  SmartLaserScanClass currentScan;
#endif

  int distLookup[CDL_MAX_X_CELLS][CDL_MAX_Y_CELLS][CDL_MAX_CURVATURE];
  // this array contains the remaining distance information

  int alphaLookup[CDL_MAX_X_CELLS][CDL_MAX_Y_CELLS][CDL_MAX_CURVATURE];
  // this array contains the remaining angle information in rad*1000.0

  double rotAccLookup[CDL_MAX_CURVATURE];
  // this array contains the maximum allowed rotational acceleration

  double transAccLookup[CDL_MAX_CURVATURE];
  // this array contains the maximum allowed translational acceleration

  int indexVW[CDL_MAX_TRA_CELLS][CDL_MAX_ROT_CELLS];
  // contains curvature index corresponding to v,w

  struct evaluationStruct {
    double costHit;
    double costPassing;
    double costSpeed;
    double costDistance;
    double costHeading;
    double costValue;
  } evaluation[CDL_MAX_TRA_CELLS][CDL_MAX_ROT_CELLS];
  // contains calculated costs of this curvature

  int indexFlag[CDL_MAX_CURVATURE];
  // indicates whether this index has already been inserted into indexList

  int indexWanted[CDL_MAX_CURVATURE];
  // for test purposes only (CDL_EVAL_APPROACH_EXACT) !!!!
  // 0 this curvature is not wanted
  // 1 this curvature is ok
  // this array is used to indicate, whether this curvature should be used
  // or not

  int indexList[CDL_MAX_CURVATURE];
  // list of indizes in the current velocity window

  int distanceList[CDL_MAX_CURVATURE];
  // the remaining distances with respect to the curvature index

  int alphaList[CDL_MAX_CURVATURE];
  // the remaining rotation angle with respect to the curvature index
  // in [rad * 1000]

  //LaserScanCartesianType scan;
  Smart::CommMobileLaserScan scan;
  // Parameter: the current laserscan used for obstacle avoidance

  double rangeFarPassing, rangeMediumPassing;
  double speedFarPassing, speedMediumPassing, speedNearPassing;
  double bonusDistPassing;
  double rangeFarStopping, rangeMediumStopping;
  double speedFarStopping, speedMediumStopping, speedNearStopping;
  double bonusDistStopping;
  // Parameter: distance dependent velocities, used within
  // CDL_EVAL_PASSING, CDL_EVAL_STOPPING

  double goalHeadingRelative;
  // Parameter: the driving direction relative to the robot frame

  double maxDistance;
  // Parameter: the maximum considered remaining free path length
  // which yields to a rating of 1.0 in the objective function
  // for the distance term. Maximum value is CDL_MAX_DISTANCE

  double desiredTranslationalSpeed;
  // Parameter: the translational speed which should be selected

  double alpha1,alpha2,alpha3;
  // Parameter: the weighting of the different components of the
  // objective function

  double deltat;
  // Parameter: time intervall for the next speed selection

  double vacc,wacc;
  // Parameter: the allowed accelerations

  double goalX, goalY;
  // Parameter: the goal point which should be reached

  double safetyClearance;
  // Parameter: the global safety clearance

  int intersectionsCircleLine(double,double,double,
                              double,double,double,double,
                              double&,double&,double&,double&,int&);
  // calculate number of intersections and points of intersection
  // of a circle with a line. This function is needed internally 
  // to check whether the selected curvature hits the goal point. 

  double min00(double,double);
  double max00(double,double);
  double rad00(double);
  double deg00(double);
  double abs00(double);
  double angle00(double);

public:
  // = Initialization and termination

  CdlLookupClass();
  // Default constructor

  ~CdlLookupClass();
  // Implicitly destroy the class

  // = User interface functions.
  // All methods return status information. If status if 0, everything
  // is OK, else an error occured.

  int setParameterWeighting(double alpha1,double alpha2,double alpha3);
  // Sets the weighting parameters for the different components of
  // the objective function

  int setParameterRobot(double deltat,double vacc,double wacc);
  // Sets the robots parameter which are relevant for the lookup-phase

  int setEvalPassing(double,double,double,double,double,double);
  // rangeFar, rangeMedium, speedFar, speedMedium, speedNear, bonusDist

  int setEvalStopping(double,double,double,double,double,double);
  // rangeFar, rangeMedium, speedFar, speedMedium, speedNear, bonusDist

  int setHeading(double heading);
  // Set the goal heading relative to the robot frame.
  // (Attention: is it different to the SmartSafAmos ?)

  double getHeading();
  // Get the goal heading relative to the robot frame.

  void setSafetyClearance(double sc);
  // Set the global safety clearance.

  int setMaxDistance(double distance);
  // Set the maximum considered free path length for the distance
  // term of the objective function

  int setDesiredTranslationalSpeed(double speed);
  // Set the desired speed which should be chosen by the objective function

  double getDesiredTranslationalSpeed();
  // Get the desired speed which is chosen by the objective function

  //int setLaserscan(SmartLaserScanClass &laserscan);
  int setLaserscan( Smart::CommMobileLaserScan &laserscan);
  // Put current laserscan into object to calculate remaining distances

  int setGoalPosition(double x,double y);
  // Set the goal coordinates which are used to calculate the deviation
  // within the cost function

  int calculateSpeedValues(double v,double w,
                           double posX,double posY,double posA,
                           double vmin,double vmax,double wmin,double wmax,
                           CdlStrategyType strategy,
                           CdlEvalFunctionType evalFunc,
                           double &vResult,double &wResult,
                           double &vAccResult,double &wAccResult);
  // calculates the next speed values taking into account the allowed
  // min/max velocities

  int freeBehavior(double,int&);
  // Used to free the robot when stalled.
  // securityDist, turnDirection

  int loadCurvatureIndexAscii(char *filename);
  // load curvature index file

  int loadDistAngleLookupAscii(char *filename);
  // load lookup file for the distance and angle values

  int loadDistAngleLookupBin(char *filename);
  // load lookup file for the distance and angle values

  int loadAccLookupBin(char *filename);
  // load lookup file for the maximum accelerations with respect 
  // to the curvature

};

#endif

