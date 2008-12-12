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

#ifndef SMART_COMM_MOBILE_LASER_SCAN_HH
#define SMART_COMM_MOBILE_LASER_SCAN_HH

#include <cmath>
#include <string>
#include <iostream>

#include "commTimeStamp.hh"
#include "commBaseState.hh"

#include "smartMobileLaserScanC.hh"

namespace Smart {

/**
  A class representing a laser scan recorded by a laser scanner which is
  mounted horizontally on a mobile platform.
 */
class CommMobileLaserScan
{
protected:
  SmartIDL::MobileLaserScan _scan;

public:

  CommMobileLaserScan();
  CommMobileLaserScan(const SmartIDL::MobileLaserScan &scan);
  virtual ~CommMobileLaserScan();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommMobileLaserScan"; };

  /// \internal
  inline const SmartIDL::MobileLaserScan &get_idl() const { return _scan; }
  /// \internal
  inline void set_idl(const SmartIDL::MobileLaserScan &scan) { _scan = scan; }

  //
  // TYPES
  //

  /**
    Polar description of a scan point.
    Polar iterators of a mobile laser scans point to such descriptions.
   */
  struct polar_point_type
  {
    inline polar_point_type(double d = 0, double a = 0, unsigned char i = 0) : distance(d), angle(a), intensity(i) {}
    inline polar_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit = 0.001);
    /**
      The distance of the point.
      The length unit is determined by the method which created this point description.
     */
    double distance;
    /**
      The direction of the point in radians relative to the coordinate system of the scanner.
     */
    double angle;
    /**
      The intensity of the returned light.
      This feature is available only on some scanners.
     */
    unsigned char intensity;
  };

