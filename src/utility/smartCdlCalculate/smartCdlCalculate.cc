//--------------------------------------------------------------------------
//
//  Copyright (C) 1997/2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
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
//
//--------------------------------------------------------------------------


// -----------------------------------------------------------------
//
// CDL - Curvature Distance Lookup Method
//
// Special version adapted to synchro-drive characteristics
//
// -----------------------------------------------------------------

// writes Index of v/w-Lookup-Table into File
// #define WRITE_INDEX_GNU 1

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "smartCdlCons.hh"
#include "smartCdlTypes.hh"

#ifndef PI
#define PI M_PI
#endif

// -----------------------------------------------------------------
//
// global variables
//
// -----------------------------------------------------------------

// Contains the contour of the vehicle
//      The allowed maximum number of line segments is defined 
//      by CDL_MAX_LINES
struct cdl_polygon_struct polygon;

// lookup index -> curvature parameters
//      my        : y value of circle center point
//      trans_dir : > 0  move forward, < 0  move backward
//      rot_dir   : > 0  turn left,    < 0  turn right
struct cdl_index_struct cdl_index_data[CDL_MAX_CURVATURE];

// v/w-index-lookup-table: v,w -> c
int cdl_index_v_w[CDL_MAX_TRA_CELLS][CDL_MAX_ROT_CELLS];

// vacc, wacc-lookup-table: c -> vMaxAcceleration, wMaxAcceleration
double cdl_curvature_vacc[CDL_MAX_CURVATURE];
double cdl_curvature_wacc[CDL_MAX_CURVATURE];

// This is the array to be calculated
int cdl_dist_lookup[CDL_MAX_X_CELLS][CDL_MAX_Y_CELLS][CDL_MAX_CURVATURE];
int cdl_alph_lookup[CDL_MAX_X_CELLS][CDL_MAX_Y_CELLS][CDL_MAX_CURVATURE];


// -----------------------------------------------------------------
// 
// Different functions
//
// -----------------------------------------------------------------

// -----------------------------------------------------------------
//
// -----------------------------------------------------------------
double min(double a,double b)
{
  if (a<b) return a; else return b;
}

double rad(double a)
{
  return (a/180.0*PI);
}


// ----------------------------------------------------------------- 
// Intersection Circle - Line
//
// Assumes unbounded line length
//
// Circle        : sqr(x-xm)+sqr(y-ym)=r*r
// Line          : a*x+b*y=c
// Intersections : (x1,y1), (x2,y2)
// Precondition  : (a,b) <> (0,0)
//
// Always true x1<=x2, ns Number of intersections
// -----------------------------------------------------------------
int cdl_intersection_circle_line(double xm,double ym,double r,
                                 double a,double b,double c,
                                 double *x1, double *y1,double *x2,double *y2,
                                 int *ns)
{
  double      ab2;
  double      dis;
  double      cc;
  double      wu;

  *ns  = 0;
  ab2 = a*a + b*b;
  if (ab2 != 0.0) {
    cc = c - a*xm - b*ym;
    dis = r*r*ab2 - cc*cc;
    if (dis >= 0.0) {
      *ns = 2;
      wu = sqrt(dis);
      if (wu == 0.0) *ns=1;
      /* ----- damit stets x1 <= x2 gilt ----- */
      if (b >= 0.0) {
        *x1 = xm + (a*cc-b*wu)/ab2;
        *y1 = ym + (b*cc+a*wu)/ab2;
        *x2 = xm + (a*cc+b*wu)/ab2;
        *y2 = ym + (b*cc-a*wu)/ab2;
      } else {
        *x2 = xm + (a*cc-b*wu)/ab2;
        *y2 = ym + (b*cc+a*wu)/ab2;
        *x1 = xm + (a*cc+b*wu)/ab2;
        *y1 = ym + (b*cc-a*wu)/ab2;
      }
    }
  }
  return CDL_OK;
}

// -----------------------------------------------------------------
// Calculates intersection of two lines if available
//
// input lines are assumed as ax+by+c=0
// input    a1,b1,c1         Parameter line 1
// input    a2,b2,c2         Parameter line 2
// output   x,y              line intersection point
// output   status  CDL_OK   found intersection
//                  CDL_INF  lines are equal, infinity intersections
//                  CDL_NO   lines are parallel, no intersection
// -----------------------------------------------------------------
int cdl_line_intersection(double a1,double b1,double c1,
                          double a2,double b2,double c2,
                          double *x,double *y)
{
  int status;

  if ((a1*b2-a2*b1) == 0.0) {
    // parallel lines
    status = CDL_NO;
  } else if ((a2==0.0) && (b2==0.0) && (c2==0.0)) {
    // equal lines
    status = CDL_INF;
  } else if ((a1==0.0) && (b1==0.0) && (c1==0.0)) {
    // equal lines
    status = CDL_INF;
  } else if ((a1/a2==b1/b2) && (a1/a2==c1/c2)) {
    // equal lines
    status = CDL_INF;
  } else {
    // lines not parallel
    *x = (b1*c2-b2*c1)/(a1*b2-a2*b1);
    *y = (c1*a2-c2*a1)/(a1*b2-a2*b1);
    status = CDL_OK;
  }
  return status;
}

// ----------------------------------------------------------------- 
// Transformation Line AB => ax+by=c
// Gradient          : b/a
// y-axis segment    : c
// return value      : OK/NOK
// -----------------------------------------------------------------
int cdl_ab_axbyc(double pax,double pay,double pbx,double pby,
                 double *a,double *b,double *c)
{
  double      deltax,deltay;

  deltax = pbx - pax;
  deltay = pby - pay;
  if (fabs(deltax) < CDL_ACCURACY) {
    // parallel to y
    *a = 1.0;
    *b = 0.0;
    *c = pax;
  } else if (fabs(deltay) < CDL_ACCURACY) {
    // parallel to x
    *a = 0.0;
    *b = 1.0;
    *c = pay;
  } else if ((fabs(deltay) < CDL_ACCURACY) && (fabs(deltax) < CDL_ACCURACY)) {
    // distance A <-> B too small
    return CDL_NOK;
  } else {
    *a = -deltay/deltax;
    *b = 1.0;
    *c = *a * pax + *b * pay;
  }
  return CDL_OK;
}

// --------------------------------------------------------------
// check wether point p is on line ab
//
// return: OK/NOK  point is on line/is not on line
// --------------------------------------------------------------
int cdl_check_point_on_line(double px,double py,
                            double ax,double ay,double bx,double by)
{
  double t;
  double y;

  if ((bx-ax)==0.0) {
    // line parallel to y-axis
    t = (py-ay)/(by-ay);
    if ((px==ax) && (0.0<=t) && (t<=1.0)) {
      return CDL_OK;
    }
  } else {
    t = (px-ax)/(bx-ax);
    y = ay + t*(by-ay);
    if ((0.0<=t) && (t<=1.0) && (fabs(y-py)<CDL_ACCURACY)) {
      return CDL_OK;
    }
  }

  return CDL_NOK;
}

// --------------------------------------------------------------
// check wether point p is inside polygon
//
// return: OK/NOK point is inside/outside
// --------------------------------------------------------------
int cdl_check_point_inside_polygon(double x_p,double y_p,
                                   struct cdl_polygon_struct *polygon)
{
  int     counter;
  double  x_help,y_help;
  double  t;
  double  x;
  int     i;

  counter=0;
  for (i=0;i<polygon->number_of_segments;i++) {
    if (polygon->line[i].y1 != polygon->line[i].y2) {
      if (polygon->line[i].y1 > polygon->line[i].y2) {
        x_help=polygon->line[i].x1;
        polygon->line[i].x1=polygon->line[i].x2;
        polygon->line[i].x2=x_help;
        y_help=polygon->line[i].y1;
        polygon->line[i].y1=polygon->line[i].y2;
        polygon->line[i].y2=y_help;
      }
      t=(y_p-polygon->line[i].y1)/(polygon->line[i].y2-polygon->line[i].y1);
      x=polygon->line[i].x1+t*(polygon->line[i].x2-polygon->line[i].x1);
      if ((0<=t) && (t<1.0) && (x<=x_p)) counter++;
    }
  }
  if ((counter%2)==0) return CDL_NOK; else return CDL_OK;
}


