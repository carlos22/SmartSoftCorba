// --------------------------------------------------------------------------
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
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------

#include <cmath>
#include <iostream>
#include <vector>
#include <iterator>

#include "commMobileLaserScan.hh"

Smart::CommTimeStamp make_ts();
Smart::CommBasePosition make_pos();
Smart::CommBaseVelocity make_vel();
Smart::CommBaseState make_bs();

/*
  Some point struct that provides the operator[](unsigned) 
  to access the coordinates.
 */
struct Point
{
  inline double &operator[](const unsigned int i) { return ((double*)this)[i]; }
  inline const double &operator[](const unsigned int i) const { return ((double*)this)[i]; }
  double x,y;
};

inline std::ostream &operator<<(std::ostream &os, const Point &p)
{
  return os << "cart(" << p.x << "," << p.y << ")";
}

struct PolarPoint
{
  double distance;
  double angle;
};

inline std::ostream &operator<<(std::ostream &os, const PolarPoint &pp)
{
  return os << "polar(a=" << pp.angle << ",d=" << pp.distance << ")";
}


int main()
{
  Smart::CommMobileLaserScan scan;
  scan.set_scan_update_count(42);
  scan.set_base_state(make_bs());
  scan.set_scanner_x(1000.0);
  scan.set_scanner_y(1000.0);
  scan.set_scanner_z(3.0);
  scan.set_scanner_azimuth(M_PI/4);
  scan.set_scan_length_unit(1);
  scan.set_scan_integer_field_of_view(-9000,4500);

  scan.set_scan_size(5);
  for(unsigned int i=0; i<scan.get_scan_size(); ++i) 
  {
    // set distances, here we are using [100mm] units
    scan.set_scan_index(i,i);
    scan.set_scan_distance(i, 1, 0.1);
    scan.set_scan_intensity(i, i%8);
  }
  scan.set_scan_valid(true);

  std::cout << "scan  = " << scan << std::endl;
  
  CORBA::Any a;
  scan.get(a);
  
  Smart::CommMobileLaserScan scan2;
  scan2.set(a);
  
  std::cout << "scan2 = " << scan2 << std::endl;

  std::cout << "ITERATOR TEST" << std::endl;

  std::cout << std::endl << std::endl;
  std::copy(scan2.polar_begin(1.0), scan2.polar_end(), std::ostream_iterator<Smart::CommMobileLaserScan::polar_point_type>(std::cout, " "));
  std::cout << std::endl << std::endl;
  std::copy(scan2.cartesian_scanner_begin(1.0), scan2.cartesian_scanner_end(), std::ostream_iterator<Smart::CommMobileLaserScan::cartesian_scanner_point_type>(std::cout, " "));
  std::cout << std::endl << std::endl;
  std::copy(scan2.cartesian_world_begin(1.0), scan2.cartesian_world_end(), std::ostream_iterator<Smart::CommMobileLaserScan::cartesian_world_point_type>(std::cout, " "));
  std::cout << std::endl << std::endl;
  
  std::cout << "OUTPUT ITERATOR TEST" << std::endl;

  std::vector<PolarPoint> polar_vect;
  scan2.get_scan_polar(std::back_inserter(polar_vect));
  std::copy(polar_vect.begin(), polar_vect.end(), std::ostream_iterator<PolarPoint>(std::cout, " "));
  std::cout << std::endl << std::endl;

  std::vector<Point> cart_vect;
  scan2.get_scan_cartesian_scanner(std::back_inserter(cart_vect));
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;

  cart_vect.clear();
  scan2.get_scan_cartesian_world(std::back_inserter(cart_vect));
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;

  std::cout << "RANDOM ACCESS ITERATOR TEST" << std::endl;

  std::cout << "container-too-small test:" << std::endl;

  polar_vect.resize(scan2.get_scan_size() / 2);
  std::cout << scan2.get_scan_polar(polar_vect.begin(), polar_vect.end()) << std::endl;
  std::copy(polar_vect.begin(), polar_vect.end(), std::ostream_iterator<PolarPoint>(std::cout, " "));
  std::cout << std::endl << std::endl;

  cart_vect.resize(scan2.get_scan_size() / 2);
  std::cout << scan2.get_scan_cartesian_scanner(cart_vect.begin(), cart_vect.end()) << std::endl;
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;

  cart_vect.resize(scan2.get_scan_size() / 2);
  std::cout << scan2.get_scan_cartesian_world(cart_vect.begin(), cart_vect.end()) << std::endl;
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;

  std::cout << "correct size test:" << std::endl;

  polar_vect.resize(scan2.get_scan_size());
  std::cout << scan2.get_scan_polar(polar_vect.begin(), polar_vect.end()) << std::endl;
  std::copy(polar_vect.begin(), polar_vect.end(), std::ostream_iterator<PolarPoint>(std::cout, " "));
  std::cout << std::endl << std::endl;

  cart_vect.resize(scan2.get_scan_size());
  std::cout << scan2.get_scan_cartesian_scanner(cart_vect.begin(), cart_vect.end()) << std::endl;
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;

  cart_vect.resize(scan2.get_scan_size());
  std::cout << scan2.get_scan_cartesian_world(cart_vect.begin(), cart_vect.end()) << std::endl;
  std::copy(cart_vect.begin(), cart_vect.end(), std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl << std::endl;


/*
  std::vector<double> distances2(scan2.get_scan_size());

  // get the distances, we are now using [m]
  scan2.get_scan_distances(distances2.begin(), distances2.end(), 1.0);
  std::cout << "Distances [m] = ";
  for(unsigned int i=0; i<distances2.size(); ++i)
  {
    std::cout << distances2[i] << " ";
  }
  std::cout << std::endl;

  // get the distances, we are now using [inches]
  scan2.get_scan_distances(distances2.begin(), distances2.end(), 0.0254);
  std::cout << "Distances [inch] = ";
  for(unsigned int i=0; i<distances2.size(); ++i)
  {
    std::cout << distances2[i] << " ";
  }
  std::cout << std::endl;
  
  // get cartesian points in world coordinates and [mm] units
  std::vector<Point> points(scan2.get_scan_size());
  scan2.get_scan_points_cartesian(points.begin(), points.end());
  std::cout << "Points [mm] = " << std::endl;
  for(unsigned int i=0; i<points.size(); ++i)
  {
    std::cout << "\(" << points[i][0] << "," << points[i][1] << ")" << std::endl;
  }
*/
  return 0;
}

Smart::CommTimeStamp make_ts()
{
  return Smart::CommTimeStamp::now();
}

Smart::CommBasePosition make_pos()
{
  Smart::CommBasePosition pos;
  
  pos.set_x(1000);
  pos.set_y(0);
  pos.set_z(-1000);

  pos.set_base_alpha(M_PI/4);
  pos.set_steer_alpha(M_PI/4);
  pos.set_turret_alpha(M_PI/4);

  pos.set_update_count(42);
  
  for(unsigned int i=0; i<3; ++i)
  for(unsigned int j=0; j<3; ++j)
  {
    pos.set_cov(i, j, 0.1*(4-(i-j)*(i-j)));
  }

  return pos;
}

Smart::CommBaseVelocity make_vel()
{
  Smart::CommBaseVelocity vel;
  vel.set_v(0);
  vel.set_omega_base(0);
  vel.set_omega_steer(0);
  vel.set_omega_turret(0);
  return vel;
}

Smart::CommBaseState make_bs()
{
  Smart::CommBaseState bs;
  bs.set_time_stamp(make_ts());
  bs.set_base_position(make_pos());
  bs.set_base_velocity(make_vel());
  return bs;
}

