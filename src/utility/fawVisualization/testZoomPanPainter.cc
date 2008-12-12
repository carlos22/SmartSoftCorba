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

#include "fawZoomPanPainter.hh"
#include "fawFigPainter.hh"

int main()
{
  const double polygon[4][2] = { {-1,-1}, {1,-1}, {1,1}, {-1,1} };

  Faw::BoundingBox source_box(-1,-1,1,1);

  Faw::FigPainter fp;
  for(unsigned int i=1; i<5; ++i)
  {
    Faw::ZoomPanPainter zpp(fp);
    Faw::BoundingBox target_box;
    target_box.assign(i,i).add(2*i,i).add(2*i,2*i).add(i,2*i);
    zpp.set_to_map(source_box, target_box);
    zpp.draw_polygon(polygon, polygon + 4);
  }
  fp.save("drawing.eps","eps");

  return 0;
}
