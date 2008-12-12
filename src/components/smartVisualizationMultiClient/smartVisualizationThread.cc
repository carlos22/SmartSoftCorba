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
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// --------------------------------------------------------------------------

#include "smartVisualizationThread.hh"

using namespace Smart;

VisualizationThreadBase::VisualizationThreadBase
(
  CHS::SmartComponent *component,
  const std::string &component_name,
  CHS::SendClient<Smart::CommPropertySet> &registration_client,
  CHS::SendClient<Faw::TransmittablePainter> &drawing_client
)
: _component(component),
  _component_name(component_name),
  _data_server(""),
  _data_service(""),
  _registration_client(registration_client),
  _drawing_client(drawing_client),
  _colour(0)
{
}

VisualizationThreadBase::~VisualizationThreadBase()
{
}

void VisualizationThreadBase::set_parameters(const CHS::SmartParameter &parameters, const std::string &grp)
{
  parameters.getString(grp, "name", _data_server);
  parameters.getString(grp, "service", _data_service);
  parameters.getInt(grp, "interval", _interval);

  parameters.getInt(grp, "colour", _colour);

  this->set_additional_parameters(parameters, grp);
}

std::string VisualizationThreadBase::_name()
{
  const std::string colon = ":";
  return _component_name + colon + _data_server + colon + _data_service;
}

CHS::StatusCode VisualizationThreadBase::_register()
{
  Smart::CommPropertySet prop_set;
  prop_set.set("name", _name());
  prop_set.set("cmd","add");
  return _registration_client.send(prop_set);
}

CHS::StatusCode VisualizationThreadBase::_unregister()
{
  Smart::CommPropertySet prop_set;
  prop_set.set("name", _name());
  prop_set.set("cmd","remove");
  return _registration_client.send(prop_set);
}
