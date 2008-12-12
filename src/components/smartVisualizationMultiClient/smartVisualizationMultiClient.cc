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

#include "smartSoft.hh"

#include "commBaseState.hh"
#include "commMobileLaserScan.hh"
#include "commLaserTrackingData.hh"
#include "commSonar.hh"

#include "commPropertySet.hh"
#include "fawTransmittablePainter.hh"

#include "smartDefaultPaintMethods.hh"
#include "smartSonarDataPaintMethod.hh"

#include "smartVisualizationThread.hh"
#include "smartBaseStateVisualizationThread.hh"
#include "smartSonarDataVisualizationThread.hh"

// -------------------------------------------------------------
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  std::string component_name = "smartVisualizationMultiClient";
  std::string drawing_server = "smartVisualizationServer";

  CHS::SmartParameter parameters;
  try {
    parameters.addFile(argc,argv,"ini");
  } catch(CHS::ParameterError e) {
    std::cerr << "WARNING: " << e.what() << std::endl;
    try {
      parameters.addFile("smartVisualizationMultiClient.ini");
    } catch(CHS::ParameterError e) {
      std::cerr << "WARNING: " << e.what() << std::endl;
    }
  }

  parameters.getString("component","name",component_name);
  parameters.getString("visualizationserver","name",drawing_server);

  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    CHS::SmartComponent component(component_name,argc,argv);

    CHS::SendClient<Smart::CommPropertySet> registration_client(&component, drawing_server, "registration");
    CHS::SendClient<Faw::TransmittablePainter> drawing_client(&component, drawing_server, "drawing");

    std::list<Smart::VisualizationThreadBase*> visualization_threads;
    std::list<Smart::VisualizationThreadBase*>::iterator thread_iter;

    std::map<std::string, Smart::VisualizationThreadBase*> thread_prototypes;
    std::map<std::string, Smart::VisualizationThreadBase*>::iterator prototype_iter;

    // set up thread prototypes
    thread_prototypes["mobilelaserscan"] = new Smart::VisualizationThread<Smart::CommMobileLaserScan>(&component,component_name,registration_client,drawing_client);
    thread_prototypes["baseposition"] = new Smart::VisualizationThread<Smart::CommBasePosition>(&component,component_name,registration_client,drawing_client);
    thread_prototypes["basestate"] = new Smart::BaseStateVisualizationThread(&component,component_name,registration_client,drawing_client);
//    thread_prototypes["lasertrackingdata"] = new Smart::VisualizationThread<Smart::CommLaserTrackingData>(&component,component_name,registration_client,drawing_client);
    thread_prototypes["tracking"] = new Smart::VisualizationThread<Smart::CommLaserTrackingData>(&component,component_name,registration_client,drawing_client);
    thread_prototypes["sonardata"] = new Smart::SonarDataVisualizationThread(&component,component_name,registration_client,drawing_client);

    // create the dataserver client threads 
    unsigned int num_dataservers = 0;
    parameters.getInt("dataservers","number",num_dataservers);
    for(unsigned int i = 0; i<num_dataservers; ++i)
    {
      std::ostringstream grp_oss;
      grp_oss << "dataserver-" << (i+1);
      const std::string grp = grp_oss.str();

      std::string datatype;
      if(parameters.getString(grp, "datatype", datatype))
      {
        prototype_iter = thread_prototypes.find(datatype);
        if(prototype_iter!=thread_prototypes.end())
        {
          Smart::VisualizationThreadBase *thread = (*prototype_iter).second->clone();
          thread->set_parameters(parameters, grp);
          visualization_threads.push_back(thread);
        }
        else
        {
          std::cerr << "WARNING: unknown data type \"" << datatype << "\"" << std::endl;
        }
      }
      else
      {
        std::cerr << "WARNING: no data type given for " << grp << std::endl;
      }
    }

    // open the threads
    thread_iter = visualization_threads.begin(); 
    while(thread_iter!=visualization_threads.end())
    {
      (*thread_iter)->open();
      ++thread_iter;
    }

    component.run();
    threadManager->wait();
    
    // destroy the threads
    while(!visualization_threads.empty())
    {
      delete visualization_threads.front();
      visualization_threads.pop_front();
    }
    
    // destroy thread prototypes
    prototype_iter = thread_prototypes.begin();
    while(prototype_iter!=thread_prototypes.end())
    {
      delete (*prototype_iter).second;
      ++prototype_iter;
    }
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

