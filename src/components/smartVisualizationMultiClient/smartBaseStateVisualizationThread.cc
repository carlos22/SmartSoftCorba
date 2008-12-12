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

#include "smartBaseStateVisualizationThread.hh"

using namespace Smart;

BaseStateVisualizationThread *BaseStateVisualizationThread::clone() const
{
  return new BaseStateVisualizationThread(_component, _component_name, _registration_client, _drawing_client);
}

int BaseStateVisualizationThread::svc()
{
  int ret = 0;
  CHS::StatusCode status;
  Smart::CommBaseState data;

  CHS::PushNewestClient<Smart::CommBaseState> *push_newest_client = 0;
  CHS::PushTimedClient<Smart::CommBaseState> *push_timed_client = 0;
  
  if(_interval==0)
  {
    push_newest_client = new CHS::PushNewestClient<Smart::CommBaseState>(_component,_data_server,_data_service);
    status = push_newest_client->subscribe();
  }
  else
  {
    push_timed_client = new CHS::PushTimedClient<Smart::CommBaseState>(_component,_data_server,_data_service);
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

void BaseStateVisualizationThread::set_additional_parameters(const CHS::SmartParameter &parameters, const std::string &grp)
{
  std::string shape_string;
  if(parameters.getString(grp,"baseshape",shape_string))
  {
    std::istringstream iss(shape_string);
    unsigned int num_chains;
    iss >> num_chains;
    std::vector<std::list<Faw::Painter::Point> > shape(num_chains);
    for(unsigned int i=0; i<num_chains; ++i)
    {
      unsigned int num_points;
      iss >> num_points;
      for(unsigned int j=0; j<num_points; ++j)
      {
        double x,y;
        iss >> x >> y;
        shape[i].push_back(Faw::Painter::Point(x,y));
      }
    }
    _paint_method.set_base_shape(shape.begin(), shape.end(), 1.0);
  }
  if(parameters.getString(grp,"turretshape",shape_string))
  {
    std::istringstream iss(shape_string);
    unsigned int num_chains;
    iss >> num_chains;
    std::vector<std::list<Faw::Painter::Point> > shape(num_chains);
    for(unsigned int i=0; i<num_chains; ++i)
    {
      unsigned int num_points;
      iss >> num_points;
      for(unsigned int j=0; j<num_points; ++j)
      {
        double x,y;
        iss >> x >> y;
        shape[i].push_back(Faw::Painter::Point(x,y));
      }
    }
    _paint_method.set_turret_shape(shape.begin(), shape.end(), 1.0);
  }
}
