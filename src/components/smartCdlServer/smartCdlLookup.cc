// --------------------------------------------------------------------------
//
//  Copyright (C) 1998/2008 Christian Schlegel, Andreas Steck
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

#include "smartCdlLookup.hh"


// <asteck: date="21.07.2008">
//#define EVAL_FUNC_3 1
// </asteck>

CdlLookupClass::CdlLookupClass()
{
// constructor
};

CdlLookupClass::~CdlLookupClass()
{
// destructor
};

double CdlLookupClass::min00(double a,double b)
{
  if (a<b) return a;
  else return b;
}

double CdlLookupClass::max00(double a,double b)
{
  if (a<b) return b;
  else return a;
}

double CdlLookupClass::rad00(double a)
{
  return (a*M_PI/180.0);
}

double CdlLookupClass::deg00(double a)
{
  return (a*180.0/M_PI);
}

double CdlLookupClass::abs00(double a)
{
  if (a<0) return (-a);
  else return a;
}

double CdlLookupClass::angle00(double a)
{
  if (a < -M_PI || a >= M_PI)
    {
      a = a - 2*M_PI * floor(a / (2*M_PI));
      while (a < -M_PI) a += 2*M_PI;
      while (a >= M_PI) a -= 2*M_PI;
    }

  return(a);
}

int CdlLookupClass::intersectionsCircleLine
(double xm,double ym,double r,
 double pax,double pay,double pbx,double pby,
 double& x1,double& y1,double& x2,double& y2,
 int& ns)
{
  double deltax,deltay;
  double a,b,c;
  double ab2,dis,cc,wu;

  // convert line from 2 point description to ax+by=c
  deltax = pbx - pax;
  deltay = pby - pay;
  if (abs00(deltax) < CDL_ACCURACY)
    {
      // parallel to y
      a = 1.0;
      b = 0.0;
      c = pax;
    }
  else if (abs00(deltay) < CDL_ACCURACY)
    {
      // parallel to x
      a = 0.0;
      b = 1.0;
      c = pay;
    }
  else if ((abs00(deltay)<CDL_ACCURACY) && (abs00(deltax)<CDL_ACCURACY))
    {
      // distance A <-> B too small
      return 1;
    }
  else
    {
      a = -deltay/deltax;
      b = 1.0;
      c = a * pax + b * pay;
    }

  // calculate nr of intersections of line with circle
  ns  = 0;
  ab2 = a*a + b*b;
  if (ab2 != 0.0)
    {
      cc = c - a*xm - b*ym;
      dis = r*r*ab2 - cc*cc;
      if (dis >= 0.0)
        {
          ns = 2;
          wu = sqrt(dis);
          if (wu == 0.0) ns=1;
          if (b >= 0.0)
            {
              x1 = xm + (a*cc-b*wu)/ab2;
              y1 = ym + (b*cc+a*wu)/ab2;
              x2 = xm + (a*cc+b*wu)/ab2;
              y2 = ym + (b*cc-a*wu)/ab2;
            }
          else
            {
              x2 = xm + (a*cc-b*wu)/ab2;
              y2 = ym + (b*cc+a*wu)/ab2;
              x1 = xm + (a*cc+b*wu)/ab2;
              y1 = ym + (b*cc-a*wu)/ab2;
            }
        }
    }
  return 0;
}


int CdlLookupClass::freeBehavior(double securityDist,int &turnDirection)
{
  //<asteck: date="17.07.2008" type of variable changed to unsigned>
  unsigned int    index;
  // </asteck>
  int    occupiedRight;
  int    occupiedLeft;
  double distance;

  occupiedRight = 0;
  occupiedLeft  = 0;

  //<asteck: date="17.07.2008" variables added>
  double x,y;
  //</asteck>

  // examine left and right side of the robot
  for (index=0;index<scan.get_scan_size();index++)
    {
      //<asteck: date="17.07.2008" some modifications due to other Laserscan-Object>
      scan.get_scan_cartesian_point_scanner( index, x, y );
      distance = sqrt((x * x)
                      +(y * y));
      if ((y < 0.0) && (distance < securityDist))
        {
          occupiedRight = 1;
        }
      if ((y > 0.0) && (distance < securityDist))
        {
          occupiedLeft = 1;
        }
      //</asteck>
    }

  // now decide what to do next
  if ((occupiedLeft == 0) && (occupiedRight == 1))
    {
      // left free, right occupied ==> turn left
      turnDirection = CDL_FREE_TURN_LEFT;
    }
  else if ((occupiedLeft == 1) && (occupiedRight == 0))
    {
      // left occupied, right free ==> turn right
      turnDirection = CDL_FREE_TURN_RIGHT;
    }
  else if ((occupiedLeft == 1) && (occupiedRight == 1))
    {
      // both sides occupied ==> no turn, too dangerous
      turnDirection = CDL_FREE_NO_TURN;
    }
  else
    {
      // both sides free, this is strange, but then turn left
      turnDirection = CDL_FREE_BOTH;
    }
  return 0;
}


