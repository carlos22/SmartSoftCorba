// --------------------------------------------------------------------------
//
//  Copyright (C) 1997/2008 Christian Schlegel, Andreas Steck
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "smartSoft.hh"

#include "commMobileLaserScan.hh"
#define INI_PARAMETERS_FILE "smartLaserCollect.ini"

CHS::SmartComponent *component;
CHS::PushNewestClient<Smart::CommMobileLaserScan> *laserClient;
double initposx,initposy,initposa;

Smart::CommMobileLaserScan scan;

double param_initposx = 0.0;
double param_initposy = 0.0;
double param_initposa = 0.0;

double param_diffDist  = 500.0;
double param_diffAngle = 30.0;

std::string param_savePatternXml = "scan%04d.2d";

// </asteck>


double normAngle(double a)
{
    if(a < -M_PI || a >= M_PI)
    {
        a = a - 2*M_PI * floor(a / (2*M_PI));
        while(a < -M_PI)
            a += 2*M_PI;
        while(a >= M_PI)
            a -= 2*M_PI;
    }

    return(a);
}

/**
 * Read the parameters.
 * The parameters are read from an ini-file.
 */
void readParameters(int argc, char **argv)
{
  CHS::SmartParameter parameter;

  try {
    parameter.addFile(argc, argv, "ini");
  } catch(CHS::ParameterError e) {
    try {
      parameter.addFile(INI_PARAMETERS_FILE);
    } catch(CHS::ParameterError e) {
      std::cerr << "No parameter file found." << std::endl;
      return;
    }
  }

  ////////////////////
  // read parameters
  //
  parameter.getDouble("initial","pos_x", param_initposx );
  std::cout << "param_initposx = " << param_initposx << std::endl;

  parameter.getDouble("initial","pos_y", param_initposy );
  std::cout << "param_initposy = " << param_initposy << std::endl;

  parameter.getDouble("initial","pos_a", param_initposa );
  std::cout << "param_initposa = " << param_initposa << std::endl;

  parameter.getDouble("initial","distance", param_diffDist );
  std::cout << "param_diffDist = " << param_diffDist << std::endl;

  parameter.getDouble("initial","angle", param_diffAngle );
  std::cout << "param_diffAngle = " << param_diffAngle << std::endl;

} // readParameters()



/**
 * Active class to collect the laserscans
 *
 */
class CollectorThread : public CHS::SmartTask
{
public:
  CollectorThread (int mode) {
    this->mode = mode;
  };

  ~CollectorThread () {};
  int svc(void);

  int mode;
};

int CollectorThread::svc(void)
{
  double x,y,a,x_old,y_old,a_old;
  double distance,angle;
  int    zaehler = 0;
  CHS::StatusCode status;

  x_old = param_initposx;
  y_old = param_initposy;
  a_old = param_initposa;

  switch( mode )
  {
    // save as XML pending on the differences of distance and angle
    case 1:
    {
      char filename[101];

      cout << "save (XML) each " << param_diffDist << " mm or each " << param_diffAngle << " degree ...\n";

      while(1)
      {
        // get scan (PushNewest)
        status = laserClient->getUpdate(scan);
        if (status != CHS::SMART_OK) {
          std::cout << "blocking wait status " << CHS::StatusCodeConversion(status) << " not ok => retry ..." << std::endl;
        }
        else
        {
          x = scan.get_base_state().get_base_position().get_x();
          y = scan.get_base_state().get_base_position().get_y();
          a = scan.get_base_state().get_base_position().get_base_alpha();

          // calculate differences of distance and angle
          distance = sqrt((x-x_old)*(x-x_old)+(y-y_old)*(y-y_old));
          angle    = fabs(normAngle(a-a_old));

          // save scan whether differences outruns the thresholds
          if ((distance > param_diffDist) || (angle > param_diffAngle))
          {
            cout << ">>>>>>>>>>>>> Successful Query " << zaehler << "\n";
            sprintf(filename, (char *)param_savePatternXml.c_str(), zaehler);
           
            std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
            scan.save_xml( myFile );
            myFile.close();

            zaehler++;
            x_old = x;
            y_old = y;
            a_old = a;
          }
       }
      } // while
      break;
    } // case 1


    // save as XML on key pressed
    case 2:
    {
      char filename[101];
      cout << "save (XML) on key pressed ...\n";

      while(1) {
        cout << "Hit <RETURN> to take scan ...\n";
        getchar();

        // get scan (PushNewest)
        status = laserClient->getUpdate(scan);
        if (status != CHS::SMART_OK) {
          std::cout << "blocking wait  status " << CHS::StatusCodeConversion(status) << " not ok => retry ..." << std::endl;
        }
        else
        {
          sprintf(filename, (char *)param_savePatternXml.c_str(), zaehler);

          std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
          scan.save_xml( myFile );
          myFile.close();

          zaehler++;
       }
      }
      break;
    } // case 2


    default:
    {
      std::cout << "selected mode is not supported !!\n";
      break;
    } // default

  } // switch( mode )

  return 0;
}


// ---------------------------------------------------------
//
// main programm
//
// ---------------------------------------------------------
int main(int argc, char** argv )
{
// <asteck date="24.07.2008">
  
  int mode;

  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    readParameters(argc,argv); 
 
    component = new CHS::SmartComponent("laserCollect",argc,argv);
    laserClient = new CHS::PushNewestClient<Smart::CommMobileLaserScan>(component);
  
    laserClient->connect("smartLaserServer","scan");
    laserClient->subscribe();

    printf("Laserscan Collector\n\n");
    printf("-1- save scan (XML) each %d mm or %4.1f degree (see .ini)\n",(int)param_diffDist, param_diffAngle);
    printf("-2- save scan (XML) on key pressed\n");

    std::cin >> mode;

    printf("\nplease enter head of goal file name (standard scan):\n");

    std::cin >> param_savePatternXml;
    param_savePatternXml = param_savePatternXml+"%04d.2d";
  
    CollectorThread collector(mode);
    collector.open();

    component->run();
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

