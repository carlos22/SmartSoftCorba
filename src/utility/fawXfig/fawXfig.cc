//--------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel
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

#include "fawXfig.hh"


// -----------------------------------------------------------
// default constructor
// -----------------------------------------------------------
FawXfigClass::FawXfigClass()
{
  file = NULL;
}

// -----------------------------------------------------------
// destructor
// -----------------------------------------------------------
FawXfigClass::~FawXfigClass()
{
  close();
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
int FawXfigClass::xfig(int x)
{
  // scale 1cm in xfig is 1m in world
  return (int)(x*0.45);
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
int FawXfigClass::yfig(int y)
{
  // scale 1cm in xfig is 1m in world
  return (int)(y*0.45);
}

// -----------------------------------------------------------
// Generate a new drawing environment
// -----------------------------------------------------------
int FawXfigClass::open(std::string& filename)
{
  if (file != NULL) {
    fclose(file);
  }
  file = fopen(filename.c_str(),"w");
  if (file==NULL) {
    // unable to open file
    return 1;
  } else {
    fprintf(file,"#FIG 3.2\n");
    fprintf(file,"Landscape\n");
    fprintf(file,"Center\n");
    fprintf(file,"Metric\n");
    fprintf(file,"A4\n");
    fprintf(file,"100.00\n");
    fprintf(file,"Single\n");
    fprintf(file,"-2\n");
    fprintf(file,"1200 2\n");
    fprintf(file,"#Created by Christian Schlegel, SmartSoft\n");

    return 0;
  } 
}

// -----------------------------------------------------------
// Close current drawing environment
// -----------------------------------------------------------
int FawXfigClass::close(void)
{
  if (file != NULL) {
    fclose(file);
    file = NULL;
  }

  return 0;
}

// -----------------------------------------------------------
// Draw a point 
// -----------------------------------------------------------
int FawXfigClass::drawPoint(int x,int y,int color,int type)
{
  int status=0;

  if (file!=NULL) {
    switch(type) {
      case FAW_XFIG_POINT:
        if (status==0) status = drawCircle(x,y,1,SOLID_LINE,1,color);
        break;
      case FAW_XFIG_CROSS:
        if (status==0) status = drawLine(x-1,y-1,x+1,y+1,SOLID_LINE,1,color);
        if (status==0) status = drawLine(x-1,y+1,x+1,y-1,SOLID_LINE,1,color);
        break;
      default:
        status=1;
        break;
    }
  } else {
    status=1;
  }
  return status;
}

// -----------------------------------------------------------
// Draw a line 
// -----------------------------------------------------------
int FawXfigClass::drawLine(int x1,int y1,int x2,int y2,int style,int thickness,int color)
{
  if (file!=NULL) {
    fprintf(file,"%d %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d\n",
      XFIG_O_POLYLINE, 
      T_POLYLINE,
      style, 
      thickness,
      color,                               // pen color
      DEFAULT,                             // fill color
      0,                                   // depth
      DEFAULT,                             // pen (unused)
      -1,                                  // not filled
      0.0,                                 // style_val
      JOIN_MITER,                          // join style
      CAP_BUTT,                            // cap style
      DEFAULT,                             // for arc-box only
      0, 0,                                // no arrows
      2);                                  // num points
    fprintf(file,"\t %d %d %d %d\n",
      xfig(x1), yfig(y1),                  // point 1
      xfig(x2), yfig(y2));                 // point 2

    return 0;
  } else {
    return 1;
  }
}


// -----------------------------------------------------------
// Draw a closed polygon 
// -----------------------------------------------------------
int FawXfigClass::drawPolygon(int x[],int y[],int n,int area_fill,int style,int thickness,int color)
{
  if (file!=NULL) {
    int i;

    fprintf(file, "%d %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d\n",
      XFIG_O_POLYLINE, 
      T_POLYLINE,
      style, 
      thickness,
      color,                               // pen color
      DEFAULT,                             // fill color
      0,                                   // depth
      DEFAULT,                             // pen (unused)
      area_fill,                           // area fill
      0.0,                                 // style_val
      JOIN_MITER,                          // join style
      CAP_BUTT,                            // cap style
      DEFAULT,                             // for arc-box only
      0, 
      0,                                   // no arrows
      n);                                  // num points
    fprintf(file,"\t");
    for (i=0; i<n; i++) fprintf(file, "%d %d ", xfig(x[i]), yfig(y[i]));
    fprintf(file, "\n");

    return 0;
  } else {
    return 1;
  }
}


// -----------------------------------------------------------
// Draw a circle 
// -----------------------------------------------------------
int FawXfigClass::drawCircle(int cx,int cy,int r,int style,int thickness,int color)
{
  return drawEllipse(cx, cy, r, r, 0.0, style, thickness, color);
}


// -----------------------------------------------------------
// Draw an ellipse 
// -----------------------------------------------------------
int FawXfigClass::drawEllipse(int cx,int cy,int a,int b,double alpha,int style,int thickness,int color)
{
  if (file!=NULL) {
    fprintf(file, "%d %d %d %d %d %d %d %d %d %f %d %f %d %d %d %d %d %d %d %d\n",
      XFIG_O_ELLIPSE, 
      T_ELLIPSE_BY_RAD,
      style, 
      thickness,
      color,                               // pen color
      DEFAULT,                             // fill color
      0,                                   // depth
      DEFAULT,                             // pen (unused)
      -1,                                  // not filled
      0.0,                                 // style_val
      1,                                   // direction
      alpha,                               // angle of x-axis
      xfig(cx),                            // center
      yfig(cy),
      a,                                   // radius
      b,
      xfig(cx),                            // 1st point
      yfig(cy),
      xfig(cx + a),                        // 2nd point
      yfig(cy + b));

    return 0;
  } else {
    return 1;
  }
}

