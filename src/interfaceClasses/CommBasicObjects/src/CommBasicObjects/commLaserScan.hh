//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

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


#ifndef _COMM_COMMBASICOBJECTS_COMMLASERSCAN_HH
#define _COMM_COMMBASICOBJECTS_COMMLASERSCAN_HH

#include <cmath>
#include <string>
#include <iostream>

// include files of classes
#include <CommBasicObjects/gen/structLaserScanPointC.hh>

// include header-files of included communication objects
#include <CommBasicObjects/commTimeStamp.hh>

// include enums


// include client-side file generated by IDL compiler
#include "CommBasicObjects/gen/smartCommLaserScanC.hh"

namespace CommBasicObjects
{

/**
  A class representing a laser scan.
 */
class CommLaserScan
{
protected:
	//CommBasicObjectsIDL::CommLaserScan idl_CommLaserScan;
	CommBasicObjectsIDL::CommLaserScan _scan;

public:
	CommLaserScan()
	{
		  _scan.is_valid = false;
		  _scan.length_unit = 1;
		  _scan.update_count = 0;
		  _scan.time.sec = 0;
		  _scan.time.usec = 0;
		  _scan.start_angle = 27000; // -90 deg
		  _scan.resolution = 50;     // 0.5 deg
		  _scan.distance_min = 0;
		  _scan.distance_max = USHRT_MAX;
	}

	CommLaserScan(const CommBasicObjectsIDL::CommLaserScan &obj) :
		_scan(obj)
	{
	}

	operator CommBasicObjectsIDL::CommLaserScan() const
	{
		return _scan;
	}

	virtual ~CommLaserScan()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	static inline std::string identifier(void)
	{
		return "CommBasicObjects::CommLaserScan";
	}

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
	    inline polar_point_type(const CommLaserScan *scan, unsigned int index, const double unit = 0.001);
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

	  //
	  // ITERATOR
	  //

	  /**
	    Random access const iterator for STL like access to polar laser scan.
	   */
	  class const_polar_iterator
	  {
	  public:
	    typedef std::random_access_iterator_tag iterator_category;
	    typedef polar_point_type value_type;
	    typedef unsigned int difference_type;
	    typedef polar_point_type* pointer;
	    typedef polar_point_type& reference;
	    inline const_polar_iterator(const CommLaserScan *scan = 0, unsigned int index = 0, const double unit = 0.001);
	    inline bool operator==(const const_polar_iterator &other) const;
	    inline bool operator!=(const const_polar_iterator &other) const;
	    inline const_polar_iterator operator++(int);
	    inline const_polar_iterator &operator++();
	    inline const_polar_iterator operator--(int);
	    inline const_polar_iterator &operator--();
	    inline const_polar_iterator operator+(int) const;
	    inline const_polar_iterator operator-(int) const;
	    inline unsigned int operator-(const const_polar_iterator&) const;
	    inline const polar_point_type operator*() const;
	    inline const polar_point_type operator[](int) const;
	  private:
	    const CommLaserScan *_scan;
	    unsigned int _index;
	    double _unit;
	  };

	  //
	  // GET
	  //

	  /**
	    Check if this scan is valid.
	    Invalid scans may be sent if a scan had to be sent in a PushTimed pattern,
	    but no valid scan was available.
	   */
	  inline bool is_scan_valid() const { return _scan.is_valid; }

	  /**
	    Returns the time stamp of the scan.
	    With the current SICK interface implementation, this is the time when
	    the header of the serial packet containing the scan was received.
	   */
	  inline CommTimeStamp get_scan_time_stamp() const { return CommTimeStamp(_scan.time); }

	  /**
	    Return the sequence count of this scan.
	    The sequence count is increased by the laser server for each transmitted scan.
	   */
	  inline unsigned long get_scan_update_count() const { return _scan.update_count; }

	  /**
	    Return the number of points in this scan.
	    A scan contains only valid points, therefore a 180 degree scan with 0.5 degree
	    resolution may contain less that 361 points.
	    Points are considered invalid, if the scanning device reports distances
	    exceeding a maximum distance.
	   */
	  inline unsigned int get_scan_size() const { return _scan.scan_points.length(); }

	  /**
	    Return the internal length unit (the resolution) of this scan in millimeters.
	    Distances are stored as integer values with respect to this length unit.
	    Therefore, this length unit defines the distance resolution of the scan.
	   */
	  inline unsigned int get_scan_length_unit() const { return _scan.length_unit; }

	  /**
	    Return the start angle of the scan in radians relative to the scanning device.
	   */
	  inline double get_scan_start_angle() const { return _d2r(_scan.start_angle); }