// -----------------------------------------------------------------
//
// Functions especially needed for the CDL calculation
//
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// Calculates circle center point of circle with center point on
// y-axis and circle through P with given radius r. If direction 
// is positiv, center point is on the left of P, else center point 
// is on the right of P.
//
//                             /\ x-axis
//                     P        |      1 intersection 1
//                              |      2 intersection 2
//                              |      distance 1-P = radius
//                              |      distance 2-P = radius
//    y  <------1-----------2------
//
// Left turn: 1 is returned, Right turn: 2 is returned
//
// dir    : > 0 : left turn, < 0 : right turn
// px,py  : point P
// radius : radius of circle
//
// status : 0   everything OK, found intersection
//         <>0  no intersection available
// ------------------------------------------------------------------
int cdl_calc_circle_center_point(double px,double py,double radius,int dir,
                                 double *mx,double *my)
{
  double  s1x,s1y,s2x,s2y;
  int     ns;

  int status;

  status = cdl_intersection_circle_line(px,py,radius,1.0,0.0,0.0,
                                        &s1x,&s1y,&s2x,&s2y,&ns);
  if (ns==0) { 		// no intersection at all
    return CDL_NOK;
  } else if (ns==1) {	// only one intersection
    *mx = 0.0;
    *my = s1y;
    return CDL_OK;
  } else {		// two intersections
    if (dir > 0) {	// take intersection left to P
      if (s1y>py) {
        *my = s1y;
      } else {
        *my = s2y;
      }
    } else {		// take intersection right to P
      if (s1y<py) {
        *my = s1y;
      } else {
        *my = s2y;
      }
    }
    *mx = 0.0;
    return CDL_OK;
  }
  return CDL_NOK;
}

// ---------------------------------------------------------------
//
// coordinate system: mathematisch pos. mit 0 Grad in x-Richtung
// Winkelabstand    : mathematisch positiv, gemessen von A nach B,
//                    Drehrichtung dabei positiv oder negativ
// Rueckgabe        : angle difference
//
// ---------------------------------------------------------------
double cdl_calc_angle(double xs,double ys,double xe,double ye,int direction)
{
  double diff;

  diff = atan2(ye,xe) - atan2(ys,xs);
  // calculation for mathematically positive rotation direction
  if (diff < 0) diff += 2*PI;
  if (direction < 0) {
    // calculation for mathematically negative rotation direction
    diff =  -(2*PI - diff);
    if (diff <= -2*PI) diff += 2*PI;
  }
  return diff;
}

// ---------------------------------------------------------------
// This function calculates the radius r of a circle with center 
// point m through point p
// ---------------------------------------------------------------
int cdl_calc_circle_radius(double mx,double my,double px,double py,double *r)
{
  *r = sqrt((px-mx)*(px-mx)+(py-my)*(py-my));
  
  return CDL_OK;
}


// --------------------------------------------------------------
// This function calculates the remaining distance from a line
// to an obstacle point P if the line is moved forward or
// backward along the x-axis. 
// --------------------------------------------------------------
int cdl_calc_distance_straight(double px,double py,
                               double ax,double ay,double bx,double by,
                               int trans,double *distance)
{
  double   a1,b1,c1,a2,b2,c2;
  double   sx,sy;
  double   hx,hy;
  double   dist1,dist2;
  int      status;

  // first sort the line end points with respect to y-axis
  if (by < ay) {
    hx = bx;bx = ax; ax = hx;
    hy = by;by = ay;ay = hy;
  };
  if (ay==by) {
    // line is parallel to x-axis
    if (ay==py) {
      // point p in line with line ab
      if (trans>0) {
        // move forward
        dist1 = px-ax;
        dist2 = px-bx;
        if ((dist1>=0.0) && (dist2>=0.0)) {
          // there is some space until collision
          *distance = min(dist1,dist2);
        } else if((dist1<0.0) && (dist2<0.0)) {
          // there is some space, but wrong direction, so no collision
          *distance = CDL_MAX_DISTANCE;
        } else {
          // p is on line ab
          // collision distance zero
          *distance = 0.0;
        }
      } else {
        // move backward
        dist1 = ax-px;
        dist2 = bx-px;
        if ((dist1>=0.0) && (dist2>=0.0)) {
          // there is some space until collision
          *distance = min(dist1,dist2);
        } else if ((dist1<0.0) && (dist2<0.0)) {
          // there is some space, but wrong direction, so no collision
          *distance = CDL_MAX_DISTANCE;
        } else {
          // p is on line ab
          // collision distance zero
          *distance = 0.0;
        }
      }
    } else {
      // point p not in line with ab
      // no collision
      *distance = CDL_MAX_DISTANCE;
    }
  } else {
    // line is not parallel to x-axis
    if ((ay<=py) && (py<=by)) {
      // collision can occure
      // second line is in parallel to x-axis with py
      status = cdl_ab_axbyc(ax,ay,bx,by,&a1,&b1,&c1);
      status = cdl_ab_axbyc(px,py,px+1.0,py,&a2,&b2,&c2);
      status = cdl_line_intersection(a1,b1,-c1,a2,b2,-c2,&sx,&sy);
      
// IN DIESEM FALL LIEFERT FKT IMMER EINEN SCHNITTPUNKT
      if (status != CDL_OK) printf("ERROR\n");
   
      if (trans>0) {
        // move forward
        dist1 = px-sx;
        if (dist1>=0.0) {
          // there is some space until collision
          *distance = dist1;
        } else {
          // no collision can occur
          *distance = CDL_MAX_DISTANCE;
        }
      } else {
        // move backward
        dist1 = sx-px;
        if (dist1>=0.0) {
          // there is some space until collision
          *distance = dist1;
        } else {
          // no collision can occur
          *distance = CDL_MAX_DISTANCE;
        }
      }
    } else {
      // no collision possible
      *distance = CDL_MAX_DISTANCE;
    }
  }
  return CDL_OK;
}

// --------------------------------------------------------------
// This function calculates the remaining distance from a line
// to an obstacle point P if the line is moved along an arc
// which is defined by circle center point M on y-axis and point P.
// The direction trans specifies whether the line is moving forward 
// or backward. The direction rot specifies wether the line turns 
// left or right.
// 
// Important hint:
//   the maximum possible distance is limited by the circumference
//   of the circle defined by the considered curvature
//   the maximum possible angle is 2 * PI
//
// px,py       : point P
// my          : circle center point M, x always is 0.0
// ax,ay,bx,by : end point of line
// trans       : > 0 : move forward, < 0 : move backward
// rot         : > 0 : turn left, < 0 : turn right
//
// *distance   : remaining distance, always positive
// *angle      : remaining angle,    always positive
//
// --------------------------------------------------------------
int cdl_calc_distance(double px,double py,double ym,
                      double ax,double ay,double bx,double by,
                      int trans,int rot,
                      double *distance,double *angle,
int ci)
{
  double   xm;
  double   a,b,c; 		// different representation of line AB
  double   s1x,s1y,s2x,s2y;	// intersection of circle with line AB
  int      number;		// number of intersections
  double   arc;                 // arc length of circle segment
  double   alpha;               // alpha of circle segment
  double   r;                   // radius of circle with m through p

  double   intersection_x,intersection_y,point_x,point_y;
  int      status;

  xm = 0.0;

  status = cdl_ab_axbyc(ax,ay,bx,by,&a,&b,&c);
  status = cdl_calc_circle_radius(xm,ym,px,py,&r);
  status = cdl_intersection_circle_line(xm,ym,r,a,b,c,&s1x,&s1y,&s2x,&s2y,&number);

  *distance = min(CDL_MAX_DISTANCE,fabs(2*PI*ym));
  *angle    = 2*PI;

  if (number == 0) {
    // no intersection
    *distance = min(CDL_MAX_DISTANCE,*distance);
    *angle    = min(2*PI,*angle);
  }
  if (number > 0) {
    // at least one intersection
    status = cdl_check_point_on_line(s1x,s1y,ax,ay,bx,by);
    if (status==CDL_OK) {
      // calculated intersection is true intersection
      // now we have obstacle point p and intersection s1 and have to
      // calculate the distance when moving along the arc

      // because the angle calculation assumes the origin for rotation
      // in (0,0), we have to transform the coordinates first.
      intersection_x = s1x - xm;
      intersection_y = s1y - ym;
      point_x        = px - xm;
      point_y        = py - ym;
      alpha          = cdl_calc_angle(intersection_x,intersection_y,point_x,point_y,rot);

      // ---------------------------------------------------------------------
      // now alpha contains the remaining rotating angle until collision
      // with obstacle point. Now we have to compute the remaining translation
      // distance with respect to the center point of the robot.
      // ---------------------------------------------------------------------
      arc = alpha * ym;
      if (arc < 0.0) arc *= -1.0;        // to ensure positive arc length
      *distance = min(arc,*distance);
      if (alpha < 0.0) alpha *= -1.0;    // to ensure positive angle of circle segment
      *angle    = min(alpha,*angle);
    } else {
      // intersection point 1 not on line
    }
  }
  if (number > 1) {
    // there is another intersection
    status = cdl_check_point_on_line(s2x,s2y,ax,ay,bx,by);
    if (status==CDL_OK) {
      // calculated intersection is true intersection, same procedure as before
      intersection_x = s2x - xm;
      intersection_y = s2y - ym;
      point_x        = px - xm;
      point_y        = py - ym;
      alpha          = cdl_calc_angle(intersection_x,intersection_y,point_x,point_y,rot);

      // compare to the above case ...
      arc   = alpha * ym;
      if (arc < 0.0) arc *= -1.0;	// to ensure positive arc length
      *distance = min(arc,*distance);
      if (alpha < 0.0) alpha *= -1.0;   // to ensure positive angle of circle segment
      *angle    = min(alpha,*angle);
    } else {
      // intersection point 2 not on line
    }
  }
  return CDL_OK;
}


