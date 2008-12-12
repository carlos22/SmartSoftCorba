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

#ifndef SMART_SONAR_DATA_PAINT_METHOD_HH
#define SMART_SONAR_DATA_PAINT_METHOD_HH

#include "fawPainter.hh"
#include "commSonar.hh"

namespace Smart {

/**
  Paint method for sonar scans.
  There cannot be a default paint method, since any sonar paint method 
  object has to be parameterized by the sonar configuration.
 */
class SonarDataPaintMethod
{
private:
  Smart::CommSonarConfig _cfg;
  
public:
  SonarDataPaintMethod() {}
  SonarDataPaintMethod(const Smart::CommSonarConfig &cfg) : _cfg(cfg) {}

  void set_sonar_config(const Smart::CommSonarConfig &cfg) { _cfg = cfg; }

  inline void operator()(const Smart::CommSonarData &data, Faw::Painter &painter, bool do_highlight) const
  {
    painter.begin();
    if(do_highlight) painter.set_pen_width(2*painter.get_pen_width());

    painter.set_length_unit(1.0);

    const unsigned int n = data.get_num_measurements();
    const CommBaseState base_state = data.get_base_state();

    for(unsigned int i=0; i<n; ++i)
    {
      const unsigned int id = data.get_sensor_id(i);
      const CommBaseState bs = base_state.extrapolate(data.get_measurement_time_stamp(i));
      const CommBasePosition base_pos = bs.get_base_position();

      const double base_x = base_pos.get_x(1.0);
      const double base_y = base_pos.get_y(1.0);
      const double base_azimuth = base_pos.get_alpha(_cfg.get_mount_point(id));

      painter.set_transformation(base_x,base_y,base_azimuth,1.0);
      painter.begin();

      const CommPosition3d sonar_rel_pos = _cfg.get_position(id);
      const CommDirection3d sonar_rel_dir = _cfg.get_direction(id);
      const double sonar_rel_x = sonar_rel_pos.get_x(1.0);
      const double sonar_rel_y = sonar_rel_pos.get_y(1.0);
      const double sonar_rel_azimuth = sonar_rel_dir.get_azimuth();
      
      painter.set_transformation(sonar_rel_x,sonar_rel_y,sonar_rel_azimuth,1.0);

      if(data.is_measurement_valid(i))
      {
        const double dist = data.get_measured_distance(i, 1.0);
        const double hw = _cfg.get_half_width(id);
        painter.draw_arc(0,0,dist,-hw,+hw);
      }
      else
      {
        painter.draw_circle(0,0,0.05);
      }
      
      painter.end();
    }

    painter.end();
  }
  inline void operator()(const Smart::CommSonarData &scan, Faw::Painter &painter) const
  {
    this->operator()(scan,painter,false);
  }
};

} // namespace Smart

#endif
