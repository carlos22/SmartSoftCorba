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
// --------------------------------------------------------------------------

#include "smartSoft.hh"

#include "commJoystick.hh"
#include "commNavigationVelocity.hh"

class JoystickNavigationThread: public CHS::SmartTask
{
public:
  JoystickNavigationThread
  (
    CHS::PushNewestClient<Smart::CommJoystick> &joystick_client,
    CHS::SendClient<Smart::CommNavigationVelocity> &navigation_client,
    double max_velocity,
    double max_steering
  )
  : _joystick_client(joystick_client),
    _navigation_client(navigation_client),
    _max_velocity(max_velocity),
    _max_steering(max_steering)
  {
  }
  
  int svc();

private:
  CHS::PushNewestClient<Smart::CommJoystick> &_joystick_client;
  CHS::SendClient<Smart::CommNavigationVelocity> &_navigation_client;
  double _max_velocity;
  double _max_steering;
};

int JoystickNavigationThread::svc()
{
  Smart::CommJoystick js;
  Smart::CommNavigationVelocity vel;
  CHS::StatusCode status;

  status = _joystick_client.subscribe();
  if(status!=CHS::SMART_OK)
  {
    std::cerr << "ERROR: failed to subscribe (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
    return -1;
  }

  while(true)
  {
    status = _joystick_client.getUpdateWait(js);
    if(status!=CHS::SMART_OK)
    {
      std::cerr << "ERROR: failed to get update (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
      break;
    }
    vel.set_v(js.get_y() * _max_velocity, 1.0);
    vel.set_omega(-js.get_x() * _max_steering);
    status = _navigation_client.send(vel);
    if(status!=CHS::SMART_OK)
    {
      std::cerr << "ERROR: failed to send velocity (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
      break;
    }
    else
    {
      std::cout << vel << " sent." << std::endl;
    }
  }

  status = _joystick_client.unsubscribe();
  if(status!=CHS::SMART_OK)
  {
    std::cerr << "WARNING: failed to unsubscribe (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
  }

  return 0;
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  CHS::SmartParameter parameters;
  parameters.addCommandLine("cmdline", argc, argv);

  std::string joystick_server = "smartJoystickServer";
  std::string joystick_service = "joystick";

  // TODO 
  //std::string navigation_server = "smartPioneerBaseServer";
  std::string navigation_server = "smartCdlServer";
  std::string navigation_service = "navigationvelocity";

  double max_velocity = 1.0; // [m/s]
  double max_steering = 1.2; // [rad/s]

  parameters.getString("cmdline","joystick",joystick_server);
  parameters.getString("cmdline","joystick_service",joystick_service);
  parameters.getString("cmdline","base",navigation_server);
  parameters.getString("cmdline","base_service",navigation_service);

  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component("smartExampleJoystickNavigationClient",argc,argv);

    CHS::PushNewestClient<Smart::CommJoystick> joystick_client(&component, joystick_server, joystick_service);
    CHS::SendClient<Smart::CommNavigationVelocity> navigation_client(&component, navigation_server, navigation_service);

    JoystickNavigationThread joystick_navigation_thread(joystick_client, navigation_client, max_velocity, max_steering);

    joystick_navigation_thread.open();

    component.run();
    threadManager->wait();
  } 
  catch  (const CORBA::Exception &) 
  {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  } 
  catch (...) 
  {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