	  /**
	    Return the angular resolution (distance between two scan rays) of the scan in radians.
	    The returned value is non-negative and smaller than 2*pi.
	   */
	  inline double get_scan_resolution() const { return _d2r(_scan.resolution); }

	  /**
	    Return the min distance that the sensor can meassure in units of \a unit meters.
	   */
	  inline double get_min_distance(const double unit = 0.001) const
	  {
	    return _scan.distance_min * 0.001 * _scan.length_unit / unit;
	  }

	  /**
	    Return the max distance that the sensor can meassure in units of \a unit meters.
	   */
	  inline double get_max_distance(const double unit = 0.001) const
	  {
	    return _scan.distance_max * 0.001 * _scan.length_unit / unit;
	  }

	  /**
	    Return the angle of scan point number \a i in radians relative to the scanning device.
	    The returned value is non-negative and smaller than 2*pi.
	    Scan point counting starts with zero.
	    The angle of point \i is \c start_angle+resolution*point[i].index
	   */
	  inline double get_scan_angle(unsigned int i) const
	  {
	    return _d2r(_norm_angle(_scan.start_angle + _scan.scan_points[i].index * _scan.resolution));
	  }

	  /**
	    Return the distance of scan point \a i in units of \a unit meters.
	   */
	  inline double get_scan_distance(unsigned int i, const double unit = 0.001) const
	  {
	    return _scan.scan_points[i].distance * 0.001 * _scan.length_unit / unit;
	  }

