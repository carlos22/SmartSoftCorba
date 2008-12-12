//--------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
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

#ifndef SMART_DEFAULT_PAINT_METHODS_HH
#define SMART_DEFAULT_PAINT_METHODS_HH

#include <sstream>

#include "fawPainter.hh"

#include "commMobileLaserScan.hh"
#include "commBasePosition.hh"
#include "commLaserTrackingData.hh"

#include "smartSonarDataPaintMethod.hh"
#include "smartBasePositionPaintMethod.hh"
#include "smartBaseStatePaintMethod.hh"

namespace Faw {

/**
  Default paint method for laser scans.
  Draws each scan point with a cross.
 */
template<>
struct DefaultPaintMethod<Smart::CommMobileLaserScan>
{
  inline void operator()(const Smart::CommMobileLaserScan &scan, Painter &painter, bool do_highlight) const
  {
    if(scan.is_scan_valid())
    {
      painter.begin();
      if(do_highlight) painter.set_pen_width(2*painter.get_pen_width());
      painter.set_length_unit(1.0);
      painter.draw_crosses(scan.cartesian_world_begin(1.0), scan.cartesian_world_end());
      painter.end();
    }
  }
  inline void operator()(const Smart::CommMobileLaserScan &scan, Painter &painter) const
  {
    this->operator()(scan,painter,false);
  }
};

template<>
struct DefaultPaintMethod<Smart::CommLaserTrackingData>
{
  inline void operator()(const Smart::CommLaserTrackingData &td, Painter &painter, bool do_highlight) const
  {
    painter.begin();
    if(do_highlight) painter.set_pen_width(2*painter.get_pen_width());

    painter.set_length_unit(1.0);

    Smart::CommMobileLaserScan scan = td.get_laser_scan();
    if(scan.is_scan_valid())
    {
      Smart::CommMobileLaserScan::const_cartesian_world_iterator iter = scan.cartesian_world_begin(1.0);
      painter.draw_pixels(iter, scan.cartesian_world_end());

      const unsigned int n = td.get_num_objects();
      for(unsigned int i=0; i<n; ++i)
      {
        const unsigned int begin = td.get_object_first_point(i);
        const unsigned int size = td.get_object_last_point(i) - begin + 1;
        std::ostringstream oss;
        oss << td.get_object_id(i);
        std::string msg = oss.str();
        const double mx = (iter[begin].x + iter[begin + size - 1].x)/2;
        const double my = (iter[begin].y + iter[begin + size - 1].y)/2;

        painter.begin();
        painter.set_pen_width(2);
        painter.draw_chain(iter + begin, iter + begin + size);
        painter.draw_text(mx,my,msg.c_str());
        painter.end();
      }
    }

    
    painter.end();
  }
  inline void operator()(const Smart::CommLaserTrackingData &td, Painter &painter) const
  {
    this->operator()(td,painter,false);
  }
};

} // namespace Faw

#endif