// --------------------------------------------------------------
//
// Different functions to calculate the lookup-table
//
// --------------------------------------------------------------

// --------------------------------------------------------------
//
// Calculation of the curvature indices of the lookup-table,
// initialization of the v/w-grid and of the vacc,wacc-lookup-table.
//
// Remarks:
// - all combinations of v/w, which result in the same curvature c,
//   lead to the same trajectory of the robot. The only difference
//   is the different velocity when travelling with curvature c.
//
// Gleiche Curvatures liegen auf einer Geraden, so dass die
// Steigung der Geraden von 0/0 zu v/w bzw. deren Winkel
// gegenueber der x-Achse als Index fuer die Lookup-Tabellen
// verwendet werden kann. 
//
// Maximal sollten nur soviele Curvature-Werte unterschieden 
// werden, wie es Randzellen im diskreten Raster des v/w-Raumes
// gibt, da alle anderen Zellen durch diese Geraden mit abgedeckt
// werden und keine Randzellen aufgrund unzureichender Aufloesung
// zusammengefasst werden muessen.
//
// Die Indizes der Tabelle sind so organisiert, dass die Zelle
// am Ursprung den Bereich von jeweils -STEP/2 bis +STEP/2 umfasst.
// 
// --------------------------------------------------------------

// --------------------------------------------------------------
// Der Algorithmus:
//
// - diese werden auf 360 Grad gleichmaessig verteilt
// - fuer jede Zelle im v/w-Raum wird nun der richtige Index bestimmt
//
// Aufwandsabschaetzung:
//
// - Diskretisierung Roboterumfeld +/- 3m bei 10cm ergibt 61x61 = 3721 Zellen
// - Diskretisierung v/w Raum:
//     v: +/- 1000mm/s in 10mm/s-Schritten ergibt 201 Werte
//     w: +/- 70 Grad/s in 1 Grad/s        ergibt 141 Werte
//    
//     v/w-Raster hat die Groesse 201x141 = 28341 Zellen
//     Umfang 201+201+141+141-4           = 680   Zellen
//
//     gesamte Lookup-Tabelle kartesischer Raum:
//     3721 Zellen x 680 Werte            = 2 530 280 Werte
//
// - weitere Reduktion:
//     Einschraenkung der Curvature auf 360 verschiedene Werte
//     3721 Zellen x 360 Werte            = 1 339 560 Werte
//
// - Genauigkeit 20 mm ergibt bei 1 Byte 255 * 20 mm = 5100 mm
//               10 mm                               = 2550 mm
// -----------------------------------------------------------------
     
// global: curvature-Feld
//         index_v_w

// Berechnung der Winkel zu den Indizes wie folgt:
// - einmal alle Randzellen durchgehen 
// - fuer jede Randzelle das Index-Paar berechnen
// - aus dem Indexpaar den zugehoerigen Winkel bestimmen
// - Winkel merken und Index erhoehen
//
// - nun alle Zellen des v-w-Rasters durchgehen
// - fuer jede Zelle den Winkel wie oben bestimmen
// - nun in der zuvor berechneten Liste den Winkel mit der kleinsten
//   Abweichung suchen und diesen Index eintragen
//
// - 
//
//
//
int cdl_calculate_lookup_index(void)
{
  int    index,vi,wi;
  double angle_step;
  double alpha; 
  double v,w;
  double cdl_v_rot_min,cdl_v_rot_max;
  double cdl_a_rot_max,cdl_a_tra_max;
  double cdl_v_rot_step;
  double radius;

  double curvature[CDL_MAX_CURVATURE];
  int    curvature_counter;
  double diff,diff_old;
  int    help;

#ifdef WRITE_INDEX_GNU
  FILE *file;
  file=fopen("gnu.dat","w");
#endif

  // ---------------------------------------------
  // Generating curvature indices ...
  // Calculating Tacceleration ...
  // ---------------------------------------------
  // v  translation  y-direction
  // w  rotation     x-direction
  angle_step     = CDL_ANGLE_STEP;
  cdl_v_rot_min  = rad(CDL_V_ROT_MIN);
  cdl_v_rot_max  = rad(CDL_V_ROT_MAX);
  cdl_v_rot_step = rad(CDL_V_ROT_STEP);
  cdl_a_rot_max  = rad(CDL_A_ROT_MAX);
  cdl_a_tra_max  = CDL_A_TRA_MAX;

  curvature_counter=0;
  wi=0;
  for (vi=0;vi<CDL_TRA_CELLS-1;vi++) {
    v=CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP;
    w=cdl_v_rot_min+wi*cdl_v_rot_step;
    
    // ------------------------------------------------------
    // calculate maximum accelerations for given curvature
    // ------------------------------------------------------
    if (fabs(v) < CDL_V_TRA_STEP/2.0) {
      // no translation => rotate at the current position
      cdl_curvature_vacc[curvature_counter]=0.0;
      cdl_curvature_wacc[curvature_counter]=cdl_a_rot_max;
    } else if (fabs(w) < cdl_v_rot_step/2.0) {
      // no rotation => driving straight ahead
      cdl_curvature_vacc[curvature_counter]=cdl_a_tra_max;
      cdl_curvature_wacc[curvature_counter]=0.0;
    } else {
      // translation and rotation
      cdl_curvature_vacc[curvature_counter]=min(cdl_a_tra_max,
                                                fabs(cdl_a_rot_max*v/w));
      cdl_curvature_wacc[curvature_counter]=min(cdl_a_rot_max,
                                                fabs(cdl_a_tra_max*w/v));
    };

    // ------------------------------------------------------
    //
    // ------------------------------------------------------
    alpha=atan2(v,w);
    curvature[curvature_counter]=alpha;
    curvature_counter++;
  }

  vi=CDL_TRA_CELLS-1;
  for (wi=0;wi<CDL_ROT_CELLS-1;wi++) {
    v=CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP;
    w=cdl_v_rot_min+wi*cdl_v_rot_step;

    // ------------------------------------------------------
    // calculate maximum accelerations for given curvature
    // ------------------------------------------------------
    if (fabs(v) < CDL_V_TRA_STEP/2.0) {
      // no translation => rotate at the current position
      cdl_curvature_vacc[curvature_counter]=0.0;
      cdl_curvature_wacc[curvature_counter]=cdl_a_rot_max;
    } else if (fabs(w) < cdl_v_rot_step/2.0) {
      // no rotation => driving straight ahead
      cdl_curvature_vacc[curvature_counter]=cdl_a_tra_max;
      cdl_curvature_wacc[curvature_counter]=0.0;
    } else {
      // translation and rotation
      cdl_curvature_vacc[curvature_counter]=min(cdl_a_tra_max,
                                                fabs(cdl_a_rot_max*v/w));
      cdl_curvature_wacc[curvature_counter]=min(cdl_a_rot_max,
                                                fabs(cdl_a_tra_max*w/v));
    };

    // ------------------------------------------------------
    //
    // ------------------------------------------------------
    alpha   = atan2(v,w);
    curvature[curvature_counter]=alpha;
    curvature_counter++;
  }

  wi=CDL_ROT_CELLS-1;
  for (vi=CDL_TRA_CELLS-1;vi>0;vi--) {
    v=CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP;
    w=cdl_v_rot_min+wi*cdl_v_rot_step;

    // ------------------------------------------------------
    // calculate maximum accelerations for given curvature
    // ------------------------------------------------------
    if (fabs(v) < CDL_V_TRA_STEP/2.0) {
      // no translation => rotate at the current position
      cdl_curvature_vacc[curvature_counter]=0.0;
      cdl_curvature_wacc[curvature_counter]=cdl_a_rot_max;
    } else if (fabs(w) < cdl_v_rot_step/2.0) {
      // no rotation => driving straight ahead
      cdl_curvature_vacc[curvature_counter]=cdl_a_tra_max;
      cdl_curvature_wacc[curvature_counter]=0.0;
    } else {
      // translation and rotation
      cdl_curvature_vacc[curvature_counter]=min(cdl_a_tra_max,
                                                fabs(cdl_a_rot_max*v/w));
      cdl_curvature_wacc[curvature_counter]=min(cdl_a_rot_max,
                                                fabs(cdl_a_tra_max*w/v));
    };

    // ------------------------------------------------------
    //
    // ------------------------------------------------------
    alpha                        = atan2(v,w);
    curvature[curvature_counter] = alpha;
    curvature_counter++;
  }

  vi=0;
  for (wi=CDL_ROT_CELLS-1;wi>0;wi--) {
    v=CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP;
    w=cdl_v_rot_min+wi*cdl_v_rot_step;

    // ------------------------------------------------------
    // calculate maximum accelerations for given curvature
    // ------------------------------------------------------
    if (fabs(v) < CDL_V_TRA_STEP) {
      // no translation => rotate at the current position
      cdl_curvature_vacc[curvature_counter]=0.0;
      cdl_curvature_wacc[curvature_counter]=cdl_a_rot_max;
    } else if (fabs(w) < cdl_v_rot_step/2.0) {
      // no rotation => driving straight ahead
      cdl_curvature_vacc[curvature_counter]=cdl_a_tra_max;
      cdl_curvature_wacc[curvature_counter]=0.0;
    } else {
      // translation and rotation
      cdl_curvature_vacc[curvature_counter]=min(cdl_a_tra_max,
                                                fabs(cdl_a_rot_max*v/w));
      cdl_curvature_wacc[curvature_counter]=min(cdl_a_rot_max,
                                                fabs(cdl_a_tra_max*w/v));
    };

    // ------------------------------------------------------
    //
    // ------------------------------------------------------
    alpha   = atan2(v,w);
    curvature[curvature_counter]=alpha;
    curvature_counter++;
  }

  // --------------------------------------------------------
  // now fill in the curvature index lookup table
  // --------------------------------------------------------
  for (vi=0;vi<CDL_TRA_CELLS;vi++) {
    for (wi=0;wi<CDL_ROT_CELLS;wi++) {
      v=CDL_V_TRA_MIN+vi*CDL_V_TRA_STEP;
      w=cdl_v_rot_min+wi*cdl_v_rot_step;
      alpha   = atan2(v,w);

      diff_old = 1000.0;
      for (help=0;help<curvature_counter;help++) {
        diff = alpha - curvature[help];
        if (diff < 0) diff *= -1.0;
        if (diff < diff_old) {
          cdl_index_v_w[vi][wi] = help;
          diff_old = diff;
        }
      }

#ifdef WRITE_INDEX_GNU
  fprintf(file,"%d %d %d\n",(int)index_v,(int)index_w,cdl_index_v_w[vi][wi]);
#endif

      // Weil der Fahrzeugbezugspunkt mittig im Roboter liegt,
      // ergibt sich der Kreismittelpunkt direkt aus dem zur
      // curvature gehoerenden Radius. Bei einem Dreirad ist
      // beispielsweise der Achsabstand einzubeziehen.
      index = cdl_index_v_w[vi][wi];
      if (v < 0.0) {
        cdl_index_data[index].trans_dir = -1;
      } else if (v == 0.0) {
        cdl_index_data[index].trans_dir = 0;
      } else {
        cdl_index_data[index].trans_dir = 1;
      }
      if (w < 0.0) {
        cdl_index_data[index].rot_dir = -1;
        radius = v/w;
      } else if ((-cdl_v_rot_step/2.0 < w) && (w < cdl_v_rot_step/2.0)) {
        cdl_index_data[index].rot_dir = 0;
        radius = 0.0;    // muesste eigentlich unendlich sein
      } else {
        cdl_index_data[index].rot_dir = 1;
        radius = v/w;
      }
      cdl_index_data[index].my = radius;

    }
 
#ifdef WRITE_INDEX_GNU
  fprintf(file,"\n");
#endif
  }

#ifdef WRITE_INDEX_GNU
  fclose(file);
#endif

  return CDL_OK;
}


