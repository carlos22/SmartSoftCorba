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

#include "fawXfig.hh"

// ----------------------------------------------------------
//
// ----------------------------------------------------------
int test()
{
  FawXfigClass drawing;
  std::string       filename("test.fig");
 
  int x[] = {20000,25000,22000};
  int y[] = {30000,31000,32000};
 
  drawing.open(filename);
 
  // points
  drawing.drawPoint(43000,15000,BLACK,FAW_XFIG_POINT);
  drawing.drawPoint(43000,16000,RED,FAW_XFIG_CROSS);
 
  // line
  drawing.drawLine(40000,15000,40000,12000,DASH_LINE,1,BLUE);
 
  // polyline
  drawing.drawPolygon(x,y,3,UNFILLED,SOLID_LINE,2,YELLOW);
 
  // circle
  drawing.drawCircle(35000,35000,1000,DOTTED_LINE,1,YELLOW);
 
  // ellipse
  drawing.drawEllipse(30000,30000,2000,1000,3.14159265/4,DASH_DOT_LINE,1,GREEN);
 
  drawing.close();                     

  return 0;
}


int main()
{
  test();
}

