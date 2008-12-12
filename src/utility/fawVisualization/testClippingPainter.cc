//--------------------------------------------------------------------------
//
//  Copyright (C) 2002 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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

#include <fstream>
#include <string>

#include "fawClippingPainter.hh"
#include "fawFigPainter.hh"
#include "fawBoundingBoxPaintMethod.hh"

int main()
{
  const double points[5][2] = { {-1,-1}, {1,-1}, {-1,1}, {0,1}, {0,3} };

  Faw::BoundingBox clipping_box(-0.5, -2, 0.5, 2);

  Faw::FigPainter fp;

  fp.begin();
  fp.set_pen_color(0xcccccc);
  fp.draw_chain(points, points + 5);
  fp.end();

  Faw::ClippingPainter cp(fp,clipping_box);
  cp.begin();
  cp.draw_chain(points, points + 5);
  cp.draw_arrow(-1,1,0,0.5);
  cp.draw_arrow(0,-0.5,1,-1);
  cp.end();
  
  fp.begin();
  fp.set_pen_color(0xaaaaaa);
  fp.draw(clipping_box);
  fp.end();
  
  fp.save("drawing.eps","eps");

  return 0;
}
