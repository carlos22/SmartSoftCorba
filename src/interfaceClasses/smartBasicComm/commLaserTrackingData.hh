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

#ifndef SMART_COMM_LASER_TRACKING_DATA_HH
#define SMART_COMM_LASER_TRACKING_DATA_HH

#include "smartLaserTrackingDataC.hh"

#include "commMobileLaserScan.hh"

namespace Smart {

/**
  This class describes a partition of a laser scan into a set of tracked objects.
  Each object is given by the index of its first and last laser scan point,
  and by its ID which is inherited from an object of the preceding scan.
 */
class CommLaserTrackingData
{
protected:
  SmartIDL::LaserTrackingData _data;

public:

  CommLaserTrackingData() {}
  CommLaserTrackingData(const SmartIDL::LaserTrackingData &data) : _data(data) {}
  virtual ~CommLaserTrackingData() {}

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommLaserTrackingData"; };

  /// internal use only
  inline const SmartIDL::LaserTrackingData &get_idl() const { return _data; }
  /// internal use only
  inline void set_idl(const SmartIDL::LaserTrackingData &data) { _data = data; }

  //
  // SET
  //

  /**
    Set the laser scan to which this tracking data refers.
   */
  inline void set_laser_scan(const CommMobileLaserScan &scan) 
  { 
    _data.scan = scan.get_idl(); 
  }

  /**
    Set the number of objects which this tracking data contains.
    Always set the number accordingly before setting object information.
   */
  inline void set_num_objects(unsigned int n) 
  { 
    _data.objects.length(n); 
  }

  /**
    Set the ID of object \a obj_idx to \a id.
   */
  inline void set_object_id(unsigned int obj_idx, unsigned long id) 
  { 
    _data.objects[obj_idx].id = id; 
  }

  /**
    Set the first scan point of object \a obj_idx to scan point index \a pnt_index.
   */
  inline void set_object_first_point(unsigned int obj_idx, unsigned int pnt_index) 
  {
    _data.objects[obj_idx].first_point = pnt_index;
  }

  /**
    Set the last scan point of object \a obj_idx to scan point index \a pnt_index.
   */
  inline void set_object_last_point(unsigned int obj_idx, unsigned int pnt_index) 
  {
    _data.objects[obj_idx].last_point = pnt_index;
  }

  //
  // GET
  //
  
  /**
    Return the laser scan to which this tracking data refers.
   */
  inline CommMobileLaserScan get_laser_scan() const 
  {
    return CommMobileLaserScan(_data.scan); 
  }

  /**
    Return the number of objects contained in this tracking data.
   */
  inline unsigned int get_num_objects() const 
  { 
    return _data.objects.length(); 
  }

  /**
    Return the ID of object \a obj_idx.
   */
  inline unsigned long get_object_id(unsigned int obj_idx) const 
  { 
    return _data.objects[obj_idx].id; 
  }

  /**
    Get the scan point index of the first scan point belonging to object \a obj_idx.
   */
  inline unsigned int get_object_first_point(unsigned int obj_idx) const 
  {
    return _data.objects[obj_idx].first_point;
  }

  /**
    Get the scan point index of the last scan point belonging to object \a obj_idx.
   */
  inline unsigned int get_object_last_point(unsigned int obj_idx) const 
  {
    return _data.objects[obj_idx].last_point;
  }

  //
  // INPUT/OUTPUT
  //

  /**
    Write information about this tracking data to the given output stream.
   */
  void print(std::ostream &os = std::cout) const;

  /**
    Save an XML like representation of this tracking data to the given output stream.
   */
  void save_xml(std::ostream &os, const std::string &indent = "") const;

  /**
    Load from XML represenation on an istream.
    Be warned, no error checking is done.
    For details, see \ref smart_basic_com-xml.
   */
  void load_xml(std::istream &is);
};

//
// Implementation
//

inline std::ostream &operator<<(std::ostream &os, const CommLaserTrackingData &data)
{
  data.print(os);
  return os;
}

} // namespace Smart

#endif