// --------------------------------------------------------------
//
// Loop to calculate lookuptable entries of local cartesian robot map
//
// --------------------------------------------------------------

// --------------------------------------------------------------
// Der Algorithmus:
//
// Algorithmus:
// 
// x-Index kartesischer Raum {
//   y-Index kartesischer Raum {
//     Curvature Index von 0 .. max {
//       Segmentnummer der Fahrzeugkontur {
//         Schnittpunkt(e) Fahrzeugkontursegment mit Kreis um M durch Zelle (x,y)
//       }
//       kleinste Kreisbogenlaenge -> Lookup-Tabelle als freie Strecke eintragen
//     }
//   }
// }
//
//
//
// muﬂ erweitert werden, damit auch Winkel berechnet wird !!!!
//
//
//
// ---------------------------------------------------------------

int cdl_calculate_lookup_table(struct cdl_polygon_struct *polygon)
{
  int      xi,yi,ci,seg;
  double   x,y;
  int      status;

  int      trans;
  int      rot;
  double   distance;
  double   angle;
  double   my;

  double   min_distance;
  double   min_angle;

  // -------------------------------------------------------------
  //
  // -------------------------------------------------------------

printf("Einsprung Lookup Table\n");

  for (xi=0;xi<CDL_C_X_CELLS;xi++) {         // x-index cartesian space
    x=CDL_C_X_MIN+xi*CDL_C_RES;
    for (yi=0;yi<CDL_C_Y_CELLS;yi++) {       // y-index cartesian space
      y=CDL_C_Y_MIN+yi*CDL_C_RES;
      status = cdl_check_point_inside_polygon(x,y,polygon);
      if (status == CDL_OK) {
        // -------------------------------------------------------
        // point p is inside robot => remaining distance zero
        // -------------------------------------------------------
        for (ci=0;ci<CDL_CURVATURE_INDICES;ci++) {
          cdl_dist_lookup[xi][yi][ci] = 0;
          cdl_alph_lookup[xi][yi][ci] = 0;
        }
      } else {
        // -------------------------------------------------------
        // point p is outside of robot => calculate distance
        // -------------------------------------------------------
        for (ci=0;ci<CDL_CURVATURE_INDICES;ci++) {
          min_distance = CDL_MAX_DISTANCE;
          min_angle    = 2*PI;
          for (seg=0;seg<polygon->number_of_segments;seg++) {
            // ---------------------------------------------------
            // calculate intersections
            // ---------------------------------------------------
            my     = cdl_index_data[ci].my;
            trans  = cdl_index_data[ci].trans_dir;
            rot    = cdl_index_data[ci].rot_dir;
            if ((trans > 0) || (trans == 0)) {
              // ---------- robot drives forward
              if (rot != 0) {
                // ---------- robot drives forward and turns or turns in place
                status = cdl_calc_distance(x,y,my,
                                 polygon->line[seg].x1,polygon->line[seg].y1,
                                 polygon->line[seg].x2,polygon->line[seg].y2,
                                 trans,rot,
                                 &distance,&angle,
ci);
              } else {
                // ---------- robot drives forward, straight line
                status = cdl_calc_distance_straight(x,y,
                                 polygon->line[seg].x1,polygon->line[seg].y1,
                                 polygon->line[seg].x2,polygon->line[seg].y2,
                                 trans,
                                 &distance);
                angle = 0.0;
              }
            } else if (trans < 0) {
              // ---------- robot drives backward
              if (rot != 0) {
                // ---------- robot drives backward and turns
                status = cdl_calc_distance(x,y,my,
                                 polygon->line[seg].x1,polygon->line[seg].y1,
                                 polygon->line[seg].x2,polygon->line[seg].y2,
                                 trans,rot,
                                 &distance,&angle,
ci);
              } else {
                // ---------- robot drives backward, straight line
                status = cdl_calc_distance_straight(x,y,
                                 polygon->line[seg].x1,polygon->line[seg].y1,
                                 polygon->line[seg].x2,polygon->line[seg].y2,
                                 trans,
                                 &distance);
                angle = 0.0;
              }
            } else {
              // ---------- robot doesn't move at all
              distance = 0.0;
              angle    = 0.0;
            }

            min_distance = min(min_distance,distance);
            min_angle    = min(min_angle,angle);

          }
          // ---------------------------------------------------------------
          // Now all segments have been considered. The minimum distance
          // drivable using the current curvature is in min_distance. This
          // is not the distance of the hull of the robot until collision,
          // but the distance of the powered wheel until collision.
          // The according rotation angle is in min_alpha. Both values
          // are always positive, [0,CDL_MAX_DISTANCE], [0, 2 PI]
          // ---------------------------------------------------------------
          cdl_dist_lookup[xi][yi][ci] = (int)min_distance;
          cdl_alph_lookup[xi][yi][ci] = (int)(min_angle*1000.0);
        } 
      }
    }
  }
for (xi=0;xi<CDL_C_X_CELLS;xi++) {         // x-index cartesian space
  for (yi=0;yi<CDL_C_Y_CELLS;yi++) {       // y-index cartesian space
printf("%d ",cdl_dist_lookup[xi][yi][370]);
  }
  printf("\n");
}

  return CDL_OK;
}

