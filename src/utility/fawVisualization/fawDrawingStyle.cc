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
 
#include "fawDrawingStyle.hh"

const Faw::DrawingStyle::Color Faw::DrawingStyle::COLORS[Faw::DrawingStyle::NUM_COLORS] =
{
  0x000000, 
  0x800000, 0x008000, 0x000080, 0x008080, 0x800080, 0x808000,
  0xff0000, 0x00ff00, 0x0000ff, 0x00ffff, 0xff00ff, 0xffff00,
  0xff8080, 0x80ff80, 0x8080ff, 0x80ffff, 0xff80ff, 0xffff80,
  0xffffff
};