int CdlLookupClass::calculateSpeedValues(
  double v,double w,
  double posX,double posY,double posA,
  double vminIni,double vmaxIni,
  double wminIni,double wmaxIni,
  CdlStrategyType strategy,
  CdlEvalFunctionType evalFunc,
  double &vResult,double &wResult,
  double &vAccResult,double &wAccResult)
{
  double cdl_v_rot_min;
  double cdl_v_rot_max;
  double cdl_v_rot_step;
  double vmin,vmax;
  double wmin,wmax;
  int    ivmin,ivmax;
  int    iwmin,iwmax;

  int indexCurvature;
  int vi,wi,ci,li;

  int ix,iy;

//<asteck: date="17.07.2008">
  unsigned int scanindex;
  // initialize
  double costPassing = 0.0;
  double costValue   = 0.0;
//</asteck>

  double costHit;
  double costSpeed,costDist,costHeading,costResult;
  double vtrans,vrot,dist,angle;
  double vTransAllowed,vRotAllowed;

  double vRotStepHelp,vTransStepHelp;
  double posXm,posYm;
  double posXstop,posYstop,posAstop,deltaX,deltaY;

  double vAccMax, wAccMax;
  double brakeDist, brakeAngle;
  double radius;
  double passingDistance,hitDistance,hitAngle,goalDistance;
  double x1i,y1i,x2i,y2i;
  int    nrIntersections;

  double alpha;

  int i,j;
  int status;

#if DEBUG_INTERACTIVE
  cout << "CDL debug interactive\n\n";
  cout << "current v, w [mm/s, deg/s]\n";
  cin >> v;
  cin >> w;
  w = rad00(w);
  cout << "current pos x, y, a [mm,mm,deg]\n";
  cin >> posX;
  cin >> posY;
  cin >> posA;
  posA = rad00(posA);
  cout << "we set the following parameters:\n\n";
  cout << "vminIni vmaxIni " << vminIni << " " << vmaxIni << "\n";
  cout << "wminIni wmaxIni " << deg00(wminIni) << " " << deg00(wmaxIni) << "\n";
  cout << "strategy " << strategy << "\n\n";
#endif

#if DEBUG_VW_WINDOW
  FILE *file;
  FILE *eval;
  char filenameVW[100];
  char filenameScan[100];
  char filenameEval[100];
  static int fileNumber=0;

  sprintf(filenameVW,"vwwindow%d.dbg",fileNumber);
  sprintf(filenameScan,"vwscan%d.dbg",fileNumber);
  sprintf(filenameEval,"vweval%d.dbg",fileNumber);
  fileNumber++;
#endif

  // preparations for the following computations
  cdl_v_rot_min  = CDL_V_ROT_MIN*M_PI/180.0;
  cdl_v_rot_max  = CDL_V_ROT_MAX*M_PI/180.0;
  cdl_v_rot_step = CDL_V_ROT_STEP*M_PI/180.0;

//
//
//
//
//
// Initialisierung der maxdistance gemaess dem radius vornehmen !!!!
// diesen Wert im Prinzip aus distlookup entnehmen !!!!
//
//
//
//
//
  for (i=0;i<CDL_CURVATURE_INDICES;i++)
    {
      indexFlag[i]    = 0;
      distanceList[i] = (int)CDL_MAX_DISTANCE;
      alphaList[i]    = (int)(2*M_PI*1000.0);
    }

  for (i=0;i<CDL_MAX_TRA_CELLS;i++)
    {
      for (j=0;j<CDL_MAX_ROT_CELLS;j++)
        {
          evaluation[i][j].costHit      = 0.0;
          evaluation[i][j].costPassing  = 0.0;
          evaluation[i][j].costSpeed    = 0.0;
          evaluation[i][j].costDistance = 0.0;
          evaluation[i][j].costHeading  = 0.0;
          evaluation[i][j].costValue    = 0.0;
        }
    }

  // calculate current velocity window
  vmin = v - deltat * vacc;
  vmax = v + deltat * vacc;
  wmin = w - deltat * wacc;
  wmax = w + deltat * wacc;

  // consider physical constraints of the robot
  if (vmin < CDL_V_TRA_MIN) vmin=CDL_V_TRA_MIN;
  if (vmax > CDL_V_TRA_MAX) vmax=CDL_V_TRA_MAX;
  if (wmin < cdl_v_rot_min) wmin=cdl_v_rot_min;
  if (wmax > cdl_v_rot_max) wmax=cdl_v_rot_max;

  // consider constraints of allowed velocity window
  if (vmin < vminIni) vmin=vminIni;
  if (vmax > vmaxIni) vmax=vmaxIni;
  if (wmin < wminIni) wmin=wminIni;
  if (wmax > wmaxIni) wmax=wmaxIni;

  ivmin = (int)(floor((vmin-CDL_V_TRA_MIN)/CDL_V_TRA_STEP+0.5));
  ivmax = (int)(floor((vmax-CDL_V_TRA_MIN)/CDL_V_TRA_STEP+0.5));
  iwmin = (int)(floor((wmin-cdl_v_rot_min)/cdl_v_rot_step+0.5));
  iwmax = (int)(floor((wmax-cdl_v_rot_min)/cdl_v_rot_step+0.5));

#if DEBUG_VW_WINDOW
  currentScan.save(filenameScan);
  file=fopen(filenameVW,"w");
  if (file!=NULL)
    {
      fprintf(file,"vmin vmax wmin wmax %f %f %f %f\n",
              vmin,vmax,deg00(wmin),deg00(wmax));
      fprintf(file,"ivmin ivmax iwmin iwmax %d %d %d %d\n",
              ivmin,ivmax,iwmin,iwmax);
      fprintf(file,"goalX goalY %f %f\n",goalX,goalY);
    }
#endif

  // extract the relevant curvature indices
  indexCurvature=0;
  for (vi=ivmin;vi<=ivmax;vi++)
    {
      for (wi=iwmin;wi<=iwmax;wi++)
        {
          ci = indexVW[vi][wi];
          if (indexFlag[ci]==0)
            {
              indexFlag[ci]=1;
              indexList[indexCurvature++]=ci;
            }
        }
    }

#if DEBUG_VW_WINDOW
  if (file!=NULL)
    {
      for (vi=ivmin;vi<=ivmax;vi++)
        {
          for (wi=iwmin;wi<=iwmax;wi++)
            {
              fprintf(file,"vi wi v w ci %d %d %f %f %d\n",
                      vi,
                      wi,
                      CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP,
                      deg00(cdl_v_rot_min+wi*cdl_v_rot_step),
                      indexVW[vi][wi]);
            }
        }
    }
#endif

  //<asteck: date="17.07.2008" variables added>
  double x, y;
  //</asteck>

  // now calculate the min distances considering the laser scan
  for (scanindex=0;scanindex<scan.get_scan_size();scanindex++)
    {

      //<asteck: date="17.07.2008" some modifications due to other Laserscan-Object>
      scan.get_scan_cartesian_point_scanner( scanindex, x, y );
      if (x < 0.0)
        {
          ix = (int)(ceil((x-CDL_C_X_MIN)/CDL_C_RES));
        }
      else
        {
          ix = (int)(floor((x-CDL_C_X_MIN)/CDL_C_RES));
        }
      if (y < 0.0)
        {
          iy = (int)(ceil((y-CDL_C_Y_MIN)/CDL_C_RES));
        }
      else
        {
          iy = (int)(floor((y-CDL_C_Y_MIN)/CDL_C_RES));
        }
      //</asteck>
#if DEBUG_VW_WINDOW
      if (file!=NULL)
        {
          fprintf(file,"scanindex %d ix iy %d %d scan.x scan.y %f %f\n",
                  scanindex,ix,iy,x,y);
        }
#endif

      if ((ix>=0) && (ix<CDL_C_X_CELLS) && (iy>=0) && (iy<CDL_C_Y_CELLS))
        {
          for (li=0;li<indexCurvature;li++)
            {
              ci=indexList[li];
              if (distLookup[ix][iy][ci]<distanceList[ci])
                {
                  distanceList[ci] = distLookup[ix][iy][ci];
                }
              if (alphaLookup[ix][iy][ci]<alphaList[ci])
                {
                  alphaList[ci]    = alphaLookup[ix][iy][ci];
                }
            }
        }
    }

#if DEBUG_VW_WINDOW
  if (file!=NULL)
    {
      for (li=0;li<indexCurvature;li++)
        {
          fprintf(file,"index distance angle %d %d %f\n",
                  indexList[li],
                  distanceList[indexList[li]],
                  deg00((double)alphaList[indexList[li]]/1000));
        }
    }
#endif

  // ------------------------------------------------------
  // now we have to calculate the evaluation function
  // ------------------------------------------------------
  vResult        = 0.0;
  wResult        = 0.0;
  vAccResult     = 0.0;
  wAccResult     = 0.0;
  costResult     = 0.0;
  vRotStepHelp   = cdl_v_rot_step/2.0;
  vTransStepHelp = CDL_V_TRA_STEP/2.0;



//
//
// BEGIN TEST AREA CDL_EVAL_APPROACH_EXACT
//
//

  for (i=0;i<CDL_MAX_CURVATURE;i++)
    {
      indexWanted[i] = 1;
    }
  for (vi=ivmin;vi<=ivmax;vi++)
    {
      for (wi=iwmin;wi<=iwmax;wi++)
        {
          ci   = indexVW[vi][wi];
          dist = (double)(distanceList[ci]);
          if (dist < 500.0)
            {
              for (i=-40;i<=40;i++)
                {
                  j=ci+i;
                  if (j>CDL_MAX_CURVATURE) j-=CDL_MAX_CURVATURE;
                  if (j<0) j+=CDL_MAX_CURVATURE;
                  indexWanted[j]=0;
                }
            }
        }
    }

#if DEBUG_VW_WINDOW
  if (file!=NULL)
    {
      for (i=0;i<CDL_MAX_CURVATURE;i++)
        {
          fprintf(file,"curvature %d: flag %d wanted %d\n",i,indexFlag[i],indexWanted[i]);
        }
    }
#endif

//
//
// END TEST AREA CDL_EVAL_APPROACH_EXACT
//
//


  for (vi=ivmin;vi<=ivmax;vi++)
    {
      for (wi=iwmin;wi<=iwmax;wi++)
        {
          vtrans  = CDL_V_TRA_MIN + vi*CDL_V_TRA_STEP;
          vrot    = cdl_v_rot_min + wi*cdl_v_rot_step;
          ci      = indexVW[vi][wi];
          dist    = (double)(distanceList[ci]);
          angle   = (double)alphaList[ci]/1000.0;

          vAccMax = transAccLookup[ci];
          wAccMax = rotAccLookup[ci];

          // the following is necessary to prevent rounding errors
          if (abs00(vtrans) < vTransStepHelp) vtrans = 0.0;
          if (abs00(vrot)   < vRotStepHelp)   vrot   = 0.0;

          // We consider the distance and angle changes within the
          // current interval used for calculation of the next
          // motion command. To simplify the calculation and to be
          // on the safe side, we always substract that from the
          // remaining distance and angle.

          dist   = dist -abs00(v)*deltat;
          if (dist  < 0.0) dist =0.0;
          angle  = angle-abs00(w)*deltat;
          if (angle < 0.0) angle=0.0;

          dist  -= 200.0;
          angle -= rad00(5.0);
          if (dist  < 0.0) dist  = 0.0;
          if (angle < 0.0) angle = 0.0;


          // first we have to check whether the robot can stop on the
          // remaining distance and angle with the current speed

          vTransAllowed = sqrt(2.0 * dist  * vAccMax);
          vRotAllowed   = sqrt(2.0 * angle * wAccMax);

#if DEBUG_VW_WINDOW
          if (file!=NULL)
            {
              fprintf(file,"=====\n");
              fprintf(file,"vtrans vrot             %f %f\n",vtrans,deg00(vrot));
              fprintf(file,"ci corrected dist angle %d %f %f\n",ci,dist,deg00(angle));
              fprintf(file,"vAccMax wAccMax         %f %f\n",vAccMax,deg00(wAccMax));
              fprintf(file,"vtrans vTransAllowed    %f %f\n",vtrans,vTransAllowed);
              fprintf(file,"vrot   vRotAllowed      %f %f\n",deg00(vrot),
                      deg00(vRotAllowed));
            }
#endif

          if ((abs00(vtrans) <= vTransAllowed) && (abs00(vrot) <= vRotAllowed))
            {
              // these velocities are allowed

// factors
#define BRAKE_SECURITY_FACTOR  1.0
              // ----------------------------------------------------
              // now switch depending on the selected strategy
              // ----------------------------------------------------
              switch (strategy)
                {
                case CDL_STRATEGY_10:
                  // ------------------------------------------------
                  // - assumes absolute current position
                  // - assumes absolute goal position
                  //
                  // - calculate position when assuming maximum
                  //   allowed decceleration on selected curvature
                  //   for determining all the relevant cost values
                  // ------------------------------------------------

                  //
                  // calculate stop position
                  //
                  if (abs00(vrot) <= vRotStepHelp)
                    {
                      //
                      //
                      // drive straight forward
                      //
                      //
                      if (abs00(vtrans) <= vTransStepHelp)
                        {
                          // no movement at all, but set brakeDist to an
                          // uncritical value to get the heading direction
                          // for determining the costGoal value
                          brakeDist  = 1.0;
                          brakeAngle = 0.0;
                        }
                      else
                        {
                          // robot moves straight forward
                          brakeDist  = BRAKE_SECURITY_FACTOR*(vtrans*vtrans)/(2*vAccMax);
                          brakeAngle = 0.0;
                        }
                      posXstop     = posX+brakeDist*cos(posA);
                      posYstop     = posY+brakeDist*sin(posA);
                      posAstop     = angle00(posA);

                      // intersection of circle around goal position
                      // and line through current position and goal position
                      status = intersectionsCircleLine(
                                 goalX,goalY,CDL_CAPTURE_DISTANCE,
                                 posX,posY,posXstop,posYstop,
                                 x1i,y1i,x2i,y2i,
                                 nrIntersections);
                      if ((status == 0) && (nrIntersections > 0))
                        {
                          // now check whether the intersections are in
                          // driving direction
                          double alpha1,alpha2,alpha;

                          alpha1 = atan2(posYstop-posY,posXstop-posX);
                          alpha2 = atan2(y1i-posY,x1i-posX);
                          alpha  = angle00(alpha2 - alpha1);

                          if ((alpha > -M_PI) && (alpha < M_PI))
                            {
                              // correct heading to hit the goal point therefore
                              // driving straight forward allows to hit the goal
                              costPassing = 1;
                            };
                        }
                      else
                        {
                          // goal point is in reverse direction to current heading or
                          // is too far away from driving direction
                          costPassing = 0;
                        }

                      // calculate whether goal point hits the goal position
                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

#if DEBUG_VW_WINDOW
                      if (file!=NULL)
                        {
                          fprintf(file,"drive straight forward\n");
                          fprintf(file,"brakeDist brakeAngle %f %f\n",
                                  brakeDist,deg00(brakeAngle));
                          fprintf(file,"stopPosX stopPosY stopPosA %f %f %f\n",
                                  posXstop,posYstop,deg00(posAstop));
                          fprintf(file,"hitDistance hitAngle %f %f\n",
                                  hitDistance,deg00(hitAngle));
                          fprintf(file,"costHit costPassing %f %f\n",
                                  costHit,costPassing);
                        }
#endif

                    }
                  else if (abs00(vtrans) <= vTransStepHelp)
                    {
                      //
                      //
                      // turn in place
                      //
                      //
                      brakeDist       = 0.0;
                      brakeAngle      = BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      if (vrot < 0.0) brakeAngle *= -1.0;
                      posXstop        = posX;
                      posYstop        = posY;
                      posAstop        = angle00(posA+brakeAngle);
                      deltaX          = goalX - posXstop;
                      deltaY          = goalY - posYstop;
                      passingDistance = sqrt(deltaX*deltaX+deltaY*deltaY);

                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // turning in place allows to hit the goal since the
                          // robot is already within the circle around the goal
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      // calculate whether goal point hits the goal position
                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

#if DEBUG_VW_WINDOW
                      if (file!=NULL)
                        {
                          fprintf(file,"turn in place\n");
                          fprintf(file,"brakeDist brakeAngle %f %f\n",
                                  brakeDist,deg00(brakeAngle));
                          fprintf(file,"costHit costPassing %f %f\n",
                                  costHit,costPassing);
                        }
#endif

                    }
                  else if (vrot > 0.0)
                    {
                      //
                      //
                      // drive on curvature, left
                      //
                      //
                      brakeAngle      = BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      radius          = vtrans/vrot;
                      posXm           = posX  - radius * sin(posA);
                      posYm           = posY  + radius * cos(posA);
                      posXstop        = posXm + radius * sin(posA+brakeAngle);
                      posYstop        = posYm - radius * cos(posA+brakeAngle);
                      posAstop        = angle00(posA+brakeAngle);

                      deltaX          = posXm - goalX;
                      deltaY          = posYm - goalY;
                      passingDistance = abs00(sqrt(deltaX*deltaX+deltaY*deltaY)-radius);
                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // the considered path goes through the specified
                          // goal point within a reasonable bound
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

#if DEBUG_VW_WINDOW
                      if (file!=NULL)
                        {
                          fprintf(file,"drive on curvature, left\n");
                          fprintf(file,"brakeAngle %f\n",
                                  deg00(brakeAngle));
                          fprintf(file,"stopPosX stopPosY stopPosA %f %f %f\n",
                                  posXstop,posYstop,deg00(posAstop));
                          fprintf(file,"posXm posYm deltx delty radius %f %f %f %f %f\n",
                                  posXm,posYm,deltaX,deltaY,radius);
                          fprintf(file,"hitDistance hitAngle %f %f\n",
                                  hitDistance,deg00(hitAngle));
                          fprintf(file,"costHit costPassing %f %f\n",
                                  costHit,costPassing);
                        }
#endif
                    }
                  else
                    {
                      //
                      //
                      // drive on curvature, right
                      //
                      //
                      brakeAngle      = -BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      radius          = -vtrans/vrot;   // assume positive radius
                      posXm           = posX  + radius * sin(posA);
                      posYm           = posY  - radius * cos(posA);
                      posXstop        = posXm - radius * sin(posA+brakeAngle);
                      posYstop        = posYm + radius * cos(posA+brakeAngle);
                      posAstop        = angle00(posA+brakeAngle);
                      deltaX          = posXm - goalX;
                      deltaY          = posYm - goalY;
                      passingDistance = abs00(sqrt(deltaX*deltaX+deltaY*deltaY)-radius);

                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // the considered path goes through the specified
                          // goal point within a reasonable bound
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

#if DEBUG_VW_WINDOW
                      if (file!=NULL)
                        {
                          fprintf(file,"drive on curvature, right\n");
                          fprintf(file,"brakeAngle %f\n",
                                  deg00(brakeAngle));
                          fprintf(file,"stopPosX stopPosY stopPosA %f %f %f\n",
                                  posXstop,posYstop,deg00(posAstop));
                          fprintf(file,"posXm posYm deltx delty radius %f %f %f %f %f\n",
                                  posXm,posYm,deltaX,deltaY,radius);
                          fprintf(file,"hitDistance hitAngle %f %f\n",
                                  hitDistance,deg00(hitAngle));
                          fprintf(file,"costHit costPassing %f %f\n",
                                  costHit,costPassing);
                        }
#endif
                    }

                  deltaX = posX - goalX;
                  deltaY = posY - goalY;
                  goalDistance = sqrt(deltaX * deltaX + deltaY * deltaY);

                  //
                  //
                  // evaluation functions
                  //
                  //
                  switch (evalFunc)
                    {
                    case CDL_EVAL_STANDARD:
                      //
                      //
                      //
                      break;
                    case CDL_EVAL_APPROACH_OBJECT:
                      //
                      //
                      //
                      break;
                    case CDL_EVAL_PASSING:
                      //
                      // std::cout << "CDL_EVAL_PASSING: posX = " << posX << "; goalX = " << goalX <<"; posY = "<<posY<< "; goalY = "<< goalY <<  "; goalDistance = " << goalDistance << std::endl;
                      //
                      if (goalDistance > rangeFarPassing)
                        {
                          desiredTranslationalSpeed = speedFarPassing;
                        }
                      else if (goalDistance > rangeMediumPassing)
                        {
                          desiredTranslationalSpeed = speedMediumPassing;
                        }
                      else
                        {
                          desiredTranslationalSpeed = speedNearPassing;
                        }
                      costHeading = 1.0-abs00(hitAngle)/M_PI;

                      costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                      if (costSpeed < 0.0) costSpeed = 0.0;

                      costDist=dist/CDL_MAX_DISTANCE;
                      if (costDist > 1.0) costDist = 1.0;

// if (indexWanted[ci]==0) {
//   // curvature is not wanted
//   costValue = 10000.0;
// } else {
//   // curvature is wanted
//   costValue = 20000.0;
// };
                      costValue = 0.0;
//                costValue += 1.0*costSpeed+1.0*costDist+2000.0*costHeading;
                      costValue += 1.0*costSpeed+1.0*costDist+200.0*costHeading;
                      costValue += 2*costPassing;

                      if (dist > bonusDistPassing)
                        {
                          costValue += 5000.0;
                        }

                      if ((vtrans > 5.0) && (vtrans < 100.0)) costValue = 0.0;
                      if (fabs(vtrans) < 5.0)
                        {
                          if (vrot >  rad00(10)) vrot =  rad00(10);
                          if (vrot < -rad00(10)) vrot = -rad00(10);
                        }

                      alpha  = angle00(posA - atan2(goalY-posY,goalX-posX));
                      if ((fabs(alpha) > rad00(45)) && (vtrans > 400.0)) costValue=0;


                      break;
                    case CDL_EVAL_STOPPING:
                      //
                      //
                      // std::cout << "CDL_EVAL_STOPPING: posX = " << posX << "; goalX = " << goalX <<"; posY = "<<posY<< "; goalY = "<< goalY <<  "; goalDistance = " << goalDistance << std::endl;
                      //
                      if (goalDistance > rangeFarStopping)
                        {
                          desiredTranslationalSpeed = speedFarStopping;
                        }
                      else if (goalDistance > rangeMediumStopping)
                        {
                          desiredTranslationalSpeed = speedMediumStopping;
                        }
                      else
                        {
                          desiredTranslationalSpeed = speedNearStopping;
                        }
                      costHeading = 1.0-abs00(hitAngle)/M_PI;

                      costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                      if (costSpeed < 0.0) costSpeed = 0.0;

                      costDist=dist/CDL_MAX_DISTANCE;
                      if (costDist > 1.0) costDist = 1.0;

// if (indexWanted[ci]==0) {
//   // curvature is not wanted
//   costValue = 10000.0;
// } else {
//   // curvature is wanted
//   costValue = 20000.0;
// };

                      costValue = 0.0;
//                costValue += 1.0*costSpeed+1.0*costDist+2000.0*costHeading;
//                costValue += 1.0*costSpeed+1.0*costDist+200.0*costHeading; //Lutz
                      costValue += 1.0*costSpeed+1.0*costDist+200.0*costHeading;
                      costValue += 2*costHit;

                      if (dist > bonusDistStopping)
                        {
                          costValue += 5000.0;
                        }

                      if ((vtrans > 5.0) && (vtrans < 50.0)) costValue = 0.0;
                      if (fabs(vtrans) < 5.0)
                        {
                          if (vrot >  rad00(10)) vrot =  rad00(10);
                          if (vrot < -rad00(10)) vrot = -rad00(10);
                        }
                      alpha  = angle00(posA - atan2(goalY-posY,goalX-posX));
                      if ((fabs(alpha) > rad00(45)) && (vtrans > 400.0)) costValue=0;

                      break;


                    case CDL_EVAL_APPROACH_EXACT:

                      if (indexWanted[ci]==0)
                        {
                          // curvature is not wanted
                          costValue = 10000.0;
                        }
                      else
                        {
                          // curvature is wanted
                          costValue = 20000.0;
                        };

                      costHeading = 1.0-abs00(hitAngle)/M_PI;

                      costDist=dist/CDL_MAX_DISTANCE;
                      if (costDist > 1.0) costDist = 1.0;

// if (goalDistance > 2500.0) {
//   desiredTranslationalSpeed = 100.0;
// } else if (goalDistance > 1000.0) {
//   desiredTranslationalSpeed = 100.0;
// } else {
//   desiredTranslationalSpeed = 50.0;
// }
// costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
// if (costSpeed < 0.0) costSpeed = 0.0;
// costValue += 2000.0*costHeading+1.0*costSpeed+1.0*costDist;

                      if (abs00(vrot) > rad00(30.0)) costValue = 0.0;
                      if (goalDistance > 1000.0)
                        {
                          if ((vtrans<95.0) || (vtrans>105.0)) costValue = 0.0;
                        }
                      else
                        {
                          if ((vtrans <45.0) || (vtrans>55.0)) costValue = 0.0;
                        }
                      costValue += 2000.0*costHeading;

                      break;
                    default:
                      costValue = 0.0;
                      break;
                    }


#if EVAL_FUNC_1
                  {
                    double free = 1000.0;
                    double angle = (60.0*M_PI/180.0);

                    if (hitAngle < -angle) hitAngle=-angle;
                    if (hitAngle >  angle) hitAngle= angle;
                    costHeading=1.0-abs00(angle00(hitAngle-vrot))/(2*angle);

                    if (hitDistance > free)
                      {
                        desiredTranslationalSpeed = 900.0;
                      }
                    else
                      {
                        desiredTranslationalSpeed = 200.0;
                      }
                    costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/vmaxIni;
                    costDist=dist/5000.0;
                    if (costDist>1.0) costDist=1.0;

                    if ((dist > free) && (vtrans>100.0))
                      {
                        alpha1=1.0;
                        alpha2=1.0;
                        alpha3=2.0;
                        costValue=5+alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                      }
                    else
                      {
                        alpha1=1.0;
                        alpha2=1.0;
                        alpha3=1.0;
                        costValue=alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                      }
                  }
#endif

                  //
                  //
                  // evaluation function 3
                  //
                  //
#if EVAL_FUNC_3
                  costValue = 0;
                  if (goalDistance < 500.0)
                    {
                      desiredTranslationalSpeed = 50.0;
                    }
                  else if (goalDistance < 1000.0)
                    {
                      desiredTranslationalSpeed = 100.0;
//              costValue += costHit*10.0;
                    }
                  else
                    {
                      desiredTranslationalSpeed = 900.0;
//              if ((dist > 1000.0) || (angle > rad00(20.0))) costValue += 6;
//              if (abs00(deg00(hitAngle)) < 10.0) costValue+=3;
                      costValue += costPassing;
                    }

                  costHeading = 1.0-abs00(hitAngle)/M_PI;
                  costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                  if (costSpeed < 0.0) costSpeed = 0.0;
                  costDist=dist/CDL_MAX_DISTANCE;
                  if (costDist > 1.0) costDist = 1.0;

                  if ((dist > 1000.0) && (vtrans > 100.0))
                    {
                      alpha1 = 1.0;
                      alpha2 = 1.0;
                      alpha3 = 2.0;
                      costValue=5+alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                    }
                  else
                    {
                      alpha1 = 1.0;
                      alpha2 = 1.0;
                      alpha3 = 2.0;
                      costValue=alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                    }

                  evaluation[vi][wi].costHit      = costHit;
                  evaluation[vi][wi].costPassing  = costPassing;
                  evaluation[vi][wi].costSpeed    = costSpeed;
                  evaluation[vi][wi].costDistance = costDist;
                  evaluation[vi][wi].costHeading  = costHeading;
                  evaluation[vi][wi].costValue    = costValue;

#endif

                  //
                  //
                  // evaluation function 4
                  //
                  //
#if EVAL_FUNC_4
                  if (hitDistance > 2000.0)
                    {
                      desiredTranslationalSpeed = 500.0;
                    }
                  else
                    {
                      desiredTranslationalSpeed = 100.0;
                    }
                  if (dist > 1500.0)
                    {
                      costHeading = 1.0-abs00(hitAngle)/M_PI;
                      costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                      if (costSpeed < 0.0) costSpeed = 0.0;
                      costDist=dist/CDL_MAX_DISTANCE;
                      if (costDist > 1.0) costDist = 1.0;
                      costValue = 1.0*costSpeed+1.0*costDist+1000.0*costHeading;
                    }
                  else
                    {
                      costValue = 0.0;
                    }
#endif

#if DEBUG_VW_WINDOW
                  if (file!=NULL)
                    {
                      fprintf(file,"evaluation\n");
                      fprintf(file,"  hitAngle     %f\n",deg00(hitAngle));
                      fprintf(file,"  hitDistance  %f\n",hitDistance);
                      fprintf(file,"  costHeading  %f\n",costHeading);
                      fprintf(file,"  costSpeed    %f\n",costSpeed);
                      fprintf(file,"  costPassing  %f\n",costPassing);
                      fprintf(file,"  costValue    %f\n",costValue);
                    }
#endif

                  break;


                case CDL_STRATEGY_11:
                  //
                  // this strategy is used to turn in place
                  //
                  // the desired heading is specified by the current
                  // robot position and the goal position
                  //
                  // important hint:
                  // to prevent overshooting the goal orientation, hitAngles
                  // stopping before the goal orientation are better than
                  // hitAngles stopping after the desired orientation. Therefore,
                  // the evaluation function considers the turning direction
                  //
                {
                  double desiredHeading;   //     [-M_PI, M_PI]

                  desiredHeading = angle00(atan2(goalY-posY,goalX-posX));
                  if (abs00(vrot) <= vRotStepHelp)
                    {
                      //
                      // no movement at all
                      //
                      brakeAngle = 0.0;
                      posAstop   = posA;
                    }
                  else
                    {
                      //
                      // robot turns in place
                      //
                      brakeAngle = (vrot*vrot)/(2*wAccMax);
                      if (vrot < 0.0) brakeAngle *= -1.0;
                      // factor 2.0 is used to damp the rotational velocity
                      posAstop   = angle00(posA+2.0*brakeAngle);
                    }
                  hitAngle = angle00(desiredHeading-posAstop);

                  if ((vrot > 0.0) && (hitAngle >= 0.0))
                    {
                      // turning left
                      costHeading = 1.0-abs00(hitAngle)/M_PI;
                    }
                  else if ((vrot < 0.0) && (hitAngle <= 0.0))
                    {
                      // turning right
                      costHeading = 1.0-abs00(hitAngle)/M_PI;
                    }
                  else
                    {
                      // overshooting
                      costHeading = 0.0;
                    }

                  costValue = costHeading;

                  evaluation[vi][wi].costHit      = 0.0;
                  evaluation[vi][wi].costPassing  = 0.0;
                  evaluation[vi][wi].costSpeed    = 0.0;
                  evaluation[vi][wi].costDistance = 0.0;
                  evaluation[vi][wi].costHeading  = costHeading;
                  evaluation[vi][wi].costValue    = costValue;

#if DEBUG_VW_WINDOW
                  if (file!=NULL)
                    {
                      fprintf(file,"rotate in place\n");
                      fprintf(file,"pos  x y a %f %f %f\n",posX,posY,deg00(posA));
                      fprintf(file,"goal x y   %f %f\n",goalX,goalY);
                      fprintf(file,"desiredHeading %f\n",deg00(desiredHeading));
                      fprintf(file,"brakeAngle  %f\n",deg00(brakeAngle));
                      fprintf(file,"posAstop    %f\n",deg00(posAstop));
                      fprintf(file,"hitAngle    %f\n",deg00(hitAngle));
                      fprintf(file,"costHeading %f\n",costHeading);
                    }
#endif
                }
                break;




                case CDL_STRATEGY_1:
                  // ------------------------------------------------
                  //  - minimize difference to desiredTranslationalSpeed
                  //  - maximize remaining travel distance
                  //  - prefer curvatures leading to the desired heading
                  // ------------------------------------------------

// Achtung: costSpeed wird komplizierter, wenn tatsaechlich auch
// negative Geschwindigkeit zugelassen ist und MAX und MIN
// verschieden sind. Irgendwie muss die Differenz auf das
// Intervall [0,1] abgebildet werden

                  costSpeed   = 1.0-abs00(desiredTranslationalSpeed-vtrans)/vmaxIni;
// immer in Relation zur derzeit ueberhaupt maximal erlaubten
// Translationsgeschwindigkeit sehen !!

                  costDist    = dist/maxDistance;
                  if (costDist > 1.0) costDist=1.0;
                  costHeading = 1.0-abs00(angle00(goalHeadingRelative-vrot*deltat))/M_PI;
                  costValue   = alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;

                  break;

                case CDL_STRATEGY_2:
                  // ----------------------------------------------------
                  // - akzeptiert Geschwindigkeitsvorgabe von aussen
                  // ----------------------------------------------------
                {
                  double free = 1000.0;
                  double angle = (60.0*M_PI/180.0);

                  if (goalHeadingRelative < -angle) goalHeadingRelative=-angle;
                  if (goalHeadingRelative >  angle) goalHeadingRelative= angle;
                  costHeading=1.0-abs00(angle00(goalHeadingRelative-vrot))/(2*angle);

                  costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/vmaxIni;
                  costDist=dist/maxDistance;
                  if (costDist>1.0) costDist=1.0;
                  if ((dist > free) && (vtrans>100.0))
                    {
                      alpha1=1.0;
                      alpha2=1.0;
                      alpha3=2.0;
                      costValue=5+alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                    }
                  else
                    {
                      alpha1=1.0;
                      alpha2=1.0;
                      alpha3=1.0;
                      costValue=alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                    }

                }
                break;

                case CDL_STRATEGY_3:
                  // ----------------------------------------------------
                  // - macht Geschwindigkeitsregelung selbstaendig
                  // - muss man noch fertig implementieren
                  // ----------------------------------------------------
                {
                  double angle = (40.0*M_PI/180.0);

                  if (goalHeadingRelative < -angle) goalHeadingRelative=-angle;
                  if (goalHeadingRelative >  angle) goalHeadingRelative= angle;

                  if ((dist < 1000.0) || (abs00(vrot) > angle) || (abs00(vtrans)<50.0))
                    {
                      // mindestens Restwegstrecke von 1000 mm
                      // mindestens Translationsgeschwindigkeit 100 mm/s
                      // maximal Drehung um 40 Grad/s
                      costValue = 0.0;
                    }
                  else
                    {
                      costHeading=1.0-abs00(angle00(goalHeadingRelative-vrot))/(2*angle);

                      costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/vmaxIni;

                      costDist=dist/maxDistance;
                      if (costDist>1.0) costDist=1.0;
                      costValue=alpha1*costSpeed+alpha2*costDist+alpha3*costHeading;
                    }
                }
                break;
                case CDL_STRATEGY_4:
                  // -------------------------------------------------------
                  // used for testing purposes
                  // - je groesser die Abweichung von der Zielrichtung, desto hoeher w
                  // - je groesser die Entfernung zu Ziel, desto hoeher v
                  // -------------------------------------------------------
// vtrans, vrot         aktuell betrachtete Geschwindigkeit
// dist                 verbleibende Reststrecke
// goalHeadingRelative  Ausrichtungsfehler bezueglich Ziel
//
                {
                  double angle = (90*M_PI/180.0);
                  double mind  = 1000.0;
                  double minv  = 50.0;
                  double k     = 2.0;               // Linearitaetsfaktor
                  double wmax  = (50.0*M_PI/180.0);   // maximal erlaubte Rotationsgeschwindigkeit
                  double amax  = 30.0;              // zur Berechnung der maximal erlaubten Geschwindigkeit

                  double wsoll;
                  double vsoll;

                  // Begrenzung des Winkelfehlers
                  if (goalHeadingRelative < -angle) goalHeadingRelative=-angle;
                  if (goalHeadingRelative >  angle) goalHeadingRelative= angle;

                  // Bestimmung der Solldrehgeschwindigkeit, begrenzt durch wmax
//            wsoll = sqrt(2*goalHeadingRelative*wmax);
                  wsoll = goalHeadingRelative*k;
                  if (wsoll > wmax) wsoll =  wmax;
                  if (wsoll <-wmax) wsoll = -wmax;

                  // Bestimmung der Sollfahrgeschwindigkeit
                  if (maxDistance < 200.0) vsoll=0.0;
                  else vsoll=sqrt(2*maxDistance*amax);

                  if ((dist < mind) || (abs00(vrot) > wsoll) || (abs00(vtrans) < minv))
                    {
                      costValue = 0.0;
                    }
                  else
                    {
                      costHeading = 1.0 - min00(abs00(wsoll-vrot)/(2*wmax),1.0);
                      costSpeed   = 1.0 - min00(abs00(vsoll-vtrans)/vmaxIni,1.0);
                      costDist    = min00(dist/maxDistance,1.0);
// vtrans=0.0;
                      costValue = 1.0 * costSpeed + 1.0 * costDist + 1.0 * costHeading;
                    }
                  break;
                }


//<asteck>
                case CDL_STRATEGY_5:
                {
                  // ------------------------------------------------------------------------
                  // used for v-w commands from "outside" (for example from joystick)
                  // commands should be set with setDesiredTranslationSpeed and setHeading
                  // ------------------------------------------------------------------------
                  double free   = 1000.0;
                  double angle  = (60.0*M_PI/180.0);

                  if (goalHeadingRelative < -angle) goalHeadingRelative=-angle;
                  if (goalHeadingRelative >  angle) goalHeadingRelative= angle;

                  costHeading = 1.0 - abs00( angle00(goalHeadingRelative-vrot)) / (2*angle);
                  costSpeed   = 1.0 - abs00( desiredTranslationalSpeed-vtrans) / vmaxIni;

                  costDist    = 1; //dist/maxDistance;

                  alpha1 = 1.0;
                  alpha2 = 1.0;
                  alpha3 = 3.0;

                  //costValue =  alpha1*costSpeed + alpha3*costHeading;
                  costValue = alpha1*costSpeed + alpha3*costHeading;

                  break;
                } // CDL_STRATEGY_5

//</asteck>

                case CDL_STRATEGY_6:
                {
                  // ------------------------------------------------
                  // - assumes absolute current position
                  // - assumes absolute goal position
                  //
                  // - calculate position when assuming maximum
                  //   allowed decceleration on selected curvature
                  //   for determining all the relevant cost values
                  // ------------------------------------------------

                  //
                  // calculate stop position
                  //
                  if (abs00(vrot) <= vRotStepHelp)
                    {
                      //
                      //
                      // drive straight forward
                      //
                      //
                      if (abs00(vtrans) <= vTransStepHelp)
                        {
                          // no movement at all, but set brakeDist to an
                          // uncritical value to get the heading direction
                          // for determining the costGoal value
                          brakeDist  = 1.0;
                          brakeAngle = 0.0;
                        }
                      else
                        {
                          // robot moves straight forward
                          brakeDist  = BRAKE_SECURITY_FACTOR*(vtrans*vtrans)/(2*vAccMax);
                          brakeAngle = 0.0;
                        }
                      posXstop     = posX+brakeDist*cos(posA);
                      posYstop     = posY+brakeDist*sin(posA);
                      posAstop     = angle00(posA);

                      // intersection of circle around goal position
                      // and line through current position and goal position
                      status = intersectionsCircleLine(
                                 goalX,goalY,CDL_CAPTURE_DISTANCE,
                                 posX,posY,posXstop,posYstop,
                                 x1i,y1i,x2i,y2i,
                                 nrIntersections);
                      if ((status == 0) && (nrIntersections > 0))
                        {
                          // now check whether the intersections are in
                          // driving direction
                          double alpha1,alpha2,alpha;

                          alpha1 = atan2(posYstop-posY,posXstop-posX);
                          alpha2 = atan2(y1i-posY,x1i-posX);
                          alpha  = angle00(alpha2 - alpha1);

                          if ((alpha > -M_PI) && (alpha < M_PI))
                            {
                              // correct heading to hit the goal point therefore
                              // driving straight forward allows to hit the goal
                              costPassing = 1;
                            };
                        }
                      else
                        {
                          // goal point is in reverse direction to current heading or
                          // is too far away from driving direction
                          costPassing = 0;
                        }

                      // calculate whether goal point hits the goal position
                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }


                    }
                  else if (abs00(vtrans) <= vTransStepHelp)
                    {
                      //
                      //
                      // turn in place
                      //
                      //
                      brakeDist       = 0.0;
                      brakeAngle      = BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      if (vrot < 0.0) brakeAngle *= -1.0;
                      posXstop        = posX;
                      posYstop        = posY;
                      posAstop        = angle00(posA+brakeAngle);
                      deltaX          = goalX - posXstop;
                      deltaY          = goalY - posYstop;
                      passingDistance = sqrt(deltaX*deltaX+deltaY*deltaY);

                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // turning in place allows to hit the goal since the
                          // robot is already within the circle around the goal
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      // calculate whether goal point hits the goal position
                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

                    }
                  else if (vrot > 0.0)
                    {
                      //
                      //
                      // drive on curvature, left
                      //
                      //
                      brakeAngle      = BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      radius          = vtrans/vrot;
                      posXm           = posX  - radius * sin(posA);
                      posYm           = posY  + radius * cos(posA);
                      posXstop        = posXm + radius * sin(posA+brakeAngle);
                      posYstop        = posYm - radius * cos(posA+brakeAngle);
                      posAstop        = angle00(posA+brakeAngle);

                      deltaX          = posXm - goalX;
                      deltaY          = posYm - goalY;
                      passingDistance = abs00(sqrt(deltaX*deltaX+deltaY*deltaY)-radius);
                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // the considered path goes through the specified
                          // goal point within a reasonable bound
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }

                    }
                  else
                    {
                      //
                      //
                      // drive on curvature, right
                      //
                      //
                      brakeAngle      = -BRAKE_SECURITY_FACTOR*(vrot*vrot)/(2*wAccMax);
                      radius          = -vtrans/vrot;   // assume positive radius
                      posXm           = posX  + radius * sin(posA);
                      posYm           = posY  - radius * cos(posA);
                      posXstop        = posXm - radius * sin(posA+brakeAngle);
                      posYstop        = posYm + radius * cos(posA+brakeAngle);
                      posAstop        = angle00(posA+brakeAngle);
                      deltaX          = posXm - goalX;
                      deltaY          = posYm - goalY;
                      passingDistance = abs00(sqrt(deltaX*deltaX+deltaY*deltaY)-radius);

                      if (passingDistance<CDL_CAPTURE_DISTANCE)
                        {
                          // the considered path goes through the specified
                          // goal point within a reasonable bound
                          costPassing = 1;
                        }
                      else
                        {
                          costPassing = 0;
                        }

                      deltaX      = goalX - posXstop;
                      deltaY      = goalY - posYstop;
                      hitDistance = sqrt(deltaX*deltaX + deltaY*deltaY);
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          costHit = 1;
                        }
                      else
                        {
                          costHit = 0;
                        }

                      // calculate the alignment of the stop position with
                      // the goal point
                      if (hitDistance < CDL_CAPTURE_DISTANCE)
                        {
                          // distance is too small, therefore heading cannot be
                          // calculated correctly. Since heading in goal point
                          // is not relevant in this mode, set goalAngle to 0.
                          hitAngle = 0.0;
                        }
                      else
                        {
                          hitAngle = angle00(atan2(deltaY,deltaX)-posAstop);
                        }
                    }

                  deltaX = posX - goalX;
                  deltaY = posY - goalY;
                  goalDistance = sqrt(deltaX * deltaX + deltaY * deltaY);

                  //
                  //
                  // evaluation functions
                  //
                  //
                  switch (evalFunc)
                    {
                    case CDL_EVAL_STANDARD:
                      //
                      //
                      //
                      break;
                    case CDL_EVAL_APPROACH_OBJECT:
                      //
                      //
                      //
                      break;
                    case CDL_EVAL_PASSING:
                      //
                      // std::cout << "CDL_EVAL_PASSING: posX = " << posX << "; goalX = " << goalX <<"; posY = "<<posY<< "; goalY = "<< goalY <<  "; goalDistance = " << goalDistance << std::endl;
                      //
                      if (goalDistance > rangeFarPassing)
                        {
                          desiredTranslationalSpeed = speedFarPassing;
                        }
                      else if (goalDistance > rangeMediumPassing)
                        {
                          desiredTranslationalSpeed = speedMediumPassing;
                        }
                      else
                        {
                          desiredTranslationalSpeed = speedNearPassing;
                        }
                      costHeading = 1.0-abs00(hitAngle)/M_PI;

                      costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                      if (costSpeed < 0.0) costSpeed = 0.0;

                      costDist=dist/CDL_MAX_DISTANCE;
                      if (costDist > 1.0) costDist = 1.0;

                      costValue = 0.0;
//                costValue += 1.0*costSpeed+1.0*costDist+2000.0*costHeading;
                      costValue += 1.0*costSpeed+1.0*costDist+200.0*costHeading;
                      costValue += 2*costPassing;

                      if (dist > bonusDistPassing)
                        {
                          costValue += 5000.0;
                        }

                      if ((vtrans > 5.0) && (vtrans < 100.0)) costValue = 0.0;
                      if (fabs(vtrans) < 5.0)
                        {
                          if (vrot >  rad00(10)) vrot =  rad00(10);
                          if (vrot < -rad00(10)) vrot = -rad00(10);
                        }

                      alpha  = angle00(posA - atan2(goalY-posY,goalX-posX));
                      if ((fabs(alpha) > rad00(45)) && (vtrans > 400.0)) costValue=0;


                      break;
                    case CDL_EVAL_STOPPING:
                      if (goalDistance > rangeFarStopping)
                        {
                          desiredTranslationalSpeed = speedFarStopping;

                          costHeading = 1.0-abs00(hitAngle)/M_PI;
                          costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                          if (costSpeed < 0.0) costSpeed = 0.0;

                          costDist=dist/CDL_MAX_DISTANCE;
                          if (costDist > 1.0) costDist = 1.0;

                          costValue = 0.0;
                          costValue += 1.0*costSpeed+1.0*costDist+10.0*costHeading;
                          costValue += 2*costHit;
 
                        }
                      else if (goalDistance > rangeMediumStopping)
                        {
                          desiredTranslationalSpeed = speedMediumStopping;

                          costHeading = 1.0-abs00(hitAngle)/M_PI;
                          costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                          if (costSpeed < 0.0) costSpeed = 0.0;

                          costDist=dist/CDL_MAX_DISTANCE;
                          if (costDist > 1.0) costDist = 1.0;

                          costValue = 0.0;
                          costValue += 1.0*costSpeed+1.0*costDist+50.0*costHeading;
                          costValue += 2*costHit;

                        }
                      else
                        {
                          desiredTranslationalSpeed = speedNearStopping;

                          costHeading = 1.0-abs00(hitAngle)/M_PI;
                          costSpeed=1.0-abs00(desiredTranslationalSpeed-vtrans)/1000.0;
                          if (costSpeed < 0.0) costSpeed = 0.0;

                          costDist=dist/CDL_MAX_DISTANCE;
                          if (costDist > 1.0) costDist = 1.0;

                          costValue = 0.0;
                          costValue += 1.0*costSpeed+1.0*costDist+200.0*costHeading;
                          costValue += 2*costHit;

                        }





                      if (dist > bonusDistStopping)
                        {
                          costValue += 5000.0;
                        }

                      if ((vtrans > 5.0) && (vtrans < 50.0)) costValue = 0.0;
                      if (fabs(vtrans) < 5.0)
                        {
                          if (vrot >  rad00(10)) vrot =  rad00(10);
                          if (vrot < -rad00(10)) vrot = -rad00(10);
                        }
                      alpha  = angle00(posA - atan2(goalY-posY,goalX-posX));
                      if ((fabs(alpha) > rad00(45)) && (vtrans > 400.0)) costValue=0;

                      break;

                    default:
                      costValue = 0.0;
                      break;
                    }

                 
                  break;
                 }
                default:
                  // ----------------------------------------------------
                  //
                  // ----------------------------------------------------
                  std::cout<<"Debug CDL_Lookup in default!!\n";
                  costValue = 0.0;
                  break;
                } // end switch()

              if (costValue > costResult)
                {
                  vResult    = vtrans;
                  wResult    = vrot;
                  vAccResult = vAccMax;
                  wAccResult = wAccMax;
                  costResult = costValue;
                }
            }
          else
            {
              // these velocities are not allowed
#if DEBUG_VW_WINDOW
              if (file!=NULL)
                {
                  fprintf(file,"=> these velocities are not allowed\n");
                }
#endif
            }
        }
    } // end for (vi=ivmin;vi<=ivmax;vi++)
