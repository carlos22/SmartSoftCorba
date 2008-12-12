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
#include "commBasePositionUpdate.hh"

using namespace Smart;

CommBasePositionUpdate::CommBasePositionUpdate()
{
}

CommBasePositionUpdate::CommBasePositionUpdate(const SmartIDL::BasePositionUpdate &upd)
: _upd(upd)
{
}

CommBasePositionUpdate::~CommBasePositionUpdate()
{
}

void CommBasePositionUpdate::get(CORBA::Any &a) const
{
  a <<= _upd;
}

void CommBasePositionUpdate::set(const CORBA::Any &a)
{
  SmartIDL::BasePositionUpdate *tmp_upd;
  if(a >>= tmp_upd)
  {
    _upd = *tmp_upd;
  }
}

void CommBasePositionUpdate::print(std::ostream &os) const
{
  os << "CommBasePositionUpdate("
     << "time=" << get_time_stamp()
     << ",old=" << get_old_position()
     << ",corrected=" << get_corrected_position() << ")";
}

void CommBasePositionUpdate::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<base_position_update>" << std::endl;
  get_time_stamp().save_xml(os, indent + "  ");
  get_old_position().save_xml(os, indent + "  ");
  get_corrected_position().save_xml(os, indent + "  ");
  os << indent << "</base_position_update>" << std::endl;
}

void CommBasePositionUpdate::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<base_position_update>");
  static const KnuthMorrisPratt kmp_end("</base_position_update>");

  kmp_begin.search(is);

  CommTimeStamp ts;
  CommBasePosition old, corrected;

  ts.load_xml(is);
  old.load_xml(is);
  corrected.load_xml(is);

  set_time_stamp(ts);
  set_old_position(old);
  set_corrected_position(corrected);

  kmp_end.search(is);
}
