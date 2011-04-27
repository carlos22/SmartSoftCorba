//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.6.0
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
// If you want the workflow to re-generate this file, please
// delete it before running the workflow.
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck, Matthias Lutz
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
//  This file is part of the "SmartSoft Mapper/GridMapper component".
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

#include "CurMapTask.hh"
#include "gen/SmartMapperGridMap.hh"

#include <iostream>

CurMapTask::CurMapTask()
{
	std::cout << "constructor CurMapTask\n";
}

int CurMapTask::svc()
{
	  CommBasicObjects::CommMobileLaserScan laserscan;

	  struct MapperStateStruct localState;
	  int                      updateCounter;
	  int                      status;
	  CHS::StatusCode          statusCode;

	  updateCounter=0;
	  while(1)
	  {
		  // ------------------------------------------------
		  // wait for activation
		  // ("buildcurrentmap")
		  // ------------------------------------------------
		  statusCode = COMP->stateServer->acquire("buildcurrentmap");
		  if (statusCode == CHS::SMART_OK)
		  {
			  // current map autoupdate service is activated
			  COMP->MapperStateLock.acquire();
			  localState = COMP->globalState;
			  COMP->MapperStateLock.release();

			  // wait for scan (PushNewest)
			  statusCode = COMP->laserClient->getUpdateWait(laserscan);
			  if (statusCode == CHS::SMART_OK)
			  {
				  // laserscan received
				  updateCounter++;
				  if (updateCounter >= localState.curInterval)
				  {
					  //
					  // generate current map and send it via autoupdate to clients
					  //
					  updateCounter=0;
					  if(COMP->ini.general.verbose == true)
					  {
						  std::cout << "MAPPER: autoupdate current map: generate current map\n";
					  }

					  COMP->CurLock.acquire();

					  if (localState.curEmpty == 0)
					  {
						  // accumulating mode for current map
						  // convert obstacle growing cells into normal occupied cells
						  status = COMP->currentGridMap->convertObstacleGrowing();
						  if(COMP->ini.general.verbose == true)
						  {
							  std::cout << "MAPPER: autoupdate current map: accumulating mode\n";
						  }
					  }
					  else
					  {
						  // delete current map for each cycle
						  status = COMP->currentGridMap->clearMap();
						  if(COMP->ini.general.verbose == true)
						  {
							  std::cout << "MAPPER: autoupdate current map: clear map\n";
						  }
					  }

					  if (localState.curLtmState==0)
					  {
						  // don't use the ltm for preoccupation
						  if(COMP->ini.general.verbose == true)
						  {
							  std::cout << "MAPPER: autoupdate current map: no preoccupation\n";
						  }
					  }
					  else
					  {
						  // use ltm for preoccupation
						  COMP->LtmLock.acquire();


						  status = COMP->currentGridMap->setLtmOccupation(localState.curLtmThreshold,
	                                               *(COMP->ltmGridMap));

						  COMP->LtmLock.release();
						  if(COMP->ini.general.verbose == true)
						  {
							  std::cout << "MAPPER: autoupdate current map: ltm preoccupation\n";
						  }
					  }

					  // now update map with current laserscan
					  status = COMP->currentGridMap->update(laserscan);

					  // send it via autoupdate to subscribed clients
					  CommNavigationObjects::CommGridMap gm;
					  gm = (CommNavigationObjects::CommGridMap)(*(COMP->currentGridMap));
					  COMP->curPushServer->put( gm );
					  COMP->CurLock.release();

					  if(COMP->ini.general.verbose == true)
					  {
						  std::cout << "MAPPER: autoupdate current map: sent to subscribed clients\n";
					  }
				  }
				  else
				  {
					  // not yet the nth update cycle
					  if(COMP->ini.general.verbose == true)
					  {
						  std::cout << "MAPPER: autoupdate current map: No update, cycle number ";
						  std::cout << updateCounter << "\n";
					  }
				  }
			  } // if (statusCode == CHS::SMART_OK)
			  else
			  {
				    std::cout << "MAPPER: did not receive laser scan. " << CHS::StatusCodeConversion(statusCode) << std::endl;
			  }
		  } // if (statusCode == CHS::SMART_OK)

		  // -----------------------------------------------------
		  // unlock state at end of while loop
		  // -----------------------------------------------------
		  statusCode = COMP->stateServer->release("buildcurrentmap");

	  } // while
	  return 0;
} // svc current map class