#if DEBUG_VW_WINDOW
  if (file!=NULL)
    {
      fprintf(file,"Selected:\n");
      fprintf(file,"  vResult    %f\n",vResult);
      fprintf(file,"  wResult    %f\n",deg00(wResult));
      fprintf(file,"  vAccResult %f\n",vAccResult);
      fprintf(file,"  wAccResult %f\n",deg00(wAccResult));
      fprintf(file,"  cost       %f\n",costResult);
      fclose(file);
    }

  eval = fopen(filenameEval,"w");
  if (eval != NULL)
    {
      fprintf(eval,"costHit\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costHit);
            }
          fprintf(eval,"\n");
        }
      fprintf(eval,"costPassing\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costPassing);
            }
          fprintf(eval,"\n");
        }
      fprintf(eval,"costSpeed\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costSpeed);
            }
          fprintf(eval,"\n");
        }
      fprintf(eval,"costDistance\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costDistance);
            }
          fprintf(eval,"\n");
        }
      fprintf(eval,"costHeading\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costHeading);
            }
          fprintf(eval,"\n");
        }
      fprintf(eval,"costValue\n");
      for (i=ivmin;i<=ivmax;i++)
        {
          for (j=iwmin;j<=iwmax;j++)
            {
              fprintf(eval,"%f ",evaluation[i][j].costValue);
            }
          fprintf(eval,"\n");
        }
      fclose(eval);
    }