  /**
    Cartesian description of a scan point in the coordinate system of the scanner.
    Cartesian iterators for points in the scanner coordinate system point to such descriptions.
   */
  struct cartesian_scanner_point_type
  {
    inline cartesian_scanner_point_type(double px = 0, double py = 0, unsigned char i = 0) : x(px), y(py), intensity(i) {}
    inline cartesian_scanner_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit = 0.001);
    /**
      The x coordinate of the scan point in the coordinate system of the scanner.
      The length unit depends on the method which you used to get this description.
     */
    double x;
    /**
      The y coordinate of the scan point in the coordinate system of the scanner.
      The length unit depends on the method which you used to get this description.
     */
    double y;
    /**
      The intensity of the returned light.
      This feature is available only on some scanners.
     */
    unsigned char intensity;
    /**
      Subscript operator to access the coordinates of the point.
      Index 0 refers to the x component, index 1 refers to the y component.
     */
    inline double operator[](const unsigned int i) const { return ((double*)this)[i]; }
    /**
      Subscript operator to access the coordinates of the point.
      Index 0 refers to the x component, index 1 refers to the y component.
     */
    inline double &operator[](const unsigned int i) { return ((double*)this)[i]; }
  };

  /**
    Cartesian description of a scan point in the world coordinate system.
    Cartesian iterators for points in the world coordinate system point to such descriptions.
   */
  struct cartesian_world_point_type
  {
    inline cartesian_world_point_type(double px, double py, unsigned char i) : x(px), y(py), intensity(i) {}
    inline cartesian_world_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit = 0.001);
    /**
      The x coordinate of the scan point in the world coordinate system.
      The length unit depends on the method which you used to get this description.
     */
    double x;
    /**
      The y coordinate of the scan point in the world coordinate system.
      The length unit depends on the method which you used to get this description.
     */
    double y;
    /**
      The intensity of the returned light.
      This feature is available only on some scanners.
     */
    unsigned char intensity;
    /**
      Subscript operator to access the coordinates of the point.
      Index 0 refers to the x component, index 1 refers to the y component.
     */
    inline double operator[](const unsigned int i) const { return ((double*)this)[i]; }
    /**
      Subscript operator to access the coordinates of the point.
      Index 0 refers to the x component, index 1 refers to the y component.
     */
    inline double &operator[](const unsigned int i) { return ((double*)this)[i]; }
  };

  //
  // ITERATORS
  //
  
  /**
    Generic random access const iterator for STL like access to laser scans.
   */
  template<class Pnt>
  class const_point_iterator //: public std::iterator<std::random_access_iterator_tag,Pnt>
  {
  public:
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef Pnt value_type;
    typedef unsigned int difference_type;
    typedef Pnt* pointer;
    typedef Pnt& reference;
    inline const_point_iterator(const CommMobileLaserScan *scan = 0, unsigned int index = 0, const double unit = 0.001);
    inline bool operator==(const const_point_iterator<Pnt> &other) const;
    inline bool operator!=(const const_point_iterator<Pnt> &other) const;
    inline const_point_iterator<Pnt> operator++(int);
    inline const_point_iterator<Pnt> &operator++();
    inline const_point_iterator<Pnt> operator--(int);
    inline const_point_iterator<Pnt> &operator--();
    inline const_point_iterator<Pnt> operator+(int) const;
    inline const_point_iterator<Pnt> operator-(int) const;
    inline unsigned int operator-(const const_point_iterator<Pnt>&) const;
    inline const Pnt operator*() const;
    inline const Pnt operator[](int) const;
  private:
    const CommMobileLaserScan *_scan;
    unsigned int _index;
    double _unit;
  };
  /**
    Random access const iterator for retrieving polar points from this scan.
   */
  typedef const_point_iterator<polar_point_type> const_polar_iterator;
  /**
    Random access const iterator for retrieving cartesian points in the scanner coordinate system
    from this scan.
   */
  typedef const_point_iterator<cartesian_scanner_point_type> const_cartesian_scanner_iterator;
  /**
    Random access const iterator for retrieving cartesian points in the world coordinate system
    from this scan.
   */
  typedef const_point_iterator<cartesian_world_point_type> const_cartesian_world_iterator;

  //
  // GET
  //

  /**
    Return the state (timestamped configuration and velocities) of the robot
    associated to this laser scan.
   */
  inline CommBaseState get_base_state() const { return CommBaseState(_scan.base_state); }

  /**
    Get the global x position of the scanner at the moment when the scan was recorded.
   */
  inline double get_scanner_x(const double unit = 0.001) const { return _scan.scanner_x * 0.001 / unit; }

  /**
    Get the global y position of the scanner at the moment when the scan was recorded.
   */
  inline double get_scanner_y(const double unit = 0.001) const { return _scan.scanner_y * 0.001 / unit; }

  /**
    Get the global z position of the scanner at the moment when the scan was recorded.
   */
  inline double get_scanner_z(const double unit = 0.001) const { return _scan.scanner_z * 0.001 / unit; }

  /**
    Get the global orientation of the scanner at the moment when the scan was recorded.
   */
  inline double get_scanner_azimuth() const { return _scan.scanner_a; }

  /**
    Check if this scan is valid.
    Invalid scans may be sent if a scan had to be sent in a PushTimed pattern,
    but no valid scan was available.
   */
  inline bool is_scan_valid() const { return _scan.laser_scan.is_valid; }

  /**
    Returns the time stamp of the scan.
    With the current SICK interface implementation, this is the time when
    the header of the serial packet containing the scan was received.
   */
  inline CommTimeStamp get_scan_time_stamp() const { return CommTimeStamp(_scan.laser_scan.time); }

  /**
    Return the sequence count of this scan.
    The sequence count is increased by the laser server for each transmitted scan.
   */
  inline unsigned long get_scan_update_count() const { return _scan.laser_scan.update_count; }

  /**
    Return the number of points in this scan.
    A scan contains only valid points, therefore a 180 degree scan with 0.5 degree
    resolution may contain less that 361 points.
    Points are considered invalid, if the scanning device reports distances
    exceeding a maximum distance.
   */
  inline unsigned int get_scan_size() const { return _scan.laser_scan.scan_points.length(); }

  /**
    Return the internal length unit (the resolution) of this scan in millimeters.
    Distances are stored as integer values with respect to this length unit.
    Therefore, this length unit defines the distance resolution of the scan.
   */
  inline unsigned int get_scan_length_unit() const { return _scan.laser_scan.length_unit; }

  /**
    Return the start angle of the scan in radians relative to the scanning device.
   */
  inline double get_scan_start_angle() const { return _hd2r(_scan.laser_scan.start_angle); }

  /**
    Return the angular resolution (distance between two scan rays) of the scan in radians.
    The returned value is non-negative and smaller than 2*pi.
   */
  inline double get_scan_resolution() const { return _hd2r(_scan.laser_scan.resolution); }

  /**
    Return the angle of scan point number \a i in radians relative to the scanning device.
    The returned value is non-negative and smaller than 2*pi.
    Scan point counting starts with zero.
    The angle of point \i is \c start_angle+resolution*point[i].index
   */
  inline double get_scan_angle(unsigned int i) const 
  { 
    return _hd2r(_norm_angle(int(_scan.laser_scan.start_angle) + _scan.laser_scan.scan_points[i].index*int(_scan.laser_scan.resolution))); 
  }

  /**
    Return the distance of scan point \a i in units of \a unit meters.
   */
  inline double get_scan_distance(unsigned int i, const double unit = 0.001) const 
  { 
    return _scan.laser_scan.scan_points[i].distance * 0.001 * _scan.laser_scan.length_unit / unit; 
  }

  /**
    Return the scan ray index of scan point \a point_index.
    The scan ray index may be greater than the scan point index \c point_index 
    since there may have been invalid measurements.
   */
  inline unsigned int get_scan_ray_index(unsigned int point_index) const
  {
    return _scan.laser_scan.scan_points[point_index].index;
  }

  /**
    Get cartesian information about scan point number \a i in the coordinate system of the scanner.
    The information is returned in reference arguments \a x and \a y in units
    of \a unit meters (default is 0.001 meters).
   */
  inline void get_scan_cartesian_point_scanner(unsigned int i, double &x, double &y, const double unit = 0.001) const
  {
    const double a = get_scan_angle(i);
    const double d = get_scan_distance(i, unit);
    x = d * cos(a);
    y = d * sin(a);
  }

  /**
    Get cartesian information about scan point number \a i in the world coordinate system.
    The information is returned in reference arguments \a x and \a y in units
    of \a unit meters (default is 0.001 meters).
   */
  inline void get_scan_cartesian_point_world(unsigned int i, double &x, double &y, const double unit = 0.001) const
  {
    double sx,sy;
    get_scan_cartesian_point_scanner(i,sx,sy,unit);
    const double cos_a = cos(get_scanner_azimuth());
    const double sin_a = sin(get_scanner_azimuth());
    x = sx * cos_a - sy * sin_a + get_scanner_x(unit);
    y = sx * sin_a + sy * cos_a + get_scanner_y(unit);
  }

  /**
    Return the intensitiy of the scan point.
    The intensity of the reflected light is desribed by a value between 0 and 7.
    This feature is available on some scanners only.
   */
  inline unsigned char get_scan_intensity(unsigned int i) const
  {
    return _scan.laser_scan.scan_points[i].intensity;
  }

  /**
    Get a polar description of the scan into a container.
    The element type of the iterator is required to provide a public \c double member
    called \c distance, and a public \c double member called \c angle.
   */
  template<class Out>
  inline void get_scan_polar(Out iter, const double unit = 0.001) const;

  /**
    Get a polar description of the scan into a container.
    The element type of the iterator is required to provide a public \c double member
    called \c distance, and a public \c double member called \c angle.
    The forward iterators \a begin and \a end indicate the area in the container
    where the point should be written.
    The number of copied points is returned, which may be less than the size of the scan
    if there is not enough space between the iterators.
   */
  template<class For>
  inline unsigned int get_scan_polar(For begin, For end, const double unit = 0.001) const;

  /**
    Get a random access iterator to the begin of the polar description of the scan
    relative to the scanner.
    Iterators point to \c polar_point_type structs and are suitable for reading only.
   */
  inline const_polar_iterator polar_begin(const double unit = 0.001) const
  {
    return const_polar_iterator(this, 0, unit);
  }

  /**
    Get a random access iterator to the end of a polar description of the scan
    relative to the scanner.
    Iterators point to \c polar_point_type structs and are suitable for reading only.
   */
  inline const_polar_iterator polar_end(const double unit = 0.001) const
  {
    return const_polar_iterator(this, get_scan_size(), unit);
  }

  /**
    Retrieve the valid scan points as cartesian points in the scanner coordinate system.
    An output iterator to a point container is expected as the first argument.
    The point type is expected to provide the
    subscript operator \c operator[](unsigned int), where index 0 refers to
    the x-coordinate and index 1 refers to the y-coordinate.
    The \a unit argument can be used to set the lengt unit for the returnded points,
    the default is [mm].
   */
  template<class Out>
  inline void get_scan_cartesian_scanner(Out iter, const double unit = 0.001) const;

  /**
    Retrieve the valid scan points as cartesian points in the scanner coordinate system.
    An output iterator to a point container is expected as the first argument.
    The point type is expected to provide the
    subscript operator \c operator[](unsigned int), where index 0 refers to
    the x-coordinate and index 1 refers to the y-coordinate.
    The \a unit argument can be used to set the lengt unit for the returnded points,
    the default is [mm].
    The forward iterators \a begin and \a end indicate the area in the container
    where the point should be written.
    The number of copied points is returned, which may be less than the size of the scan
    if there is not enough space between the iterators.
   */
  template<class For>
  inline unsigned int get_scan_cartesian_scanner(For begin, For end, const double unit = 0.001) const;

  /**
    Get a random access iterator to the begin of the cartesian description of the scan
    in the coordinate system of the scanner.
    Iterators point to \c cartesian_scanner_point_type structs 
    and are suitable for reading only.
   */
  inline const_cartesian_scanner_iterator cartesian_scanner_begin(const double unit = 0.001) const
  {
    return const_cartesian_scanner_iterator(this, 0, unit);
  }

  /**
    Get a random access iterator to the end of the cartesian description of the scan
    in the coordinate system of the scanner.
    Iterators point to \c cartesian_scanner_point_type structs 
    and are suitable for reading only.
   */
  inline const_cartesian_scanner_iterator cartesian_scanner_end(const double unit = 0.001) const
  {
    return const_cartesian_scanner_iterator(this, get_scan_size(), unit);
  }

  /**
    Retrieve the valid scan points as cartesian points in the world coordinate system.
    An output iterator to a point container is expected as the first argument.
    The point type is expected to provide the
    subscript operator \c operator[](unsigned int), where index 0 refers to
    the x-coordinate and index 1 refers to the y-coordinate.
    The \a unit argument can be used to set the lengt unit for the returnded points,
    the default is [mm].
   */
  template<class Out>
  inline void get_scan_cartesian_world(Out iter, const double unit = 0.001) const;

  /**
    Retrieve the valid scan points as cartesian points in the world coordinate system.
    An output iterator to a point container is expected as the first argument.
    The point type is expected to provide the
    subscript operator \c operator[](unsigned int), where index 0 refers to
    the x-coordinate and index 1 refers to the y-coordinate.
    The \a unit argument can be used to set the lengt unit for the returnded points,
    the default is [mm].
    The forward iterators \a begin and \a end indicate the area in the container
    where the point should be written.
    The number of copied points is returned, which may be less than the size of the scan
    if there is not enough space between the iterators.
   */
  template<class For>
  inline unsigned int get_scan_cartesian_world(For begin, For end, const double unit = 0.001) const;

  /**
    Get a random access iterator to the begin of the cartesian description of the scan
    in the world coordinate system.
    Iterators point to \c cartesian_world_point_type structs 
    and are suitable for reading only.
   */
  inline const_cartesian_world_iterator cartesian_world_begin(const double unit = 0.001) const
  {
    return const_cartesian_world_iterator(this, 0, unit);
  }

  /**
    Get a random access iterator to the end of the cartesian description of the scan
    in the world coordinate system.
    Iterators point to \c cartesian_scanner_point_type structs 
    and are suitable for reading only.
   */
  inline const_cartesian_world_iterator cartesian_world_end(const double unit = 0.001) const
  {
    return const_cartesian_world_iterator(this, get_scan_size(), unit);
  }

  //
  // SET
  //

  /**
    Set the base state associated to this laser scan.
   */
  inline void set_base_state(const CommBaseState &bs) { _scan.base_state = bs.get_idl(); }

  /**
    Set the global x position of the scanner at the moment when the scan was recorded.
   */
  inline void set_scanner_x(double x, const double unit = 0.001) { _scan.scanner_x = x * unit * 1000; }

  /**
    Set the global y position of the scanner at the moment when the scan was recorded.
   */
  inline void set_scanner_y(double y, const double unit = 0.001) { _scan.scanner_y = y * unit * 1000; }

  /**
    Set the global z position of the scanner at the moment when the scan was recorded.
   */
  inline void set_scanner_z(double z, const double unit = 0.001) { _scan.scanner_z = z * unit * 1000; }

  /**
    Set the global orientation of the scanner at the moment when the scan was recorded.
   */
  inline void set_scanner_azimuth(double a) { _scan.scanner_a = a; }

  /**
    Set the valid flag of this scan.
   */
  inline void set_scan_valid(bool v) { _scan.laser_scan.is_valid = v; }
  
  /**
    Set the time stamp of this scan.
   */
  inline void set_scan_time_stamp(const CommTimeStamp &ts) { _scan.laser_scan.time = ts.get_idl(); }

  /**
    Set the sequence counter of this scan.
   */
  inline void set_scan_update_count(unsigned long count) { _scan.laser_scan.update_count = count; }

  /**
    Clears the scan points (invalidates them), sets the number of scan points to \a n.
   */
  inline void set_scan_size(unsigned int n);

  /**
    Set the internal length unit (the resolution) of the scan in millimeters.
    IMPORTANT: always set the length unit before setting any distances!
    Otherwise, the distances that have already been set will change their meaning.
   */
  inline void set_scan_length_unit(unsigned int unit) { _scan.laser_scan.length_unit = unit; }

  /**
    Set the angle of the first scan ray (relative to the scanner device) 
    and the angular resolution of the scan in 0.01 degree units.
   */
  inline void set_scan_integer_field_of_view(int start_angle, unsigned int resolution)
  {
    _scan.laser_scan.start_angle = _norm_angle(start_angle);
    _scan.laser_scan.resolution = resolution;
  }

  /**
    Set the angle of scan point \a i by setting its index to \a index.
    For $i<j$, $scan\_point[i].index<scan\_point[j].index$ is required.
   */
  inline void set_scan_index(unsigned int i, unsigned int index)
  {
    _scan.laser_scan.scan_points[i].index = index;
  }

  /**
    Set the distance of scan point \a index to \a dist * \a unit meters
    and set its valid flag to true.
   */
  inline void set_scan_distance(unsigned int i, double dist, const double unit = 0.001);

  /**
    Set a low level distance of the scan.
    The integer value is interpreted according to the length unit of the scan.
   */
  inline void set_scan_integer_distance(unsigned int i, unsigned int dist);

  /**
    Set the intensitiy of the scan point.
    The intensity of the reflected light is desribed by a value between 0 and 7.
   */
  inline void set_scan_intensity(unsigned int i, unsigned char intensity);

  /**
    Cut away scan points whose integer distance value exceeds the threshold.
    This can be used to cut away overflows.
   */
  void limit_scan_distance_integer(unsigned int max_dist);

  //
  // INPUT/OUTPUT
  //

  /**
    Print information about this scan to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;
  
  /**
    Save an XML like representation of this mobile laser scan to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);

private:

  inline int _norm_angle(const int hd) const 
  { 
    const int a = hd % 36000;
    if(a<0) return a + 36000;
    return a;
  }

  /**
    Convert hundredths of degrees to radians.
   */
  inline double _hd2r(int hd) const { return hd * 0.0001745329; }

  /**
    Convert radians to hundredths of degrees.
   */
  inline short int _r2hd(double r)  const { return int(::rint(r * 5729.578)); }
};