	  /**
	    Return the intensitiy of the scan point.
	    The intensity of the reflected light is desribed by a value between 0 and 7.
	    This feature is available on some scanners only.
	   */
	  inline unsigned char get_scan_intensity(unsigned int i) const
	  {
	    return _scan.scan_points[i].intensity;
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

	  //
	  // SET
	  //

	  /**
	    Set the valid flag of this scan.
	   */
	  inline void set_scan_valid(bool v) { _scan.is_valid = v; }

	  /**
	    Set the time stamp of this scan.
	   */
	  inline void set_scan_time_stamp(const CommTimeStamp &ts) { _scan.time = ts; }

	  /**
	    Set the sequence counter of this scan.
	   */
	  inline void set_scan_update_count(unsigned long count) { _scan.update_count = count; }

	  /**
	    Clears the scan points (invalidates them), sets the number of scan points to \a n.
	   */
	  inline void set_scan_size(unsigned int n);

	  /**
	    Set the internal length unit (the resolution) of the scan in millimeters.
	    IMPORTANT: always set the length unit before setting any distances!
	    Otherwise, the distances that have already been set will change their meaning.
	   */
	  inline void set_scan_length_unit(unsigned int unit) { _scan.length_unit = unit; }


	  /**
	    Set the angle of the first scan ray (relative to the scanner device)
	    and the angular resolution of the scan in 0.01 degree units.
	   */
	  inline void set_scan_integer_field_of_view(int start_angle, unsigned int resolution)
	  {
	    set_scan_double_field_of_view(double(start_angle) / 100.0, double(resolution) / 100.0);
	  }

	  /**
	    Set the angle of the first scan ray (relative to the scanner device)
	    and the angular resolution of the scan in 1 degree units.
	   */
	  inline void set_scan_double_field_of_view(double start_angle, double resolution)
	  {
	    _scan.start_angle = _norm_angle(start_angle);
	    _scan.resolution = resolution;
	  }

	  /**
	    Set the angle of scan point \a i by setting its index to \a index.
	    For $i<j$, $scan\_point[i].index<scan\_point[j].index$ is required.
	   */
	  inline void set_scan_index(unsigned int i, unsigned int index)
	  {
	    _scan.scan_points[i].index = index;
	  }

	  /**
	    Set the min distance that the sensor can meassure to \a dist * \a unit meters
	   */
	  inline void set_min_distance(double dist, const double unit)
	  {
	    _scan.distance_min = int(::rint(dist * unit * 1000) / _scan.length_unit);
	  }

	  /**
	    Set the max distance that the sensor can meassure to \a dist * \a unit meters
	   */
	  inline void set_max_distance(double dist, const double unit)
	  {
	    _scan.distance_max = int(::rint(dist * unit * 1000) / _scan.length_unit);
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


	  inline double _norm_angle(const double d) const
	  {
		 double a = fmod(d, 360.0);
		 if (d < 0)
			 a += 360.0;

		 return a;
	  }

	  /**
	    Convert degrees to radians.
	   */
	  inline double _d2r(double d) const { return d * 0.01745329; }

	  /**
	    Convert radians to degrees.
	   */
	  inline double _r2d(double r)  const { return int(::rint(r * 57.29578)); }

	};

	inline CommLaserScan::polar_point_type::polar_point_type(const CommLaserScan *scan, unsigned int index, const double unit)
	{
	  distance = scan->get_scan_distance(index, unit);
	  angle = scan->get_scan_angle(index);
	  intensity = scan->get_scan_intensity(index);
	}

	inline std::ostream &operator<<(std::ostream &os, const struct CommLaserScan::polar_point_type::polar_point_type &pnt)
	{
	  return os << "polar_point(a=" << pnt.angle << ",d=" << pnt.distance << ",i=" << int(pnt.intensity) << ")";
	}


	inline CommLaserScan::const_polar_iterator::const_polar_iterator(const CommLaserScan *scan, unsigned int index, const double unit)
	: _scan(scan), _index(index), _unit(unit)
	{
	}

	inline bool CommLaserScan::const_polar_iterator::operator==(const CommLaserScan::const_polar_iterator &other) const
	{
	  return (_scan==other._scan) && (_index==other._index);
	}

	inline bool CommLaserScan::const_polar_iterator::operator!=(const CommLaserScan::const_polar_iterator &other) const
	{
	  return !(*this==other);
	}

	inline CommLaserScan::const_polar_iterator CommLaserScan::const_polar_iterator::operator++(int)
	{
	  CommLaserScan::const_polar_iterator iter = *this;
	  ++_index;
	  return iter;
	}

	inline CommLaserScan::const_polar_iterator &CommLaserScan::const_polar_iterator::operator++()
	{
	  ++_index;
	  return *this;
	}

	inline CommLaserScan::const_polar_iterator CommLaserScan::const_polar_iterator::operator--(int)
	{
	  CommLaserScan::const_polar_iterator iter = *this;
	  --_index;
	  return iter;
	}

	inline CommLaserScan::const_polar_iterator &CommLaserScan::const_polar_iterator::operator--()
	{
	  --_index;
	  return *this;
	}

	inline CommLaserScan::const_polar_iterator CommLaserScan::const_polar_iterator::operator+(int i) const
	{
	  return CommLaserScan::const_polar_iterator(_scan,_index+i,_unit);
	}

	inline CommLaserScan::const_polar_iterator CommLaserScan::const_polar_iterator::operator-(int i) const
	{
	  return CommLaserScan::const_polar_iterator(_scan,_index-i,_unit);
	}

	inline unsigned int CommLaserScan::const_polar_iterator::operator-(const CommLaserScan::const_polar_iterator &i) const
	{
	  return _index - i._index;
	}

	inline const CommLaserScan::polar_point_type CommLaserScan::const_polar_iterator::operator*() const
	{
	  return CommLaserScan::polar_point_type(_scan,_index,_unit);
	}

	inline const CommLaserScan::polar_point_type CommLaserScan::const_polar_iterator::operator[](int i) const
	{
	  return CommLaserScan::polar_point_type(_scan, _index+i, _unit);
	}

	template<class Out>
	inline void CommLaserScan::get_scan_polar(Out iter, const double unit) const
	{
	  const double dist_scale = _scan.length_unit * 0.001 / unit;
	  const unsigned int n = get_scan_size();
	  for(unsigned int i=0; i<n; ++i)
	  {
	    typename Out::container_type::value_type value;
	    value.distance = _scan.scan_points[i].distance * dist_scale;
	    value.angle = get_scan_angle(i);
	    (*iter) = value;
	    ++iter;
	  }
	}

	template<class For>
	inline unsigned int CommLaserScan::get_scan_polar(For begin, For end, const double unit) const
	{
	  const double dist_scale = _scan.length_unit * 0.001 / unit;
	  const unsigned int n = get_scan_size();
	  For iter = begin;
	  unsigned int i=0;
	  for(i=0; (i<n) && (iter!=end); ++i)
	  {
	    (*iter).distance = _scan.scan_points[i].distance * dist_scale;
	    (*iter).angle = get_scan_angle(i);
	    ++iter;
	  }
	  return i;
	}

	inline void CommLaserScan::set_scan_size(unsigned int n)
	{
	  _scan.scan_points.length(n);
	}

	inline void CommLaserScan::set_scan_distance(unsigned int index, double dist, const double unit)
	{
	  _scan.scan_points[index].distance = int(::rint(dist * unit * 1000) / _scan.length_unit);
	}

	inline void CommLaserScan::set_scan_integer_distance(unsigned int index, unsigned int dist)
	{
	  _scan.scan_points[index].distance = dist;
	}

	inline void CommLaserScan::set_scan_intensity(unsigned int index, unsigned char intensity)
	{
	  _scan.scan_points[index].intensity = intensity;
	}

	inline std::ostream &operator<<(std::ostream &os, const CommLaserScan &scan)
	{
	  scan.print(os);
	  return os;
	}

}
#endif