#endif

#if DEBUG_INTERACTIVE
  cout << "CDL debug interactive result\n\n";
  cout << "  v w       " << vResult << " " << deg00(wResult) << "\n";
  cout << "  vAcc wAcc " << vAccResult << " " << deg00(wAccResult) << "\n\n";
#endif

#if DEBUG_ONLY_ONE_CYCLE
  cout << "CDL: Debug only one cycle\n";
  cout << "-1- next cycle\n";
  cout << "-2- exit\n";
  cin >> i;
  if (i==2) exit(0);
#endif

  return 0;
};

//int CdlLookupClass::setLaserscan(SmartLaserScanClass &laserscan)
int CdlLookupClass::setLaserscan( Smart::CommMobileLaserScan &laserscan)
{
  //laserscan.getCartesianRobot(scan);

  //<asteck: date="17.07.2008" other Laserscan-Object>
  scan = laserscan;
  //<asteck>

#if DEBUG_VW_WINDOW
  currentScan = laserscan;
#endif

  return 0;
};

int CdlLookupClass::setParameterWeighting(double a1,double a2,double a3)
{
  alpha1 = a1;
  alpha2 = a2;
  alpha3 = a3;

  return 0;
};

int CdlLookupClass::setEvalPassing(double a1,double a2,double a3,double a4,double a5,double a6)
{
  rangeFarPassing    = a1;
  rangeMediumPassing = a2;
  speedFarPassing    = a3;
  speedMediumPassing = a4;
  speedNearPassing   = a5;
  bonusDistPassing   = a6;

  return 0;
};

