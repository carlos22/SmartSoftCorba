//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//                2009 Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2.1
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this library; if not, write to the Free Software Foundation, Inc.,
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#include "ParameterHandler.hh"
#include "gen/SmartPioneerBaseServer.hh"

#include <iostream>

void ParameterHandler::handleSend(const CommBasicObjects::CommBaseParameter &r) throw()
{
    CommBasicObjects::BaseTagType tag;
    int p1,p2,p3,p4,p5;

    r.get(tag,p1,p2,p3,p4,p5);

    switch (tag)
    {
      //case Smart::BASE_RESET:
      case CommBasicObjects::BaseTagType::BASE_RESET:
      {
    	  COMP->robot->resetPosition();
    	  std::cout << "RESET BASE !!!!!!!!!!!!!!\n\n";
    	  break;
      } // case BASE_RESET

      //case Smart::SONAR:
      case CommBasicObjects::BaseTagType::BASE_SONAR:
      {
    	  //COMP->robot->resetPosition();
    	  std::cout << "SONAR = " << p1 << std::endl;
    	  COMP->robot->setSonarState( (bool) p1);
    	  break;
      } // case SONAR

    } // switch
}
