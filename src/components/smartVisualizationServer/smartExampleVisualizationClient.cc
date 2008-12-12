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

#include "commPropertySet.hh"
#include "fawTransmittablePainter.hh"

using namespace Faw;

// -------------------------------------------------------------
//   Client Thread
// -------------------------------------------------------------
class ClientThread: public CHS::SmartTask
{
public:
  ClientThread(const std::string &client_name, CHS::SendClient<Smart::CommPropertySet> &registration_client, CHS::SendClient<Faw::TransmittablePainter> &drawing_client)
  : _client_name(client_name),
    _registration_client(registration_client),
    _drawing_client(drawing_client)
  {
  }
  
  int svc();

  void sign_on();
  void sign_off();
  void show_position(double x, double y, double a);
    
private:
  std::string _client_name;
  CHS::SendClient<Smart::CommPropertySet> &_registration_client;
  CHS::SendClient<Faw::TransmittablePainter> &_drawing_client;
};

int ClientThread::svc()
{
  while(true)
  {
    sign_on();

    double x = 0;
    double y = 0;
    double a = 0;

    const unsigned int n1 = 5 + (rand() % 5);

    for(unsigned int i=0; i<n1; ++i)
    {
      const double dx = 0.001 * ((rand() % 101) - 50);
      const double dy = 0.001 * ((rand() % 101) - 50);
      const double da = 0.004 * ((rand() % 101) - 50);

      const unsigned int n2 = 5 + (rand() % 10);

      for(unsigned int j=0; j<n2; ++j)
      {
        show_position(x,y,a);
        x += dx;
        y += dy;
        a += da;
        usleep(100*1000);
      }
    }

    sign_off();

    sleep(1);
  }
  return 0;
}

void ClientThread::sign_on()
{
  Smart::CommPropertySet m;
  m.set("name",_client_name);
  m.set("cmd","add");
  _registration_client.send(m);
}

void ClientThread::sign_off()
{
  Smart::CommPropertySet m;
  m.set("name",_client_name);
  m.set("cmd","remove");
  _registration_client.send(m);
}

void ClientThread::show_position(double x, double y, double a)
{
  static const double rect[4][2] = { { 0.1,0.1 }, { -0.1,0.1 }, { -0.1,-0.1 }, { 0.1,-0.1 } };
  Faw::TransmittablePainter painter;
  painter.set_label(_client_name);
  painter.set_length_unit(1.0);
  painter.set_transformation(x,y,a);
  painter.draw_polygon(rect, rect + 4);
  _drawing_client.send(painter);
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  std::string client_name = "smartExampleVisualizationClient";
  std::string server_name = "smartVisualizationServer";
  
  CHS::SmartParameter parameters;
  parameters.addCommandLine("cmdline", argc, argv);

  parameters.getString("cmdline","client",client_name);
  parameters.getString("cmdline","server",server_name);

  try
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component(client_name,argc,argv);

    CHS::SendClient<Smart::CommPropertySet> registration_client(&component,server_name,"registration");
    CHS::SendClient<Faw::TransmittablePainter> drawing_client(&component,server_name,"drawing");

    ClientThread client_thread1(client_name + "(Thread 1)", registration_client, drawing_client);
    ClientThread client_thread2(client_name + "(Thread 2)", registration_client, drawing_client);
    ClientThread client_thread3(client_name + "(Thread 3)", registration_client, drawing_client);
    ClientThread client_thread4(client_name + "(Thread 4)", registration_client, drawing_client);
    ClientThread client_thread5(client_name + "(Thread 5)", registration_client, drawing_client);
    ClientThread client_thread6(client_name + "(Thread 6)", registration_client, drawing_client);

    client_thread1.open();
    client_thread2.open();
    client_thread3.open();
    client_thread4.open();
    client_thread5.open();
    client_thread6.open();

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

