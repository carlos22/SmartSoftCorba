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

#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "fawFigPainter.hh"

inline double width_of_ellipse(const double a, const double b, const double alpha)
{
  const double t = atan2(-b*sin(alpha),a*cos(alpha));
  const double s = sin(t);
  const double c = cos(t);
  const double as = a*s;
  const double ac = a*c;
  const double bs = b*s;
  const double bc = b*c;
  const double f = (ac*bc + as*bs) / (as*as + bc*bc);
  const double d1 = bc*f;
  const double d2 = as*f;
  return sqrt(d1*d1 + d2*d2);
}

inline double height_of_ellipse(const double a, const double b, const double alpha)
{
  return width_of_ellipse(b,a,alpha);
}

int main()
{
  const double a = 2;
  const double b = 1;

  const unsigned int steps = 24;
  for(unsigned int i=0; i<=steps; ++i)
  {
    const double alpha = i*M_PI/(2*steps);

    const double w = width_of_ellipse(a,b,alpha);
    const double h = height_of_ellipse(a,b,alpha);

    Faw::FigPainter fp;
    fp.draw_ellipse(0,0,a,b,alpha);
    fp.draw_box(-w,-h,w,h);

    std::ostringstream oss;
    oss << "drawing-" << std::setw(2) << std::setfill('0') << i << ".eps";
    fp.save(oss.str(),"eps");
  }
  return 0;
}
