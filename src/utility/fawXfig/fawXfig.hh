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
//-------------------------------------------------------------------------

#ifndef _FAWXFIG_HH
#define _FAWXFIG_HH

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "fawFigObject.h"


// = DESCRIPTION
//     This class is used to directly draw into xfig files
//     for visualisation.
//
// = AUTHOR
//     Christian Schlegel, SFB 527, C3, 06/06/00

class FawXfigClass
  // = TITLE
  //     Used to directly draw into xfig files for visualisation.
{
protected:
  // = Private data types and methods
  FILE *file;

  int xfig(int x);
  // this transformation is applied to any x coordinate

  int yfig(int y);
  // this transformation is applied to any y coordinate

public:
  // = Initialization and termination

  FawXfigClass();
  // Default constructor 

  virtual ~FawXfigClass();
  // Implicitly destroy the FawXfigClass.

  // = User interface functions
  // All methods return status information. If status is 0, everything
  // is OK, else an error occured.

  int open(std::string& filename);
  // Generate a new drawing environment

  int close();
  // Close current drawing environment

  int drawPoint(int x,int y,int color,int type);
  // description  Draw a point at (x,y) with the specified color and type
  // type         FAW_XFIG_POINT, FAW_XFIG_CROSS
  // color        BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GREEN4

  int drawLine(int x1,int y1,int x2,int y2,int style,int thickness,int color);
  // description  Draw a line from (x1,y1) to (x2,y2) with the specified parameters
  // style        SOLID_LINE, DASH_LINE, DOTTED_LINE,
  //              DASH_DOT_LINE, DASH_2_DOTS_LINE, DASH_3_DOTS_LINE,
  //              RUBBER_LINE, PANEL_LINE
  // thickness    [1 .. ]
  // color        BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GREEN4

  int drawPolygon(int x[],int y[],int n,int area_fill,int style,int thickness,int color);
  // description  Draw a closed polygon with n points with the specified parameters
  // area_fill    UNFILLED
  // style        SOLID_LINE, DASH_LINE, DOTTED_LINE,
  //              DASH_DOT_LINE, DASH_2_DOTS_LINE, DASH_3_DOTS_LINE,
  //              RUBBER_LINE, PANEL_LINE
  // thickness    [1 .. ]
  // color        BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GREEN4

  int drawCircle(int cx,int cy,int r,int style,int thickness,int color);
  // description  Draw a circle with center point (cx,cy), radius r and the other specified parameters
  // style        SOLID_LINE, DASH_LINE, DOTTED_LINE,
  //              DASH_DOT_LINE, DASH_2_DOTS_LINE, DASH_3_DOTS_LINE,
  //              RUBBER_LINE, PANEL_LINE
  // thickness    [1 .. ]
  // color        BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GREEN4

  int drawEllipse(int cx,int cy,int a,int b,double alpha,int style,int thickness,int color);
  // description  Draw an ellipse with center point (cx,cy) and width/height (a,b) 
  //              which is rotated by alpha [radiant]
  // style        SOLID_LINE, DASH_LINE, DOTTED_LINE, 
  //              DASH_DOT_LINE, DASH_2_DOTS_LINE, DASH_3_DOTS_LINE,
  //              RUBBER_LINE, PANEL_LINE
  // thickness    [1 .. ]
  // color        BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GREEN4
};

#endif