// --------------------------------------------------------------
// To save space distances are discretised to a lower resolution.
// distResolution  : in mm
// angleResolution : in deg
// --------------------------------------------------------------
int cdl_lower_resolution(double distResolution,double angleResolution)
{
  int xi,yi,ci;

  angleResolution = angleResolution*PI/180.0;

  for (xi=0;xi<CDL_C_X_CELLS;xi++) {               // x-index cartesian space
    for (yi=0;yi<CDL_C_Y_CELLS;yi++) {             // y-index cartesian space
      for (ci=0;ci<CDL_CURVATURE_INDICES;ci++) {   // curvature-index
        cdl_dist_lookup[xi][yi][ci] = 
         (int)(floor((double)cdl_dist_lookup[xi][yi][ci]/distResolution)*distResolution);
        cdl_alph_lookup[xi][yi][ci] =
         (int)(floor((double)cdl_alph_lookup[xi][yi][ci]/angleResolution)*angleResolution);
      }
    }
  }
  return CDL_OK;
}

// --------------------------------------------------------------
//
//
// Different additional support functions to handle files etc.
//
//
// --------------------------------------------------------------

// --------------------------------------------------------------
// Write GNU-Plot-File with remaining distances if vehicle is
// moved with specified curvature.
//
// Jede Zelle der kartesischen Karte enth"alt die Entfernung,
// die das Fahrzeug bei der angegebenen Curvature fahren kann,
// bis es mit der jeweiligen Zelle kollidiert.
// --------------------------------------------------------------
int cdl_extract_distance_values(char *filename,double v,double w)
{
  FILE   *file;
  int    xi,yi;
  int    vi,wi,ci;
  double cdl_v_rot_step,cdl_v_rot_min;

  file = fopen(filename,"w");
  if (file==NULL) {
    fprintf(stderr,"Unable to open data file %s for Distance-Plot.\n",filename);
  } else {
    // Im Array ist an [0,0] der Eintrag fuer [V_ROT_MIN,V_TRA_MIN]
    cdl_v_rot_min  = rad(CDL_V_ROT_MIN);
    cdl_v_rot_step = rad(CDL_V_ROT_STEP);
    vi = (int)(floor((v-CDL_V_TRA_MIN)/CDL_V_TRA_STEP+0.5));
    wi = (int)(floor((w-cdl_v_rot_min)/cdl_v_rot_step+0.5));

    ci      = cdl_index_v_w[vi][wi];

printf("vi,wi      : %d %d\n",vi,wi);
printf("Index      : %d\n",ci);

    for (xi=0;xi<CDL_C_X_CELLS;xi++) {              // x-index cartesian space
      for (yi=0;yi<CDL_C_Y_CELLS;yi++) {            // y-index cartesian space
        fprintf(file,"%d %d %d\n",xi,yi,cdl_dist_lookup[xi][yi][ci]);
      }
      fprintf(file,"\n");
    }

    fclose(file);
  }
  return CDL_OK;
}