int CdlLookupClass::setEvalStopping(double a1,double a2,double a3,double a4,double a5,double a6)
{
  rangeFarStopping    = a1;
  rangeMediumStopping = a2;
  speedFarStopping    = a3;
  speedMediumStopping = a4;
  speedNearStopping   = a5;
//  bonusDistStopping   = 16;
  bonusDistStopping   = a6;
  return 0;
};

int CdlLookupClass::setParameterRobot(double dt,double transacc,double rotacc)
{
  deltat = dt;
  vacc   = transacc;
  wacc   = rotacc;

  return 0;
};

int CdlLookupClass::setHeading(double heading)
{
  goalHeadingRelative = heading;

  return 0;
};

double CdlLookupClass::getHeading()
{
  return goalHeadingRelative;
};

int CdlLookupClass::setMaxDistance(double distance)
{
  maxDistance = distance;
  if (maxDistance > CDL_MAX_DISTANCE) maxDistance=CDL_MAX_DISTANCE;

  return 0;
};

int CdlLookupClass::setDesiredTranslationalSpeed(double speed)
{
  desiredTranslationalSpeed = speed;

  return 0;
};

double CdlLookupClass::getDesiredTranslationalSpeed()
{
  return desiredTranslationalSpeed;
};

int CdlLookupClass::setGoalPosition(double x,double y)
{
  goalX = x;
  goalY = y;

  return 0;
};

