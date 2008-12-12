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

#include <fstream>

#include "smartSonarDataVisualizationThread.hh"

#include "commVoid.hh"

using namespace Smart;

SonarDataVisualizationThread::SonarDataVisualizationThread
(
  CHS::SmartComponent *component,
  const std::string &component_name,
  CHS::SendClient<Smart::CommPropertySet> &registration_client,
  CHS::SendClient<Faw::TransmittablePainter> &drawing_client
)
: VisualizationThreadBase(component,component_name,registration_client,drawing_client),
  _config_query_service("sonar")
{
}

SonarDataVisualizationThread *SonarDataVisualizationThread::clone() const
{
  return new SonarDataVisualizationThread(_component, _component_name, _registration_client, _drawing_client);
}

int SonarDataVisualizationThread::svc()
{
  int ret = 0;
  CHS::StatusCode status;
  Smart::CommSonarData data;

  CHS::PushNewestClient<Smart::CommSonarData> *push_newest_client = 0;
  CHS::PushTimedClient<Smart::CommSonarData> *push_timed_client = 0;

  CHS::QueryClient<Smart::CommVoid,Smart::CommSonarConfig> query_client(_component,_data_server,_config_query_service);
  Smart::CommSonarConfig sonar_config;
  status = query_client.query(Smart::CommVoid(),sonar_config);
  if(status!=CHS::SMART_OK)
  {
    std::cerr << "ERROR: failed to query sonar config (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
    return -1;
  }
  
{
  std::ofstream ofs("sonar.cfg");
  sonar_config.save_xml(ofs);
}

  _paint_method.set_sonar_config(sonar_config);
  
  if(_interval==0)
  {
    push_newest_client = new CHS::PushNewestClient<Smart::CommSonarData>(_component,_data_server,_data_service);
    status = push_newest_client->subscribe();
  }
  else
  {
    push_timed_client = new CHS::PushTimedClient<Smart::CommSonarData>(_component,_data_server,_data_service);
    status = push_timed_client->subscribe(_interval);
  }
  if(status!=CHS::SMART_OK)
  {
    std::cerr << "ERROR: failed to subscribe (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
    ret = 1;
  }
  else
  {
    status = _register();
    if(status!=CHS::SMART_OK)
    {
      std::cerr << "ERROR: failed to register with visualization server (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
      ret = 2;
    }
    else
    {
      while(true)
      {
        if(_interval==0)
        {
          status = push_newest_client->getUpdateWait(data);
        }
        else
        {
          status = push_timed_client->getUpdateWait(data);
        }
        if(status!=CHS::SMART_OK)
        {
          std::cerr << "ERROR: failed to wait for new data (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
          ret = 4;
          break;
        }
        Faw::TransmittablePainter painter;
        painter.set_label(_name());
        painter.set_pen_color(_colour);
        painter.draw(data, _paint_method);
        status = _drawing_client.send(painter);
        if(status!=CHS::SMART_OK)
        {
          std::cerr << "ERROR: failed to send drawing data (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
          ret = 8;
          break;
        }
      }
      status = _unregister();
      if(status!=CHS::SMART_OK)
      {
        std::cerr << "WARNING: failed to unregister with visualization server (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
        ret += 16;
      }
      if(_interval==0)
      {
        status = push_newest_client->unsubscribe();
      }
      else
      {
        status = push_timed_client->unsubscribe();
      }
      if(status!=CHS::SMART_OK)
      {
        std::cerr << "WARNING: failed to unsubscribe (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
        ret += 32;
      }
    }
  }
  if(_interval==0)
  {
    delete push_newest_client;
  }
  else
  {
    delete push_timed_client;
  }
  return ret;
}

void SonarDataVisualizationThread::set_additional_parameters(const CHS::SmartParameter &parameters, const std::string &grp)
{
  parameters.getString(grp, "config", _config_query_service);
}