// --------------------------------------------------------------
// 
// --------------------------------------------------------------
int cdl_save_curvature_index_ascii(char *filename)
{
  FILE *file;
  int  vi,wi;

  file = fopen(filename,"w");
  if (file!=NULL) {
    fprintf(file,"Format : SFB527 (100)\n");
    fprintf(file,"Content : CDL CURVATURE INDICES\n");
    fprintf(file,"Comment : ---\n");
    fprintf(file,"VSpaceNumberCellsTrans : %f\n",CDL_TRA_CELLS);
    fprintf(file,"VSpaceNumberCellsRot : %f\n",CDL_ROT_CELLS);
 
    for (vi=0;vi<CDL_TRA_CELLS;vi++) {
      for (wi=0;wi<CDL_ROT_CELLS;wi++) {
        fprintf(file,"%d ",cdl_index_v_w[vi][wi]);
      }
      fprintf(file,"\n");
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}   

int cdl_read_curvature_index_ascii(char *filename)
{
  FILE   *file;
  double help;
  int    num,format;
  int    vi,wi;

  file=fopen(filename,"r");
  if (file!=NULL) {
    // read first line
    num=fscanf(file,"Format : SFB527 (%d)\n",&format);
    if (num!=1) {
      fprintf(stderr,"Error in line 1: unknown file format\n");
      fclose(file);
      return CDL_NOK;
    }

    // read second line
    fscanf(file,"Content : SAF CURVATURE INDICES\n");

    // read third line
    fscanf(file,"Comment : ---\n");

    // read fourth line
    fscanf(file,"VSpaceNumberCellsTrans : %lf\n",&help);
    if ((num!=1) || (help!=CDL_TRA_CELLS)) {
      fprintf(stderr,"Error in line 4: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read fifth line
    fscanf(file,"VSpaceNumberCellsRot : %lf\n",&help);
    if ((num!=1) || (help!=CDL_ROT_CELLS)) {
      fprintf(stderr,"Error in line 5: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read data area
    for (vi=0;vi<CDL_TRA_CELLS;vi++) {
      for (wi=0;wi<CDL_ROT_CELLS;wi++) {
        num=fscanf(file,"%d ",&(cdl_index_v_w[vi][wi]));
        if (num!=1) {
          fprintf(stderr,"Error in data area: Wrong data format\n");
          fclose(file);
          return CDL_NOK;
        }
      }
      fscanf(file,"\n");
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
};

// --------------------------------------------------------------
//
// --------------------------------------------------------------
int cdl_save_dist_angle_lookup_ascii(char *filename)
{
  FILE *file;
  int  xi,yi,ci;

  file = fopen(filename,"w");
  if (file==NULL) {
    fprintf(stderr,"Unable to open data file %s for lookup-table.\n",filename);
  } else {
    fprintf(file,"Format : SFB527 (200)\n");
    fprintf(file,"Content : CDL DIST LOOKUP TABLE\n");
    fprintf(file,"Comment : ---\n");
    fprintf(file,"VSpaceMaxTransVel [mm/s] : %f\n",CDL_V_TRA_MAX);
    fprintf(file,"VSpaceMinTransVel [mm/s] : %f\n",CDL_V_TRA_MIN);
    fprintf(file,"VSpaceMaxRotVel [deg/s] : %f\n",CDL_V_ROT_MAX);
    fprintf(file,"VSpaceMinRotVel [deg/s] : %f\n",CDL_V_ROT_MIN);
    fprintf(file,"VSpaceResTrans  [mm/s] : %f\n",CDL_V_TRA_STEP);
    fprintf(file,"VSpaceResRot [deg/s] : %f\n",CDL_V_ROT_STEP);
    fprintf(file,"VSpaceNumberCellsTrans : %f\n",CDL_TRA_CELLS);
    fprintf(file,"VSpaceNumberCellsRot : %f\n",CDL_ROT_CELLS);
    fprintf(file,"CartMinX [mm] : %f\n",CDL_C_X_MIN);
    fprintf(file,"CartMaxX [mm] : %f\n",CDL_C_X_MAX);
    fprintf(file,"CartMinY [mm] : %f\n",CDL_C_Y_MIN);
    fprintf(file,"CartMaxY [mm] : %f\n",CDL_C_Y_MAX);
    fprintf(file,"CartCellSize [mm] : %f\n",CDL_C_RES);
    fprintf(file,"CartNumberCellsX : %f\n",CDL_C_X_CELLS);
    fprintf(file,"CartNumberCellsY : %f\n",CDL_C_Y_CELLS);
    fprintf(file,"CurvNumberValues : %f\n",CDL_CURVATURE_INDICES);

    for (xi=0;xi<CDL_C_X_CELLS;xi++) {              // x-index cartesian space
      for (yi=0;yi<CDL_C_Y_CELLS;yi++) {            // y-index cartesian space
        for (ci=0;ci<CDL_CURVATURE_INDICES;ci++) {  // curvature-index
          fprintf(file,"%d ",cdl_dist_lookup[xi][yi][ci]);
          fprintf(file,"%d ",cdl_alph_lookup[xi][yi][ci]);
        }
        fprintf(file,"\n");
      }
    }

    fclose(file);
    return CDL_OK;
  }    
  return CDL_NOK;
}

int cdl_save_dist_angle_lookup_bin(char *filename)
{
  FILE *file;
  int counter,size;
  double a;

  file = fopen(filename,"wb");
  if (file!=NULL) {
    counter=0;
    size=0;

    a=(double)CDL_V_TRA_MAX;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_V_TRA_MIN;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_V_ROT_MAX;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_V_ROT_MIN;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_V_TRA_STEP;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_V_ROT_STEP;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_TRA_CELLS;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_ROT_CELLS;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_X_MIN;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_X_MAX;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_Y_MIN;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_Y_MAX;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_RES;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_X_CELLS;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_C_Y_CELLS;size+=fwrite(&a,sizeof(a),1,file);counter++;
    a=(double)CDL_CURVATURE_INDICES;size+=fwrite(&a,sizeof(a),1,file);counter++;

    if (size != counter) {
      fclose(file);
      return CDL_NOK;
    };

    size=fwrite(&(cdl_dist_lookup[0][0][0]),
                sizeof(int),
                CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES,
                file);
    if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES)) {
      fclose(file);
      return CDL_NOK;
    };

    size=fwrite(&(cdl_alph_lookup[0][0][0]),
                sizeof(int),
                CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES,
                file);
    if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES)) {
      fclose(file);
      return CDL_NOK;
    };

    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

int cdl_load_dist_angle_lookup_ascii(char *filename)
{
  FILE   *file;
  int    xi,yi,ci;
  double help;
  int    num,format;

  file=fopen(filename,"r");
  if (file!=NULL) {
    // read first line
    num=fscanf(file,"Format : SFB527 (%d)\n",&format);
    if (num!=1) {
      fprintf(stderr,"Error in line 1: unknown file format\n");
      fclose(file);
      return CDL_NOK;
    }

    // read second line
    fscanf(file,"Content : CDL LOOKUP TABLE\n");

    // read third line
    fscanf(file,"Comment : ---\n");

    // read fourth line
    num=fscanf(file,"VSpaceMaxTransVel [mm/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_TRA_MAX)) {
      fprintf(stderr,"Error in line 4: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read fifth line
    num=fscanf(file,"VSpaceMinTransVel [mm/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_TRA_MIN)) {
      fprintf(stderr,"Error in line 5: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }
    // read sixth line
    num=fscanf(file,"VSpaceMaxRotVel [deg/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_ROT_MAX)) {
      fprintf(stderr,"Error in line 6: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read seventh line
    num=fscanf(file,"VSpaceMinRotVel [deg/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_ROT_MIN)) {
      fprintf(stderr,"Error in line 7: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read eigth line
    num=fscanf(file,"VSpaceResTrans  [mm/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_TRA_STEP)) {
      fprintf(stderr,"Error in line 8: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read ninth line
    num=fscanf(file,"VSpaceResRot [deg/s] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_V_ROT_STEP)) {
      fprintf(stderr,"Error in line 9: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read tenth line
    num=fscanf(file,"VSpaceNumberCellsTrans : %lf\n",&help);
    if ((num!=1) || (help!=CDL_TRA_CELLS)) {
      fprintf(stderr,"Error in line 10: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read eleventh line
    num=fscanf(file,"VSpaceNumberCellsRot : %lf\n",&help);
    if ((num!=1) || (help!=CDL_ROT_CELLS)) {
      fprintf(stderr,"Error in line 11: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read twelfth line
    num=fscanf(file,"CartMinX [mm] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_X_MIN)) {
      fprintf(stderr,"Error in line 12: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read thirteenth line
    num=fscanf(file,"CartMaxX [mm] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_X_MAX)) {
      fprintf(stderr,"Error in line 13: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read fourteenth line
    num=fscanf(file,"CartMinY [mm] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_Y_MIN)) {
      fprintf(stderr,"Error in line 14: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read fifteenth line
    num=fscanf(file,"CartMaxY [mm] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_Y_MAX)) {
      fprintf(stderr,"Error in line 15: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read sixteenth line
    num=fscanf(file,"CartCellSize [mm] : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_RES)) {
      fprintf(stderr,"Error in line 16: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read seventeenth line
    num=fscanf(file,"CartNumberCellsX : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_X_CELLS)) {
      fprintf(stderr,"Error in line 17: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read eighteenth line
    num=fscanf(file,"CartNumberCellsY : %lf\n",&help);
    if ((num!=1) || (help!=CDL_C_Y_CELLS)) {
      fprintf(stderr,"Error in line 18: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read nineteenth line
    num=fscanf(file,"CurvNumberValues : %lf\n",&help);
    if ((num!=1) || (help!=CDL_CURVATURE_INDICES)) {
      fprintf(stderr,"Error in line 19: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read 20. line
    fscanf(file,"Comment : ---\n");

    for (xi=0;xi<CDL_C_X_CELLS;xi++) {
      for (yi=0;yi<CDL_C_Y_CELLS;yi++) {
        for (ci=0;ci<CDL_CURVATURE_INDICES;ci++) {
          num=fscanf(file,"%d ",&(cdl_dist_lookup[xi][yi][ci]));
          if (num!=1) {
            fprintf(stderr,"Error in data area: Wrong data format\n");
            fclose(file);
            return CDL_NOK;
          }
          num=fscanf(file,"%d ",&(cdl_alph_lookup[xi][yi][ci]));
          if (num!=1) {
            fprintf(stderr,"Error in data area: Wrong data format\n");
            fclose(file);
            return CDL_NOK;
          }
        }
        fscanf(file,"\n");
      }
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
};

int cdl_load_dist_angle_lookup_bin(char *filename)
{
  FILE *file;
  int counter,size;
  double a;

  file = fopen(filename,"rb");
  if (file!=NULL) {
    counter=0;
    size=0;

    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_TRA_MAX) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_TRA_MIN) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_ROT_MAX) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_ROT_MIN) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_TRA_STEP) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_V_ROT_STEP) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_TRA_CELLS) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_ROT_CELLS) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_X_MIN) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_X_MAX) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_Y_MIN) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_Y_MAX) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_RES) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_X_CELLS) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_C_Y_CELLS) {
      fclose(file);
      return CDL_NOK;
    };
    size+=fread(&a,sizeof(a),1,file);counter++;
    if (a!=(double)CDL_CURVATURE_INDICES) {
      fclose(file);
      return CDL_NOK;
    };

    if (size != counter) {
      fclose(file);
      return CDL_NOK;
    };

    size=fread(&(cdl_dist_lookup[0][0][0]),
                sizeof(int),
                CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES,
                file);
    if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES)) {
      fclose(file);
      return CDL_NOK;
    };

    size=fread(&(cdl_alph_lookup[0][0][0]),
                sizeof(int),
                CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES,
                file);
    if (size != (CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES)) {
      fclose(file);
      return CDL_NOK;
    };

    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------
int cdl_save_acc_lookup_bin(char *filename)
{
  FILE *file;
  int size;
  int a;

  file = fopen(filename,"wb");
  if (file!=NULL) {
    a=(int)CDL_MAX_CURVATURE;size=fwrite(&a,sizeof(a),1,file);
    if (size != 1) {
      fclose(file);
      return CDL_NOK;
    }
    size=fwrite(&(cdl_curvature_vacc[0]),
                sizeof(double),
                CDL_MAX_CURVATURE,
                file);
    if (size != CDL_MAX_CURVATURE) {
      fclose(file);
      return CDL_NOK;
    }
    size=fwrite(&(cdl_curvature_wacc[0]),
                sizeof(double),
                CDL_MAX_CURVATURE,
                file);
    if (size != CDL_MAX_CURVATURE) {
      fclose(file);
      return CDL_NOK;
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

int cdl_load_acc_lookup_bin(char *filename)
{
  FILE *file;
  int size;
  int a;

  file = fopen(filename,"rb");
  if (file!=NULL) {
    size=fread(&a,sizeof(a),1,file);
    if ((size!=1) || (a!=(int)CDL_MAX_CURVATURE)) {
      fclose(file);
      return CDL_NOK;
    }
    size=fread(&(cdl_curvature_vacc[0]),
               sizeof(double),
               CDL_MAX_CURVATURE,
               file);
    if (size != CDL_MAX_CURVATURE) {
      fclose(file);
      return CDL_NOK;
    }
    size=fread(&(cdl_curvature_wacc[0]),
               sizeof(double),
               CDL_MAX_CURVATURE,
               file);
    if (size != CDL_MAX_CURVATURE) {
      fclose(file);
      return CDL_NOK;
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------
int cdl_save_contour_ascii(char *filename)
{
  FILE *file;
  int  i;

  file = fopen(filename,"w");
  if (file!=NULL) {
    fprintf(file,"Format : SFB527 (300)\n");
    fprintf(file,"Content : CDL CONTOUR\n");
    fprintf(file,"Comment : ---\n");
    fprintf(file,"NumberOfSegments : %d\n",polygon.number_of_segments);

    for (i=0;i<polygon.number_of_segments;i++) {
      fprintf(file,"%f %f %f %f\n",polygon.line[i].x1,
                                   polygon.line[i].y1,
                                   polygon.line[i].x2,
                                   polygon.line[i].y2);
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

int cdl_load_contour_ascii(char *filename)
{
  FILE   *file;
  int    num,i,format;

  file=fopen(filename,"r");
  if (file!=NULL) {
    // read first line
    num=fscanf(file,"Format : SFB527 (%d)\n",&format);
    if (num!=1) {
      fprintf(stderr,"Error in line 1: unknown file format\n");
      fclose(file);
      return CDL_NOK;
    }

    // read second line
    fscanf(file,"Content : CDL CONTOUR\n");

    // read third line
    fscanf(file,"Comment : ---\n");

    // read fourth line
    num=fscanf(file,"NumberOfSegments : %d\n",&polygon.number_of_segments);
    if (num!=1) {
      fprintf(stderr,"Error in line 4: Wrong format or wrong value\n");
      fclose(file);
      return CDL_NOK;
    }

    // read data area
    num=0;
    for (i=0;i<polygon.number_of_segments;i++) {
      num+=fscanf(file,"%lf %lf %lf %lf\n",&(polygon.line[i].x1),
                                           &(polygon.line[i].y1),
                                           &(polygon.line[i].x2),
                                           &(polygon.line[i].y2));
    }
    if (num!=(4*polygon.number_of_segments)) {
      fclose(file);
      return CDL_NOK;
    }
    fclose(file);
    return CDL_OK;
  }
  return CDL_NOK;
}

// --------------------------------------------------------------
//
//
//
// --------------------------------------------------------------




// --------------------------------------------------------------
//
//
//
// --------------------------------------------------------------
int main(void)
{
  int status;

  // -------------------------------------------------------
  //      A ____ B                 ^ x
  //       /    \                  |
  //    H /      \ C               |
  //     |        |     y <--------+--
  //     |        |
  //    G \      / D
  //       \____/
  //       F     E
  // -------------------------------------------------------

  // <asteck date="18.07.2008">

  polygon.number_of_segments = 8;

// exact robot shape P3DX
/*
  polygon.line[0].x1 =  170.0;polygon.line[0].y1 =   75.0;  // AB
  polygon.line[0].x2 =  170.0;polygon.line[0].y2 =  -75.0;

  polygon.line[1].x1 =  170.0;polygon.line[1].y1 =  -75.0;  // BC
  polygon.line[1].x2 =   60.0;polygon.line[1].y2 = -190.0;

  polygon.line[2].x1 =   60.0;polygon.line[2].y1 = -190.0;  // CD
  polygon.line[2].x2 =  -90.0;polygon.line[2].y2 = -190.0;

  polygon.line[3].x1 =  -90.0;polygon.line[3].y1 = -190.0;  // DE
  polygon.line[3].x2 = -220.0;polygon.line[3].y2 = -165.0;

  polygon.line[4].x1 =  -220.0;polygon.line[4].y1 = -165.0;  // EF
  polygon.line[4].x2 =  -260.0;polygon.line[4].y2 =  -60.0;

  polygon.line[5].x1 =  -260.0;polygon.line[5].y1 = -60.0;  // FG
  polygon.line[5].x2 =  -260.0;polygon.line[5].y2 =  60.0;

  polygon.line[6].x1 =  -260.0;polygon.line[6].y1 =  60.0;  // GH
  polygon.line[6].x2 =  -220.0;polygon.line[6].y2 = 165.0;

  polygon.line[7].x1 =  -220.0;polygon.line[7].y1 = 165.0;  // HI
  polygon.line[7].x2 =   -90.0;polygon.line[7].y2 = 190.0;

  polygon.line[8].x1 =   -90.0;polygon.line[8].y1 = 190.0;  // IJ
  polygon.line[8].x2 =    60.0;polygon.line[8].y2 = 190.0;

  polygon.line[9].x1 =    60.0;polygon.line[9].y1 = 190.0;  // JA
  polygon.line[9].x2 =   170.0;polygon.line[9].y2 =  75.0;
*/

// robot shape P3DX with some safety clearance
/*
  polygon.line[0].x1 =  180.0;polygon.line[0].y1 =   85.0;  // AB
  polygon.line[0].x2 =  180.0;polygon.line[0].y2 =  -85.0;

  polygon.line[1].x1 =  180.0;polygon.line[1].y1 =  -85.0;  // BC
  polygon.line[1].x2 =   60.0;polygon.line[1].y2 = -200.0;

  polygon.line[2].x1 =   60.0;polygon.line[2].y1 = -200.0;  // CD
  polygon.line[2].x2 =  -90.0;polygon.line[2].y2 = -200.0;

  polygon.line[3].x1 =  -90.0;polygon.line[3].y1 = -200.0;  // DE
  polygon.line[3].x2 = -230.0;polygon.line[3].y2 = -175.0;

  polygon.line[4].x1 =  -230.0;polygon.line[4].y1 = -175.0;  // EF
  polygon.line[4].x2 =  -270.0;polygon.line[4].y2 =  -70.0;

  polygon.line[5].x1 =  -270.0;polygon.line[5].y1 = -70.0;  // FG
  polygon.line[5].x2 =  -270.0;polygon.line[5].y2 =  70.0;

  polygon.line[6].x1 =  -270.0;polygon.line[6].y1 =  70.0;  // GH
  polygon.line[6].x2 =  -230.0;polygon.line[6].y2 = 175.0;

  polygon.line[7].x1 =  -230.0;polygon.line[7].y1 = 175.0;  // HI
  polygon.line[7].x2 =   -90.0;polygon.line[7].y2 = 200.0;

  polygon.line[8].x1 =   -90.0;polygon.line[8].y1 = 200.0;  // IJ
  polygon.line[8].x2 =    70.0;polygon.line[8].y2 = 200.0;

  polygon.line[9].x1 =    70.0;polygon.line[9].y1 = 200.0;  // JA
  polygon.line[9].x2 =   180.0;polygon.line[9].y2 =  85.0;
*/

// robot shape P3DX --- robocup setup
/*
  polygon.line[0].x1 =  220.0;polygon.line[0].y1 =  140.0;  // AB
  polygon.line[0].x2 =  220.0;polygon.line[0].y2 = -140.0;

  polygon.line[1].x1 =  220.0;polygon.line[1].y1 = -140.0;  // BC
  polygon.line[1].x2 =   60.0;polygon.line[1].y2 = -300.0;

  polygon.line[2].x1 =   60.0;polygon.line[2].y1 = -300.0;  // CD
  polygon.line[2].x2 = -180.0;polygon.line[2].y2 = -300.0;

  polygon.line[3].x1 = -180.0;polygon.line[3].y1 = -300.0;  // DE
  polygon.line[3].x2 = -310.0;polygon.line[3].y2 = -140.0;

  polygon.line[4].x1 =  -310.0;polygon.line[4].y1 = -140.0;  // EF
  polygon.line[4].x2 =  -310.0;polygon.line[4].y2 =  140.0;

  polygon.line[5].x1 =  -310.0;polygon.line[5].y1 = 140.0;  // FG
  polygon.line[5].x2 =  -180.0;polygon.line[5].y2 = 300.0;

  polygon.line[6].x1 =  -180.0;polygon.line[6].y1 = 300.0;  // GH
  polygon.line[6].x2 =    60.0;polygon.line[6].y2 = 300.0;

  polygon.line[6].x1 =    60.0;polygon.line[6].y1 = 300.0;  // HA
  polygon.line[6].x2 =   220.0;polygon.line[6].y2 = 140.0;
*/

// robot shape P3DX with some safety clearance and Forklift
/*
  polygon.line[0].x1 =  450.0;polygon.line[0].y1 =  100.0;  // AB
  polygon.line[0].x2 =  450.0;polygon.line[0].y2 = -100.0;

  polygon.line[1].x1 =  450.0;polygon.line[1].y1 = -100.0;  // BC
  polygon.line[1].x2 =   60.0;polygon.line[1].y2 = -200.0;

  polygon.line[2].x1 =   60.0;polygon.line[2].y1 = -200.0;  // CD
  polygon.line[2].x2 =  -90.0;polygon.line[2].y2 = -200.0;

  polygon.line[3].x1 =  -90.0;polygon.line[3].y1 = -200.0;  // DE
  polygon.line[3].x2 = -230.0;polygon.line[3].y2 = -175.0;

  polygon.line[4].x1 =  -230.0;polygon.line[4].y1 = -175.0;  // EF
  polygon.line[4].x2 =  -270.0;polygon.line[4].y2 =  -70.0;

  polygon.line[5].x1 =  -270.0;polygon.line[5].y1 = -70.0;  // FG
  polygon.line[5].x2 =  -270.0;polygon.line[5].y2 =  70.0;

  polygon.line[6].x1 =  -270.0;polygon.line[6].y1 =  70.0;  // GH
  polygon.line[6].x2 =  -230.0;polygon.line[6].y2 = 175.0;

  polygon.line[7].x1 =  -230.0;polygon.line[7].y1 = 175.0;  // HI
  polygon.line[7].x2 =   -90.0;polygon.line[7].y2 = 200.0;

  polygon.line[8].x1 =   -90.0;polygon.line[8].y1 = 200.0;  // IJ
  polygon.line[8].x2 =    60.0;polygon.line[8].y2 = 200.0;

  polygon.line[9].x1 =    60.0;polygon.line[9].y1 = 200.0;  // JA
  polygon.line[9].x2 =   450.0;polygon.line[9].y2 = 100.0;
*/


// exact robot shape RMP50
  polygon.line[0].x1 =  327.5;polygon.line[0].y1 =  143.0;  // AB
  polygon.line[0].x2 =  327.5;polygon.line[0].y2 = -143.0;

  polygon.line[1].x1 =  327.5;polygon.line[1].y1 = -143.0;  // BC
  polygon.line[1].x2 =  205.0;polygon.line[1].y2 = -281.0;

  polygon.line[2].x1 =  205.0;polygon.line[2].y1 = -281.0;  // CD
  polygon.line[2].x2 = -205.0;polygon.line[2].y2 = -281.0;

  polygon.line[3].x1 = -205.0;polygon.line[3].y1 = -281.0;  // DE
  polygon.line[3].x2 = -327.5;polygon.line[3].y2 = -143.0;

  polygon.line[4].x1 = -327.5;polygon.line[4].y1 = -143.0;  // EF
  polygon.line[4].x2 = -327.5;polygon.line[4].y2 =  143.0;

  polygon.line[5].x1 = -327.5;polygon.line[5].y1 =  143.0;  // FG
  polygon.line[5].x2 = -205.0;polygon.line[5].y2 =  281.0;

  polygon.line[6].x1 = -205.0;polygon.line[6].y1 =  281.0;  // GH
  polygon.line[6].x2 =  205.0;polygon.line[6].y2 =  281.0;

  polygon.line[6].x1 =  205.0;polygon.line[6].y1 =  281.0;  // HA
  polygon.line[6].x2 =  327.5;polygon.line[6].y2 =  143.0;

// </asteck>

  if (CDL_TRA_CELLS > CDL_MAX_TRA_CELLS) {
    fprintf(stderr,"CDL_TRA_CELLS\n");
    exit(0);
  }
  if (CDL_ROT_CELLS > CDL_MAX_ROT_CELLS) {
    fprintf(stderr,"CDL_ROT_CELLS\n");
    exit(0);
  }
  if (CDL_C_X_CELLS > CDL_MAX_X_CELLS) {
    fprintf(stderr,"CDL_C_X_CELLS\n");
    exit(0);
  }
  if (CDL_C_Y_CELLS > CDL_MAX_Y_CELLS) {
    fprintf(stderr,"CDL_C_Y_CELLS\n");
    exit(0);
  }
  if (CDL_CURVATURE_INDICES > CDL_MAX_CURVATURE) {
    fprintf(stderr,"CDL_CURVATURE\n");
    exit(0);
  }

  printf("----------------------------------------------------------------\n");
  printf("\n");
  printf("Berechnung der CDL-Lookup-Tabelle\n");
  printf("\n");
  printf("----------------------------------------------------------------\n");
  printf("\n");
  printf("Starten der Berechung mit folgenden Parametern:\n");
  printf("\n");
  printf("Parameter Geschwindigkeitsraum\n");
  printf("  maximale Translationsgeschwindigkeit [mm/s]  %f\n",CDL_V_TRA_MAX);
  printf("  minimale Translationsgeschwindigkeit [mm/s]  %f\n",CDL_V_TRA_MIN);
  printf("  maximale Rotationsgeschwindigkeit    [deg/s] %f\n",CDL_V_ROT_MAX);
  printf("  minimale Rotationsgeschwindigkeit    [deg/s] %f\n",CDL_V_ROT_MIN);
  printf("  Aufloesung Translation               [mm/s]  %f\n",CDL_V_TRA_STEP);
  printf("  Aufloesung Rotation                  [deg/s] %f\n",CDL_V_ROT_STEP);
  printf("  Anzahl Zellen Translation                    %f\n",CDL_TRA_CELLS);
  printf("  Anzahl Zellen Rotation                       %f\n",CDL_ROT_CELLS);
  printf("\n");
  printf("Parameter kartesischer Raum\n");
  printf("  kleinste x-Koordinate                [mm]    %f\n",CDL_C_X_MIN);
  printf("  groesste x-Koordinate                [mm]    %f\n",CDL_C_X_MAX);
  printf("  kleinste y-Koordinate                [mm]    %f\n",CDL_C_Y_MIN);
  printf("  groesste y-Koordinate                [mm]    %f\n",CDL_C_Y_MAX);
  printf("  Zellgroesse                          [mm]    %f\n",CDL_C_RES);
  printf("  Anzahl Zellen x                              %f\n",CDL_C_X_CELLS);
  printf("  Anzahl Zellen y                              %f\n",CDL_C_Y_CELLS);
  printf("\n");
  printf("Parameter Curvature\n");
  printf("  Anzahl Curvature Werte im Index              %f\n",CDL_CURVATURE_INDICES);
  printf("\n");
  printf("Groesse der Lookup-Tabelle                     %f\n",
              CDL_C_X_CELLS*CDL_C_Y_CELLS*CDL_CURVATURE_INDICES);

  // ---------------------------------------------------------------
  // Do the lookup table calculation
  // ---------------------------------------------------------------
  status = cdl_calculate_lookup_index();
  status = cdl_calculate_lookup_table(&polygon);
//  status = cdl_lower_resolution(10.0,0.5);

  printf("Berechnung beendet.\n");
  printf("Ergebnis abspeichern ...\n");

  status = cdl_save_dist_angle_lookup_bin("CDLdist.dat");
  if (status!=CDL_OK) {
    printf("Error saving distance/angle lookup table\n");
  }
  status = cdl_save_acc_lookup_bin("CDLacc.dat");
  if (status!=CDL_OK) {
    printf("Error saving acceleration lookup table\n");
  }
  status = cdl_save_curvature_index_ascii("CDLindex.dat");
  if (status!=CDL_OK) {
    printf("Error saving curvature index lookup table\n");
  }
  status = cdl_save_contour_ascii("CDLcontour.dat");
  if (status!=CDL_OK) {
    printf("Error saving contour file\n");
  }
  printf("Ergebnis abgespeichert.\n");

  // ---------------------------------------------------------------
  // These are only test routines
  // ---------------------------------------------------------------

{
double cdl_v_rot_min,cdl_v_rot_step;
double v,w,x,y;
int vi,wi,xi,yi,ci;
double d,a,vacc,wacc;

while(1) {
  printf("Ueberpruefung der Indizes in der curvature-Tabelle\n");
  printf("Eingabe v           [mm/s]  ");fflush(stdout);scanf("%lf",&v);
  printf("Eingabe w           [deg/s] ");fflush(stdout);scanf("%lf",&w);
  printf("Eingabe Hindernis x [mm]    ");fflush(stdout);scanf("%lf",&x);
  printf("Eingabe Hindernis y [mm]    ");fflush(stdout);scanf("%lf",&y);
  
  cdl_v_rot_min=rad(CDL_V_ROT_MIN);
  cdl_v_rot_step=rad(CDL_V_ROT_STEP);
  w=rad(w);

  vi=(int)(floor((v-CDL_V_TRA_MIN)/CDL_V_TRA_STEP+0.5));
  wi=(int)(floor((w-cdl_v_rot_min)/cdl_v_rot_step+0.5));
  xi=(int)(ceil((x-CDL_C_X_MIN)/CDL_C_RES));
  yi=(int)(ceil((y-CDL_C_Y_MIN)/CDL_C_RES));

  ci=cdl_index_v_w[vi][wi];
  d =cdl_dist_lookup[xi][yi][ci];
  a =(cdl_alph_lookup[xi][yi][ci]/1000.0*180.0/PI);
  vacc = cdl_curvature_vacc[ci];
  wacc = cdl_curvature_wacc[ci]*180.0/PI;

  printf("v w %f %f  Index vi,wi %d %d  Curvature %d\n",v,w,vi,wi,ci);
  printf("x y %f %f  Index xi,yi %d %d\n",x,y,xi,yi);
  printf("Distanz Winkel %f %f\n",d,a);
  printf("vacc wacc      %f %f\n",vacc,wacc);
  printf("\n");
}
}


  // ---------------------------------------------------------------
  // These are only test routines
  // ---------------------------------------------------------------
/*
  printf("Distanzfeld extrahieren ...\n");
printf("Testwert: x y ci 40 30 270 %d\n",cdl_dist_lookup[40][30][270]);

  while(1) {
    double v,w;

    printf("Eingabe von v  [mm/s]: ");fflush(stdout);scanf("%lf",&v);
    printf("            w [deg/s]: ");fflush(stdout);scanf("%lf",&w);
    status = cdl_extract_distance_values("dist.dat",v,w*PI/180.0);
  }
*/

  return 0;
}