inline CommMobileLaserScan::polar_point_type::polar_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit)
{
  distance = scan->get_scan_distance(index, unit);
  angle = scan->get_scan_angle(index);
  intensity = scan->get_scan_intensity(index);
}

inline std::ostream &operator<<(std::ostream &os, const CommMobileLaserScan::polar_point_type::polar_point_type &pnt)
{
  return os << "polar_point(a=" << pnt.angle << ",d=" << pnt.distance << ",i=" << int(pnt.intensity) << ")";
}

inline CommMobileLaserScan::cartesian_scanner_point_type::cartesian_scanner_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit)
{
  scan->get_scan_cartesian_point_scanner(index, x, y, unit);
  intensity = scan->get_scan_intensity(index);
}

inline std::ostream &operator<<(std::ostream &os, const CommMobileLaserScan::cartesian_scanner_point_type::cartesian_scanner_point_type &pnt)
{
  return os << "cartesian_scanner_point((x,y)=(" << pnt.x << "," << pnt.y << "),i=" << int(pnt.intensity) << ")";
}

inline CommMobileLaserScan::cartesian_world_point_type::cartesian_world_point_type(const CommMobileLaserScan *scan, unsigned int index, const double unit)
{
  scan->get_scan_cartesian_point_world(index, x, y, unit);
  intensity = scan->get_scan_intensity(index);
}