int CdlLookupClass::loadCurvatureIndexAscii(char *filename)
{
  FILE   *file;
  double help;
  int    num,format;
  int    vi,wi;

  file=fopen(filename,"r");
  if (file==NULL)
    {
      fprintf(stderr,"Unable to open data file %s for indices.\n",filename);
      return 1;
    }
  else
    {
      // read first line
      num=fscanf(file,"Format : SFB527 (%d)\n",&format);
      if (num!=1)
        {
          fprintf(stderr,"Error in line 1: unknown file format\n");
          fclose(file);
          return 1;
        }

      // read second line
      fscanf(file,"Content : CDL CURVATURE INDICES\n");

      // read third line
      fscanf(file,"Comment : ---\n");

      // read fourth line
      fscanf(file,"VSpaceNumberCellsTrans : %lf\n",&help);
      if ((num!=1) || (help!=CDL_TRA_CELLS))
        {
          fprintf(stderr,"Error in line 4: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read fifth line
      fscanf(file,"VSpaceNumberCellsRot : %lf\n",&help);
      if ((num!=1) || (help!=CDL_ROT_CELLS))
        {
          fprintf(stderr,"Error in line 5: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read data area
      for (vi=0;vi<CDL_TRA_CELLS;vi++)
        {
          for (wi=0;wi<CDL_ROT_CELLS;wi++)
            {
              num=fscanf(file,"%d ",&(indexVW[vi][wi]));
              if (num!=1)
                {
                  fprintf(stderr,"Error in data area: Wrong data format\n");
                  fclose(file);
                  return 1;
                }
            }
          fscanf(file,"\n");
        }
      fclose(file);
      return 0;
    }
};

int CdlLookupClass::loadDistAngleLookupAscii(char *filename)
{
  FILE   *file;
  int    xi,yi,ci;
  double help;
  int    num,format;

  file=fopen(filename,"r");
  if (file==NULL)
    {
      fprintf(stderr,"Unable to open data file %s for lookup-table.\n",filename);
      return 1;
    }
  else
    {
      // read first line
      num=fscanf(file,"Format : SFB527 (%d)\n",&format);
      if (num!=1)
        {
          fprintf(stderr,"Error in line 1: unknown file format\n");
          fclose(file);
          return 1;
        }

      // read second line
      fscanf(file,"Content : CDL LOOKUP TABLE\n");

      // read third line
      fscanf(file,"Comment : ---\n");

      // read fourth line
      num=fscanf(file,"VSpaceMaxTransVel [mm/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_TRA_MAX))
        {
          fprintf(stderr,"Error in line 4: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read fifth line
      num=fscanf(file,"VSpaceMinTransVel [mm/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_TRA_MIN))
        {
          fprintf(stderr,"Error in line 5: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read sixth line
      num=fscanf(file,"VSpaceMaxRotVel [deg/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_ROT_MAX))
        {
          fprintf(stderr,"Error in line 6: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read seventh line
      num=fscanf(file,"VSpaceMinRotVel [deg/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_ROT_MIN))
        {
          fprintf(stderr,"Error in line 7: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read eigth line
      num=fscanf(file,"VSpaceResTrans  [mm/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_TRA_STEP))
        {
          fprintf(stderr,"Error in line 8: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read ninth line
      num=fscanf(file,"VSpaceResRot [deg/s] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_V_ROT_STEP))
        {
          fprintf(stderr,"Error in line 9: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read tenth line
      num=fscanf(file,"VSpaceNumberCellsTrans : %lf\n",&help);
      if ((num!=1) || (help!=CDL_TRA_CELLS))
        {
          fprintf(stderr,"Error in line 10: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read eleventh line
      num=fscanf(file,"VSpaceNumberCellsRot : %lf\n",&help);
      if ((num!=1) || (help!=CDL_ROT_CELLS))
        {
          fprintf(stderr,"Error in line 11: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read twelfth line
      num=fscanf(file,"CartMinX [mm] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_X_MIN))
        {
          fprintf(stderr,"Error in line 12: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read thirteenth line
      num=fscanf(file,"CartMaxX [mm] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_X_MAX))
        {
          fprintf(stderr,"Error in line 13: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read fourteenth line
      num=fscanf(file,"CartMinY [mm] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_Y_MIN))
        {
          fprintf(stderr,"Error in line 14: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read fifteenth line
      num=fscanf(file,"CartMaxY [mm] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_Y_MAX))
        {
          fprintf(stderr,"Error in line 15: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read sixteenth line
      num=fscanf(file,"CartCellSize [mm] : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_RES))
        {
          fprintf(stderr,"Error in line 16: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read seventeenth line
      num=fscanf(file,"CartNumberCellsX : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_X_CELLS))
        {
          fprintf(stderr,"Error in line 17: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read eighteenth line
      num=fscanf(file,"CartNumberCellsY : %lf\n",&help);
      if ((num!=1) || (help!=CDL_C_Y_CELLS))
        {
          fprintf(stderr,"Error in line 18: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read nineteenth line
      num=fscanf(file,"CurvNumberValues : %lf\n",&help);
      if ((num!=1) || (help!=CDL_CURVATURE_INDICES))
        {
          fprintf(stderr,"Error in line 19: Wrong format or wrong value\n");
          fclose(file);
          return 1;
        }

      // read 20. line
      fscanf(file,"Comment : ---\n");

      for (xi=0;xi<CDL_C_X_CELLS;xi++)
        {
          for (yi=0;yi<CDL_C_Y_CELLS;yi++)
            {
              for (ci=0;ci<CDL_CURVATURE_INDICES;ci++)
                {
                  num=fscanf(file,"%d ",&(distLookup[xi][yi][ci]));
                  if (num!=1)
                    {
                      fprintf(stderr,"Error in data area: Wrong data format\n");
                      fclose(file);
                      return 1;
                    }
                  num=fscanf(file,"%d ",&(alphaLookup[xi][yi][ci]));
                  if (num!=1)
                    {
                      fprintf(stderr,"Error in data area: Wrong data format\n");
                      fclose(file);
                      return 1;
                    }
                }
              fscanf(file,"\n");
            }
        }
      fclose(file);
      return 0;
    }
};

int CdlLookupClass::loadDistAngleLookupBin(char *filename)
{
  FILE *file;
  int counter,size;
  double a;

  file = fopen(filename,"rb");
  if (file!=NULL)
    {
      counter=0;
      size=0;

      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_TRA_MAX)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_TRA_MIN)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_ROT_MAX)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_ROT_MIN)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_TRA_STEP)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_V_ROT_STEP)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_TRA_CELLS)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_ROT_CELLS)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_X_MIN)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_X_MAX)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_Y_MIN)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_Y_MAX)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_RES)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_X_CELLS)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_C_Y_CELLS)
        {
          fclose(file);
          return 1;
        };
      size+=fread(&a,sizeof(a),1,file);
      counter++;
      if (a!=(double)CDL_CURVATURE_INDICES)
        {
          fclose(file);
          return 1;
        };

      if (size != counter)
        {
          fclose(file);
          return 1;
        };
      size=fread(&(distLookup[0][0][0]),
                 sizeof(int),
                 (int)(CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES),
                 file);
      if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES))
        {
          fclose(file);
          return 1;
        };

      size=fread(&(alphaLookup[0][0][0]),
                 sizeof(int),
                 (int)(CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES),
                 file);
      if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES))
        {
          fclose(file);
          return 1;
        };

      fclose(file);
      return 0;
    }
  return 1;
}

int CdlLookupClass::loadAccLookupBin(char *filename)
{
  FILE *file;
  int  size;
  int  a;

  file = fopen(filename,"rb");
  if (file!=NULL)
    {
      size=fread(&a,sizeof(a),1,file);
      if ((size!=1) || (a!=(int)CDL_MAX_CURVATURE))
        {
          fclose(file);
          return 1;
        };
      size=fread(&(transAccLookup[0]),
                 sizeof(double),
                 CDL_MAX_CURVATURE,
                 file);
      if (size != CDL_MAX_CURVATURE)
        {
          fclose(file);
          return 1;
        };
      size=fread(&(rotAccLookup[0]),
                 sizeof(double),
                 CDL_MAX_CURVATURE,
                 file);
      if (size != CDL_MAX_CURVATURE)
        {
          fclose(file);
          return 1;
        };
      fclose(file);
      return 0;
    }
  return 1;
}

