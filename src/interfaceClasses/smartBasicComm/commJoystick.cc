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
#include "commJoystick.hh"

using namespace Smart;

CommJoystick::CommJoystick()
{
  _js.xpos = 0;
  _js.ypos = 0;
  _js.buttons = 0;
}

CommJoystick::CommJoystick(const SmartIDL::Joystick &js)
: _js(js)
{
}

CommJoystick::~CommJoystick()
{
}

void CommJoystick::get(CORBA::Any &a) const
{
  a <<= _js;
}

void CommJoystick::set(const CORBA::Any &a)
{
  SmartIDL::Joystick *tmp_js;
  if(a >>= tmp_js)
  {
    _js = *tmp_js;
  }
}

void CommJoystick::print(std::ostream &os) const
{
  os << "Joystick(x=" << get_x() << ",y=" << get_y() << " but=0x" << std::setw(2) << std::setfill('0') << std::hex << int(_js.buttons) << std::dec << ")";
}


void CommJoystick::save_xml(std::ostream &os, const std::string &indent) const
{
  os << indent << "<joystick>" << std::endl;
  os << indent << "  <xpos>" << _js.xpos << "</xpos>" << std::endl;
  os << indent << "  <ypos>" << _js.ypos << "</ypos>" << std::endl;
  for(unsigned int i=0; i<8; ++i)
  {
    os << indent << "  <button index=\"" << i << "\">" << int(get_button(i)) << "</button>" << std::endl;
  }
  os << indent << "</joystick>" << std::endl;
}

void CommJoystick::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<joystick>");
  static const KnuthMorrisPratt kmp_xpos("<xpos>");
  static const KnuthMorrisPratt kmp_ypos("<ypos>");
  static const KnuthMorrisPratt kmp_button("\">");
  static const KnuthMorrisPratt kmp_end("</joystick>");

  kmp_begin.search(is);
  kmp_xpos.search(is);
  is >> _js.xpos;
  kmp_ypos.search(is);
  is >> _js.ypos;
  for(unsigned int i=0; i<8; ++i)
  {
    kmp_button.search(is);
    int b;
    is >> b;
    set_button(i,b);
  }
  kmp_end.search(is);
}