inline std::ostream &operator<<(std::ostream &os, const CommMobileLaserScan::cartesian_world_point_type::cartesian_world_point_type &pnt)
{
  return os << "cartesian_world_point((x,y)=(" << pnt.x << "," << pnt.y << "),i=" << int(pnt.intensity) << ")";
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt>::const_point_iterator(const CommMobileLaserScan *scan, unsigned int index, const double unit)
: _scan(scan), _index(index), _unit(unit)
{
}

template<class Pnt>
inline bool CommMobileLaserScan::const_point_iterator<Pnt>::operator==(const CommMobileLaserScan::const_point_iterator<Pnt> &other) const
{
  return (_scan==other._scan) && (_index==other._index);
}

template<class Pnt>
inline bool CommMobileLaserScan::const_point_iterator<Pnt>::operator!=(const CommMobileLaserScan::const_point_iterator<Pnt> &other) const
{
  return !(*this==other);
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> CommMobileLaserScan::const_point_iterator<Pnt>::operator++(int)
{
  CommMobileLaserScan::const_point_iterator<Pnt> iter = *this;
  ++_index;
  return iter;
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> &CommMobileLaserScan::const_point_iterator<Pnt>::operator++()
{
  ++_index;
  return *this;
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> CommMobileLaserScan::const_point_iterator<Pnt>::operator--(int)
{
  CommMobileLaserScan::const_point_iterator<Pnt> iter = *this;
  --_index;
  return iter;
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> &CommMobileLaserScan::const_point_iterator<Pnt>::operator--()
{
  --_index;
  return *this;
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> CommMobileLaserScan::const_point_iterator<Pnt>::operator+(int i) const
{
  return CommMobileLaserScan::const_point_iterator<Pnt>(_scan,_index+i,_unit);
}

template<class Pnt>
inline CommMobileLaserScan::const_point_iterator<Pnt> CommMobileLaserScan::const_point_iterator<Pnt>::operator-(int i) const
{
  return CommMobileLaserScan::const_point_iterator<Pnt>(_scan,_index-i,_unit);
}

template<class Pnt>
inline unsigned int CommMobileLaserScan::const_point_iterator<Pnt>::operator-(const CommMobileLaserScan::const_point_iterator<Pnt> &i) const
{
  return _index - i._index;
}

template<class Pnt>
inline const Pnt CommMobileLaserScan::const_point_iterator<Pnt>::operator*() const
{
  return Pnt(_scan,_index,_unit);
}

template<class Pnt>
inline const Pnt CommMobileLaserScan::const_point_iterator<Pnt>::operator[](int i) const
{
  return Pnt(_scan, _index+i, _unit);
}

template<class Out>
inline void CommMobileLaserScan::get_scan_polar(Out iter, const double unit) const
{
  const double dist_scale = _scan.laser_scan.length_unit * 0.001 / unit;
  const unsigned int n = get_scan_size();
  for(unsigned int i=0; i<n; ++i)
  {
    typename Out::container_type::value_type value;
    value.distance = _scan.laser_scan.scan_points[i].distance * dist_scale;
    value.angle = get_scan_angle(i);
    (*iter) = value;
    ++iter;
  }
}

template<class For>
inline unsigned int CommMobileLaserScan::get_scan_polar(For begin, For end, const double unit) const
{
  const double dist_scale = _scan.laser_scan.length_unit * 0.001 / unit;
  const unsigned int n = get_scan_size();
  For iter = begin;
  unsigned int i=0;
  for(i=0; (i<n) && (iter!=end); ++i)
  {
    (*iter).distance = _scan.laser_scan.scan_points[i].distance * dist_scale;
    (*iter).angle = get_scan_angle(i);
    ++iter;
  }
  return i;
}

template<class Out>
inline void CommMobileLaserScan::get_scan_cartesian_scanner(Out iter, const double unit) const
{
  const double dist_scale = (0.001 / unit) * get_scan_length_unit();
  const unsigned int n = get_scan_size();
  for(unsigned int i=0; i<n; ++i)
  {
    typename Out::container_type::value_type pnt;
    const double dist = _scan.laser_scan.scan_points[i].distance * dist_scale;
    const double angle = get_scan_angle(i);

    // pos in scanner coord sys, millimeter units
    pnt[0] = dist * cos(angle);
    pnt[1] = dist * sin(angle);

    (*iter) = pnt;
    ++iter;
  }
}

template<class For>
inline unsigned int CommMobileLaserScan::get_scan_cartesian_scanner(For begin, For end, const double unit) const
{
  const double dist_scale = (0.001 / unit) * get_scan_length_unit();
  const unsigned int n = get_scan_size();
  For iter = begin;
  unsigned int i = 0;
  for(i=0; (i<n) && (iter!=end); ++i)
  {
    const double dist = _scan.laser_scan.scan_points[i].distance * dist_scale;
    const double angle = get_scan_angle(i);

    // pos in scanner coord sys, millimeter units
    (*iter)[0] = dist * cos(angle);
    (*iter)[1] = dist * sin(angle);

    ++iter;
  }
  return i;
}

template<class Out>
inline void CommMobileLaserScan::get_scan_cartesian_world(Out iter, const double unit) const
{
  const double dist_scale = 0.001 / unit;
  const unsigned int n = get_scan_size();
  const double cos_scanner_azimuth = cos(_scan.scanner_a);
  const double sin_scanner_azimuth = sin(_scan.scanner_a);
  for(unsigned int i=0; i<n; ++i)
  {
    typename Out::container_type::value_type pnt;
    const double dist = _scan.laser_scan.scan_points[i].distance * get_scan_length_unit();
    const double angle = get_scan_angle(i);

    // pos in scanner coord sys, millimeter units
    const double lx = dist * cos(angle);
    const double ly = dist * sin(angle);

    pnt[0] = (_scan.scanner_x + cos_scanner_azimuth * lx - sin_scanner_azimuth * ly) * dist_scale;
    pnt[1] = (_scan.scanner_y + sin_scanner_azimuth * lx + cos_scanner_azimuth * ly) * dist_scale;

    (*iter) = pnt;
    ++iter;
  }
}

template<class For>
inline unsigned int CommMobileLaserScan::get_scan_cartesian_world(For begin, For end, const double unit) const
{
  const double dist_scale = 0.001 / unit;
  const unsigned int n = get_scan_size();
  const double cos_scanner_azimuth = cos(_scan.scanner_a);
  const double sin_scanner_azimuth = sin(_scan.scanner_a);
  For iter = begin;
  unsigned int i = 0;
  for(i=0; (i<n) && (iter!=end); ++i)
  {
    const double dist = _scan.laser_scan.scan_points[i].distance * get_scan_length_unit();
    const double angle = get_scan_angle(i);

    // pos in scanner coord sys, millimeter units
    const double lx = dist * cos(angle);
    const double ly = dist * sin(angle);

    (*iter)[0] = (_scan.scanner_x + cos_scanner_azimuth * lx - sin_scanner_azimuth * ly) * dist_scale;
    (*iter)[1] = (_scan.scanner_y + sin_scanner_azimuth * lx + cos_scanner_azimuth * ly) * dist_scale;

    ++iter;
  }
  return i;
}

inline void CommMobileLaserScan::set_scan_size(unsigned int n) 
{ 
  _scan.laser_scan.scan_points.length(n); 
}

inline void CommMobileLaserScan::set_scan_distance(unsigned int index, double dist, const double unit)
{
  _scan.laser_scan.scan_points[index].distance = int(::rint(dist * unit * 1000) / _scan.laser_scan.length_unit);
}

inline void CommMobileLaserScan::set_scan_integer_distance(unsigned int index, unsigned int dist)
{
  _scan.laser_scan.scan_points[index].distance = dist;
}

inline void CommMobileLaserScan::set_scan_intensity(unsigned int index, unsigned char intensity)
{
  _scan.laser_scan.scan_points[index].intensity = intensity;
}

inline std::ostream &operator<<(std::ostream &os, const CommMobileLaserScan &scan)
{
  scan.print(os);
  return os;
}

} // namespace Smart

#endif
