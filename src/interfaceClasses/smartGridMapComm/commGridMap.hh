// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
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
// --------------------------------------------------------------------------

#ifndef SMART_COMM_GRID_MAP_HH
#define SMART_COMM_GRID_MAP_HH

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>

#include "commTimeStamp.hh"
#include "smartGridMapC.hh"

// --------------------------------------------------------------------
// Values for the actual map
// 0-127    denote traversability with 0 completely free
// 128-255  special values
// --------------------------------------------------------------------
#define MAPPER_FREE          0
#define MAPPER_OBSTACLE      128
#define MAPPER_GROWING       129
#define MAPPER_UNDELETABLE   130

// --------------------------------------------------------------------
// Values to define obstacle growing
// --------------------------------------------------------------------
#define MAPPER_GROWING_NO          0
#define MAPPER_GROWING_CIRCLE_16   1
#define MAPPER_GROWING_CIRCLE_8    2
#define MAPPER_GROWING_STAR_16     3
#define MAPPER_GROWING_STAR_12     4

namespace Smart {

/**
  A class representing a grid map
 */
class CommGridMap
{
protected:
  SmartIDL::GridMap _gridmap;

public:
  // initialize the map class
  CommGridMap();

  // copy constructor
  CommGridMap(const SmartIDL::GridMap &gridmap);

  // implicitly destroy the map class
  virtual ~CommGridMap();

  void get(CORBA::Any &a) const;
  void set(const CORBA::Any &a);
  static inline std::string identifier() { return "Smart::CommGridMap"; };


  /// extract all relevant parameters from the communication class of the map
  int get_parameter( unsigned int &id, 
                     bool &is_valid, 
                     struct timeval &time, 
                     int &xOffsetMM, 
                     int &yOffsetMM,
                     int &xOffsetCells, 
                     int &yOffsetCells, 
                     unsigned int &cellSizeMM, 
                     unsigned int &xSizeMM, 
                     unsigned int &ySizeMM, 
                     unsigned int &xSizeCells, 
                     unsigned int &ySizeCells ) const;

  /// extract a cell from the communication class of the map
 inline unsigned char get_cells(unsigned int xcell, unsigned int ycell) const
 {
   return ( _gridmap.cell[xcell+ycell*_gridmap.xSizeCells]);
 }

    

  /// save the map in X11 XPM format
  void save_xpm( std::ostream &os ) const;

  /// save map in XML format
  void save_xml( std::ostream &os, const std::string &indent = "" ) const;

  /// load map from XML format
  void load_xml( std::istream &is );


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INLINE

  /**
    Get the time stamp.
   */
  inline CommTimeStamp get_time_stamp() const { return CommTimeStamp(_gridmap.time); }

  /**
    Set the time stamp.
   */
  inline void set_time_stamp(const CommTimeStamp &ts) { _gridmap.time = ts.get_idl(); }



};

}

#endif

