// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft laser converter component".
//  It provides mapping services based on grid maps. The current map
//  represents the latest snapshot of the local surrounding based on
//  laserscans. The current map can be preoccupied by the longterm map.
//  The longterm map is a simple occupancy grid map.
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
// --------------------------------------------------------------------------


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "smartSoft.hh"

#include "smartConvertibleMobileLaserScan.hh"
//#include "commMobileLaserScan.hh"
#include "CommBasicObjects/commMobileLaserScan.hh"

#include "smartLaserXfig.hh"

double NormAngle(double a)
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


// ---------------------------------------------------------
//
// main programm
//
// ---------------------------------------------------------
int main(int argc, char** argv )
{
  Smart::SmartConvertibleMobileLaserScan scan;
  char         filename[101];
  std::string  param_savePatternXml = "scan%04d.2d";
  std::string  headfile;
  std::string  inputFilePattern;

  SmartLaserXfigClass drawing;

  bool         inputFileOk = false;
  int          inputFormat;
  int          outputFormat;
  int          number;
  int          s1;
  int          s2;

  std::cout << "\n\nScan conversion utility\n";
  std::cout << "=======================\n\n";


  /////////////////////////////////////////////////////
  // read Laserscan from file

  std::cout << "please choose your input file format:\n";
  std::cout << "-1- MobileLaserScan XML\n";
  std::cout << "-2- Steffen/CHS SmartSoft\n";
  std::cout << "-3- Steffen/ScanStudio\n";

  std::cin >> inputFormat;

  std::cout << "\nplease choose your output file format:\n";
  std::cout << "-1- MobileLaserScan XML\n";
  std::cout << "-2- Steffen/CHS SmartSoft\n";
  std::cout << "-3- Steffen/ScanStudio\n";
  std::cout << "-4- xFig (several files)\n";
  std::cout << "-5- xFig (merge to one file)\n";

  std::cin >> outputFormat;


  std::cout << "\nplease enter head of input files (standard scan): \n";
  std::cin >> headfile;
  param_savePatternXml = headfile+"%04d.2d";


  std::cout << "please enter first scan number:\n";
  std::cin >> s1;
  std::cout << "please enter last scan number:\n";
  std::cin >> s2;
  std::cout << "please enter head of goal file name:\n";
  std::cin >> headfile;

  // convert all scans ...
  for (number=s1;number<=s2;number++) 
  {

    //////////////////////////////////////////////////////////////
    // load scan dependend on the chosen file format (inputFormat)
    switch (inputFormat) 
    {
      // load XML
      case 1:
      {
        sprintf(filename, (char *)param_savePatternXml.c_str(), number);
        std::ifstream inFile ( filename );
        if( inFile.is_open() )
        {
          scan.load_xml( inFile );
          inFile.close();
          inputFileOk = true;
        }
        else
        {
          inputFileOk = false;
          std::cout << "Error opening file";
        }
        break; 
      } // case 1

      // load Steffen/CHS SmartSoft -> flag = 0
      case 2:
      {
        sprintf(filename, (char *)param_savePatternXml.c_str(), number);
        inputFileOk = scan.load_formatSteffen( filename, 0 );
        break; 
      } // case 2

      // load Steffen/ScanStudio -> flag = 1
      case 3:
      {
        sprintf(filename, (char *)param_savePatternXml.c_str(), number);
        inputFileOk = scan.load_formatSteffen( filename , 1 );
        break; 
      } // case 3

    } // switch
    // file loaded


    //////////////////////////////////////////////////////////////
    // save scan dependend on the chosen file format (outputFormat)
    if( inputFileOk )
    {
      switch (outputFormat) 
      {
        // save XML
        case 1: 
        {
          sprintf(filename,"%s%04d.2d",headfile.c_str(),number);
          std::ofstream outFile ( filename , std::ios::out | std::ios::binary);
          if( outFile.is_open() )
          {
            scan.save_xml( outFile );
            outFile.close();
          }
          else
          {
            std::cout << "Error opening file";
          }
          break;
        } //  case 1


        // save Steffen/CHS SmartSoft -> flag = 0
        case 2:
        {
          sprintf(filename,"%s%04d.2d",headfile.c_str(),number);
          scan.save_formatSteffen( filename, 0 );
          break;
        } // case 2

        // save Steffen/ScanStudio -> flag = 1
        case 3:
        {
          sprintf(filename,"%s%04d.2d",headfile.c_str(),number);
          scan.save_formatSteffen( filename, 1 );
          break;
        } // case 3

        // save xfig (several files)
        case 4:
        {
          sprintf(filename,"%s%04d.fig",headfile.c_str(),number);
          scan.save_xFig( filename );
          break;
        } // case 4
 
        // save xfig (merge to one file)
        case 5:
        {
          // first call inside for-loop -> open drawing
          if( number == s1 )
          {
            headfile += ".fig";
            drawing.open( headfile );
          }

          // append scan to drawing
          scan.save_xFig( &drawing );

          // last call inside for-loop -> close drawing
          if( number == s2 )
          {
            drawing.close();
          }
          break;
        } // case 5

      } // switch
    } // if( fileOpened )
    // file saved

  } // for
  // all scans converted

  return 0;

}

