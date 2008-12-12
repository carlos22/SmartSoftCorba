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
#include "commSonar.hh"

using namespace Smart;

CommSonarConfig::CommSonarConfig()
{
}

CommSonarConfig::CommSonarConfig(const SmartIDL::SonarSystemConfig &config)
: _config(config)
{
}

CommSonarConfig::~CommSonarConfig()
{
}

void CommSonarConfig::get(CORBA::Any &a) const
{
  a <<= _config;
}

void CommSonarConfig::set(const CORBA::Any &a)
{
  SmartIDL::SonarSystemConfig *tmp_config;
  if(a >>= tmp_config)
  {
    _config = *tmp_config;
  }
}

void CommSonarConfig::print(std::ostream &os) const
{
  const unsigned int n = get_num_sensors();
  os << "SonarConfig(";
  for(unsigned int i=0; i<n; ++i)
  {
    os << "(i=" << i << "," << get_position(i) << "," << get_direction(i) << ",half_width=" << _config.sonar_config[i].half_width << ")";
  }
  os << ")";
}

void CommSonarConfig::save_xml(std::ostream &os, const std::string &indent) const
{
  const unsigned int n = get_num_sensors();
  os << indent << "<sonar_config n=\"" << n << "\">" << std::endl;
  for(unsigned int i=0; i<n; ++i)
  {
    os << indent << "  <sensor id=\"" << i << "\">" << std::endl;
    get_position(i).save_xml(os, indent + "    ");
    get_direction(i).save_xml(os, indent + "    ");
    os << indent << "    <half_width>" << _config.sonar_config[i].half_width << "</half_width>" << std::endl;
    os << indent << "  </sensor>" << std::endl;
  }
  os << indent << "</sonar_config>" << std::endl;
}

void CommSonarConfig::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<sonar_config  n=\"");
  static const KnuthMorrisPratt kmp_sensor_begin("<sensor id=\"");
  static const KnuthMorrisPratt kmp_width("<half_width>");
  static const KnuthMorrisPratt kmp_sensor_end("</sensor>");
  static const KnuthMorrisPratt kmp_end("</sonar_config>");

  kmp_begin.search(is);

  unsigned int n;
  is >> n;
  set_num_sensors(n);

  CommPosition3d pos;
  CommDirection3d dir;
  for(unsigned int i=0; i<n; ++i)
  {
    kmp_sensor_begin.search(is);
    pos.load_xml(is);
    set_position(i, pos);
    dir.load_xml(is);
    set_direction(i,dir);
    kmp_width.search(is);
    is >> _config.sonar_config[i].half_width;
    kmp_sensor_end.search(is);
  }

  kmp_end.search(is);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


CommSonarData::CommSonarData()
{
}

CommSonarData::CommSonarData(const SmartIDL::SonarSystemData &data)
: _data(data)
{
}

CommSonarData::~CommSonarData()
{
}

void CommSonarData::get(CORBA::Any &a) const
{
  a <<= _data;
}

void CommSonarData::set(const CORBA::Any &a)
{
  SmartIDL::SonarSystemData *tmp_data;
  if(a >>= tmp_data)
  {
    _data = *tmp_data;
  }
}

void CommSonarData::print(std::ostream &os) const
{
  const unsigned int n = get_num_measurements();
  os << "SonarData(";
  os << get_base_state();
  os << ",n=" << n; 
  for(unsigned int i=0; i<n; ++i)
  {
    os << ",(id=" << get_sensor_id(i) << "," << get_measurement_time_stamp(i) << ",valid=" << int(is_measurement_valid(i)) << ",dist=" << get_measured_distance(i,1.0) << "m)";
  }
  os << ")";
}

void CommSonarData::save_xml(std::ostream &os, const std::string &indent) const
{
  const unsigned int n = get_num_measurements();
  os << indent << "<sonar_data n=\"" << n << "\">" << std::endl;
  get_base_state().save_xml(os, indent + "  ");
  for(unsigned int i=0; i<n; ++i)
  {
    os << indent << "  <measurement i=\"" << i << "\">" << std::endl;
    os << indent << "    <sensor id=\"" << get_sensor_id(i) << "\">" << std::endl;
    get_measurement_time_stamp(i).save_xml(os, indent + "    ");
    os << indent << "    <valid>" << int(is_measurement_valid(i)) << "</valid>" << std::endl;
    os << indent << "    <distance>" << _data.sonar_data[i].distance << "</distance>" << std::endl;
    os << indent << "  </measurement>" << std::endl;
  }
  os << indent << "</sonar_data>" << std::endl;
}

void CommSonarData::load_xml(std::istream &is)
{
  static const KnuthMorrisPratt kmp_begin("<sonar_data n=\"");
  static const KnuthMorrisPratt kmp_measurement_begin("<measurement");
  static const KnuthMorrisPratt kmp_sensor("<sensor id=\"");
  static const KnuthMorrisPratt kmp_valid("<valid>");
  static const KnuthMorrisPratt kmp_distance("<distance>");
  static const KnuthMorrisPratt kmp_measurement_end("</measurement>");
  static const KnuthMorrisPratt kmp_end("</sonar_data>");

  unsigned int n;
  kmp_begin.search(is);
  is >> n;
  set_num_measurements(n);
  
  CommBaseState bs;
  bs.load_xml(is);
  set_base_state(bs);
  
  unsigned int d;
  CommTimeStamp ts;
  for(unsigned int i=0; i<n; ++i)
  {
    kmp_measurement_begin.search(is);

    kmp_sensor.search(is);
    is >> d;
    set_sensor_id(i,d);

    ts.load_xml(is);
    set_measurement_time_stamp(i, ts);
    
    kmp_valid.search(is);
    is >> d;
    set_measurement_valid(i,d);

    kmp_distance.search(is);
    is >> _data.sonar_data[i].distance;

    kmp_measurement_end.search(is);
  }

  kmp_end.search(is);
}
