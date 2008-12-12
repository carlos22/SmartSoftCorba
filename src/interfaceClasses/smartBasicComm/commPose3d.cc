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

#include "smartKnuthMorrisPratt.hh"
#include "commPose3d.hh"

using namespace Smart;

void CommPose3d::get(CORBA::Any &a) const
{
  a <<= _pose;
}


void CommPose3d::set(const CORBA::Any &a)
{
  SmartIDL::Pose3d *tmp_pose;
  if(a >>= tmp_pose)
  {
    _pose = *tmp_pose;
  }
}

/*
CommPose3d CommPose3d::convert_to_absolute(const CommPose3d &relative_pose) const
{
  
}


CommPose3d CommPose3d::convert_to_relative(const CommPose3d &absolute_pose) const;
{
}
*/

void CommPose3d::print(std::ostream &os) const
{
  os << "CommPose3d(" << get_position() << "," << get_orientation() << ")";
}

void CommPose3d::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<pose3d>" << std::endl;
  get_position().save_xml(os, indent + "  ");
  get_orientation().save_xml(os, indent + "  ");
  os << indent << "</pose3d>" << std::endl;
}

void CommPose3d::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<pose3d>");
  static const KnuthMorrisPratt kmp_end("</pose3d>");

  kmp_begin.search(is);

  CommPosition3d p;
  p.load_xml(is);
  set_position(p);
  
  CommOrientation3d o;
  o.load_xml(is);
  set_orientation(o);

  kmp_end.search(is);
}
