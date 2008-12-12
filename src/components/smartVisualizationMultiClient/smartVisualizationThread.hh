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

#ifndef SMART_VISUALIZATION_THREAD_HH
#define SMART_VISUALIZATION_THREAD_HH

#include <string>

#include "smartSoft.hh"
#include "commPropertySet.hh"
#include "fawPainter.hh"
#include "fawTransmittablePainter.hh"

namespace Smart {

class VisualizationThreadBase: public CHS::SmartTask
{
public:

  VisualizationThreadBase
  (
    CHS::SmartComponent *component,
    const std::string &component_name,
    CHS::SendClient<Smart::CommPropertySet> &registration_client,
    CHS::SendClient<Faw::TransmittablePainter> &drawing_client
  );

  virtual ~VisualizationThreadBase();

  virtual VisualizationThreadBase *clone() const = 0;

  void set_parameters(const CHS::SmartParameter &parameters, const std::string &grp); 

protected:

  virtual void set_additional_parameters(const CHS::SmartParameter &parameters, const std::string &grp) {}

  CHS::SmartComponent *_component;
  std::string _component_name;
  std::string _data_server;
  std::string _data_service;
  unsigned int _interval;
  CHS::SendClient<Smart::CommPropertySet> &_registration_client;
  CHS::SendClient<Faw::TransmittablePainter> &_drawing_client;

  unsigned int _colour;

  std::string _name();
  CHS::StatusCode _register();
  CHS::StatusCode _unregister();
};


template<class T, class P = Faw::DefaultPaintMethod<T> >
class VisualizationThread: public VisualizationThreadBase
{
public:
  VisualizationThread
  (
    CHS::SmartComponent *component,
    const std::string &component_name,
    CHS::SendClient<Smart::CommPropertySet> &registration_client,
    CHS::SendClient<Faw::TransmittablePainter> &drawing_client
  ) 
  : VisualizationThreadBase(component,component_name,registration_client,drawing_client)
  {
  }

  virtual VisualizationThread<T,P> *clone() const
  {
    return new VisualizationThread<T,P>(_component, _component_name, _registration_client, _drawing_client);
  }

  int svc()
  {
    int ret = 0;
    CHS::StatusCode status;
    T data;
    P paint_method;

    CHS::PushNewestClient<T> *push_newest_client = 0;
    CHS::PushTimedClient<T> *push_timed_client = 0;
    
    if(_interval==0)
    {
      push_newest_client = new CHS::PushNewestClient<T>(_component,_data_server,_data_service);
      status = push_newest_client->subscribe();
    }
    else
    {
      push_timed_client = new CHS::PushTimedClient<T>(_component,_data_server,_data_service);
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
          painter.draw(data, paint_method);
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
};

} // namespace Smart

#endif
