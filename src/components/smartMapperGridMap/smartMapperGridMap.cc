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

#include "smartSoft.hh"
#include "commMobileLaserScan.hh"
#include "commMapperParameter.hh"
#include "commGridMap.hh"
#include "commGridMapRequest.hh"

#include "smartCurrentGridMap.hh"
#include "smartLtmGridMap.hh"

#include <iostream>
#include <sstream>

#define INI_PARAMETERS_FILE "smartMapperGridMap.ini"
#define DEBUGPRINT 0

class PrintStateChangeHandler;

// -----------------------------------------------------------------
// global data
// -----------------------------------------------------------------

struct MapperStateStruct {
  int      cellSize;         // [mm]
  int      growingType;      //
  int      curLtmState;      // [0,1]
  int      curLtmThreshold;  // [0,255]
  int      curEmpty;         // [0,1]
  int      curInterval;      // take every nth laserscan
  int      ltmInterval;      // msec
  int      ltmKalman;        // [0,255]
  char     *ltmFilename;     //
} globalState;


typedef struct GridMapRequestQueueEntry{
	CHS::QueryId id;
	Smart::CommGridMapRequest r;
};

CHS::MessageQueue<GridMapRequestQueueEntry> curMapRequestQueue;
CHS::SmartSemaphore curMapRequestSema(0);

CHS::SmartComponent *component;
CHS::PushNewestClient<Smart::CommMobileLaserScan> *laserClient;
CHS::SendServer<Smart::CommMapperParameter> *parameterServer;
CHS::PushNewestServer<Smart::CommGridMap> *currentGridMapServer;
CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap> *ltmGridMapQueryServer;
CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap> *curGridMapQueryServer;



PrintStateChangeHandler *stateHandler;
CHS::SmartStateServer *state;

Smart::SmartCurrentGridMap *currentGridMap;
Smart::SmartLtmGridMap     *ltmGridMap;

CHS::SmartMutex	curLock;
CHS::SmartMutex ltmLock;
CHS::SmartMutex mapperStateLock;

//<steck,lutz> ConditionVariable trigger
CHS::SmartMutex mapperLtmTriggerLock;
CHS::SmartConditionMutex mapperLtmTrigger(mapperLtmTriggerLock);


//
// handler of the send service of this component
//
class ParameterHandler : public CHS::SendServerHandler<Smart::CommMapperParameter>
{
public:
  void handleSend(const Smart::CommMapperParameter& cmd) throw()
  {
    Smart::MapperTagType tag;
    char                 filename[255];
    int                  p1,p2,p3,p4,p5,p6,p7;

    cmd.get(tag,p1,p2,p3,p4,p5);

    switch (tag) 
    {
      case Smart::MAPPER_CUR_PARAMETER:
      {
        // only allowed in deactivated mode
        // p1,p2 size x,y in mm, p3,p4 position x,y in mm,p5 mapId
        mapperStateLock.acquire();
        p6 = globalState.cellSize;
        p7 = globalState.growingType;
        mapperStateLock.release();
        curLock.acquire();
        delete currentGridMap;
        currentGridMap = new Smart::SmartCurrentGridMap(p1,p2,p3,p4,p6,p7,p5);
        curLock.release();
        std::cout << "MAPPER_CUR_PARAMETER new SmartCurrentGridMap created.\n"
                  << "                     Parameters: " << p1 << " " << p2 
                  << " " << p3 << " " << p4 << " " << p5 << " " << p6 << " " << p7 << " \n";
        break;
      }

      case Smart::MAPPER_CUR_LTM:
      {
        // allowed in running mode
        // p1 0/1 to disable/enable longterm map preoccupation
        // p2 threshold for longterm map obstacle
        mapperStateLock.acquire();
        globalState.curLtmState = p1;
        globalState.curLtmThreshold = p2;
        mapperStateLock.release();
        std::cout << "MAPPER_CUR_LTM: curLtmState = " << p1 << " curLtmThreshold = " << p2 << "\n";
        break;
      }

      case Smart::MAPPER_CUR_EMPTY:
      {
        // allowed in running mode
        // p1 0/1 to accumulate/empty current map
        mapperStateLock.acquire();
        globalState.curEmpty = p1;
        mapperStateLock.release();
        std::cout << "MAPPER_CUR_EMPTY: curEmpty = " << p1 << "\n";
        break;
      }

      case Smart::MAPPER_CUR_SAVE:
      {
        // allowed in running mode
        sprintf(filename,"test-cur-%d.xml",p1);
        curLock.acquire();
        std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
        currentGridMap->save_xml( myFile );
        myFile.close();
        curLock.release();
        std::cout << "MAPPER_CUR_SAVE: saved -  " << filename << "\n";
        break;
      }

      case Smart::MAPPER_CUR_SAVE_XPM:
      {
        // allowed in running mode
        sprintf(filename,"test-cur-%d.xpm",p1);
        curLock.acquire();
        std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
        currentGridMap->save_xpm( myFile );
        myFile.close();
        curLock.release();
        std::cout << "MAPPER_CUR_SAVE_XPM: saved -  " << filename << "\n";
        break;
      }

      case Smart::MAPPER_CUR_LOAD:
      {
        // only allowed in deactivated mode
        sprintf(filename,"test-cur-%d.xml",p1);
        curLock.acquire();
        std::ifstream myFile ( filename , std::ios::in | std::ios::binary);
        currentGridMap->load_xml( myFile );
        myFile.close();
        curLock.release();
        std::cout << "MAPPER_CUR_LOAD: loaded -  " << filename << "\n";
        break;
      }

      case Smart::MAPPER_CUR_LOAD_LTM:
      {
        // only allowed in deactivated mode
        curLock.acquire();
        currentGridMap->clearMap();
        currentGridMap->setLtmOccupation(globalState.curLtmThreshold,
                                         *ltmGridMap);
        curLock.release();
        std::cout << "MAPPER_CUR_LOAD_LTM: cur map built from ltm map -  threshold = " << globalState.curLtmThreshold << "\n";
        break;
      }


      case Smart::MAPPER_LTM_PARAMETER:
      {
        // only allowed in deactivated mode
        // p1,p2 size x,y in mm, p3,p4 position x,y in mm,p5 mapId
        mapperStateLock.acquire();
        p6 = globalState.cellSize;
        mapperStateLock.release();
        ltmLock.acquire();
        delete ltmGridMap;
        ltmGridMap = new Smart::SmartLtmGridMap(p1,p2,p3,p4,p6,p5);
        ltmLock.release();
        std::cout << "MAPPER_LTM_PARAMETER: new SmartLtmGridMap created.\n"  
                  << "                      Parameters: " << p1 << " " << p2 
                  << " " << p3 << " " << p4 << " " << p5 << " " << p6 << " \n";
        break;
      }

      case Smart::MAPPER_LTM_INITIALIZE:
      {
        // allowed in running mode
        // p1 contains the initializing value
        mapperStateLock.acquire();
        ltmGridMap->clearMap(p1);
        mapperStateLock.release();
        std::cout << "MAPPER_LTM_INITIALIZE: clearMap(" << p1 << ")\n";
        break;
      }

      case Smart::MAPPER_LTM_SAVE:
      {
        // allowed in running mode
        sprintf(filename,"test-ltm-%d.xml",p1);
        ltmLock.acquire();
        std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
        ltmGridMap->save_xml( myFile );
        myFile.close();
        ltmLock.release();
        std::cout << "MAPPER_LTM_SAVE: saved -  " << filename << "\n";
        break;
      }

      case Smart::MAPPER_LTM_SAVE_XPM:
      {
        // allowed in running mode
        sprintf(filename,"test-ltm-%d.xpm",p1);
        ltmLock.acquire();
        std::ofstream myFile ( filename , std::ios::out | std::ios::binary);
        ltmGridMap->save_xpm( myFile );
        myFile.close();
        ltmLock.release();
        std::cout << "MAPPER_LTM_SAVE_XPM: saved -  " << filename << "\n";
        break;
      }

      case Smart::MAPPER_LTM_LOAD:
      {
        // only allowed in deactivated mode
        sprintf(filename,"test-ltm-%d.xml",p1);
        ltmLock.acquire();
        std::ifstream myFile ( filename , std::ios::in | std::ios::binary);
        ltmGridMap->load_xml( myFile );
        myFile.close();
        ltmLock.release();
        std::cout << "MAPPER_LTM_LOAD: loaded -  " << filename << "\n";
        break;
      }
 
      default:
      {
        break;
      }

    } // switch
  } // handle
}; // class


/**
 * derive the state class and provide handler functions
 */
class PrintStateChangeHandler : public CHS::StateChangeHandler
{
public:
  void handleEnterState(const std::string & s) throw()
    {
      std::cout << "    enterHandler  activate   " << s << std::endl;
    };
  void handleQuitState(const std::string & s) throw()
    {
      std::cout << "    quitHandler   deactivate " << s << std::endl;
    };
};



/**
 *trigger Ltm class
 */
class TriggerLtmTimerCallback : public CHS::TimerHandler
{

public:

	// ---------------------------------------------	
	//used to trigger the ltm calculation
	// ---------------------------------------------

        virtual void timerExpired(const ACE_Time_Value & absolute_time, const void * arg){
	
        mapperLtmTriggerLock.acquire();
        mapperLtmTrigger.signal();
        mapperLtmTriggerLock.release();

	}
};

/**
 *ltm request handler class
 */
class LtmMapperQueryServerHandler :  public CHS::QueryServerHandler<Smart::CommGridMapRequest,Smart::CommGridMap>
{
public:
  void handleQuery(CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap> & server,
                     const CHS::QueryId id,
                     const Smart::CommGridMapRequest& r) throw()

    {
      // longterm map is sent even in deactivated module state
      std::cout << "LtmMapper QueryServer handleQuery id: " << id << std::endl;
      Smart::CommGridMap gm;
      ltmLock.acquire();
      gm = (Smart::CommGridMap)(*ltmGridMap);
      ltmLock.release();
      server.answer(id, gm);
    }
};


/**
 *current map request handler class
 */
class CurMapperQueryServerHandler :  public CHS::QueryServerHandler<Smart::CommGridMapRequest,Smart::CommGridMap>

{
public:

  void handleQuery(CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap> & server,
                     const CHS::QueryId id,
                     const Smart::CommGridMapRequest& r) throw()
    {
      GridMapRequestQueueEntry entry;
      
      entry.id = id;
      entry.r = r;

      curMapRequestQueue.enqueue(entry);
      curMapRequestSema.release();
    }

};


/**
 * current mapper class 
 */
class CurrentMapperThread : public CHS::SmartTask
{
public:
  CurrentMapperThread() {};
  ~CurrentMapperThread() {};
  int svc(void);
};


int CurrentMapperThread::svc(void)
{
  Smart::CommMobileLaserScan laserscan;

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
    statusCode = state->acquire("buildcurrentmap");   
    if (statusCode == CHS::SMART_OK) 
    {

      // current map autoupdate service is activated
      mapperStateLock.acquire();
      localState = globalState;
      mapperStateLock.release();

      // wait for scan (PushNewest)
      status = laserClient->getUpdateWait(laserscan);
      if (status == CHS::SMART_OK) 
      {
        // laserscan received
        updateCounter++;
        if (updateCounter>=globalState.curInterval) 
        {
          // 
          // generate current map and send it via autoupdate to clients
          // 
          updateCounter=0;
          #if DEBUGPRINT
            cout << "MAPPER: autoupdate current map: generate current map\n";
          #endif

          curLock.acquire();

          if (localState.curEmpty==0) 
          {
            // accumulating mode for current map
            // convert obstacle growing cells into normal occupied cells
            status = currentGridMap->convertObstacleGrowing();
            #if DEBUGPRINT
              cout << "MAPPER: autoupdate current map: accumulating mode\n";
            #endif
          } 
          else 
          {
            // delete current map for each cycle
            status = currentGridMap->clearMap();
            #if DEBUGPRINT
              cout << "MAPPER: autoupdate current map: clear map\n";
            #endif
          }

          if (localState.curLtmState==0) 
          {
            // don't use the ltm for preoccupation
            #if DEBUGPRINT
              cout << "MAPPER: autoupdate current map: no preoccupation\n";
            #endif
          } 
          else 
          {
            // use ltm for preoccupation
            ltmLock.acquire();

            status = currentGridMap->setLtmOccupation(localState.curLtmThreshold,
                                               *ltmGridMap);

            ltmLock.release();
            #if DEBUGPRINT
              cout << "MAPPER: autoupdate current map: ltm preoccupation\n";
            #endif
          }

          // now update map with current laserscan
          status = currentGridMap->update(laserscan);
  
          // send it via autoupdate to subscribed clients
          Smart::CommGridMap gm;
          gm = (Smart::CommGridMap)(*currentGridMap);
          currentGridMapServer->put( gm );
          curLock.release();

          #if DEBUGPRINT
            cout << "MAPPER: autoupdate current map: sent to subscribed clients\n";
          #endif
        } 
        else 
        {
          // not yet the nth update cycle
          #if DEBUGPRINT
            cout << "MAPPER: autoupdate current map: No update, cycle number ";
            cout << updateCounter << "\n";
          #endif
        }
      } // if (status == CHS::SMART_OK)
    } // if (statusCode == CHS::SMART_OK)

    // -----------------------------------------------------
    // unlock state at end of while loop
    // -----------------------------------------------------
    statusCode = state->release("buildcurrentmap");
     
  } // while
  return 0;
} // svc current map class


/**
 * longterm mapper class 
 */
class LtmMapperThread : public CHS::SmartTask
{
public:
  LtmMapperThread() {};
  ~LtmMapperThread() {};
  int svc(void);
};

int LtmMapperThread::svc(void)
{
  Smart::CommMobileLaserScan laserscan;

  struct MapperStateStruct localState;
  int                      status;
  CHS::StatusCode          statusCode;

  while(1)
  {
    // ------------------------------------------------
    // wait for activation
    // ("buildltmmap")
    // ------------------------------------------------
    statusCode = state->acquire("buildltmmap");
    if (statusCode == CHS::SMART_OK)
    {
      // wait for the next trigger
      mapperLtmTriggerLock.acquire();
      mapperLtmTrigger.wait();
      mapperLtmTriggerLock.release();


      // current map autoupdate service is activated
      mapperStateLock.acquire();
      localState = globalState;
      mapperStateLock.release();

      // wait for scan (PushNewest)
      status = laserClient->getUpdateWait(laserscan);
      if (status == CHS::SMART_OK)
      {
        ltmLock.acquire();
        ltmGridMap->update(laserscan, localState.ltmKalman);
        ltmLock.release();

        #if DEBUGPRINT
          cout << "MAPPER: Updated LTM\n";
        #endif

      } // if (status == CHS::SMART_OK)

    } // if (statusCode == CHS::SMART_OK)

    // -----------------------------------------------------
    // unlock state at end of while loop
    // -----------------------------------------------------
    statusCode = state->release("buildltmmap");

  } // while(1)
  return 0;
} // svc longterm map

class CurrentMapRequestThread: public CHS::SmartTask
{
public:
  CurrentMapRequestThread() {};
  ~CurrentMapRequestThread() {};
  int svc(void);
};

int CurrentMapRequestThread::svc(void)
{

  struct MapperStateStruct localState;
  Smart::CommMobileLaserScan laserscan;
  GridMapRequestQueueEntry   entry;

  Smart::CommGridMap gm;

  Smart::SmartCurrentGridMap *resultMap;  
  int                        offsetXmm,offsetYmm;
  unsigned int               sizeXmm,sizeYmm;
  unsigned int               curLtmThreshold,mapId;
  bool                       curLtmState;

  int                        status;

  CHS::StatusCode            stateValue;

  while (1) {
    // wait for new request signaled
    curMapRequestSema.acquire();
  
    // get request
    entry = curMapRequestQueue.dequeue();

    // process the request depending on the module state
    stateValue = state->tryAcquire("buildcurrentmap");
    
    if (stateValue == CHS::SMART_OK) {
      // module is active, therefore generate requested map
      mapperStateLock.acquire();
      localState=globalState;
      mapperStateLock.release();

      status = entry.r.get_parameter(mapId,offsetXmm,offsetYmm,sizeXmm,sizeYmm,
                                    curLtmState,curLtmThreshold);
      resultMap = new Smart::SmartCurrentGridMap(sizeXmm,sizeYmm,
                                     offsetXmm,offsetYmm,
                                     localState.cellSize,
                                     localState.growingType,
                                     mapId);

      #if DEBUGPRINT
        cout << "MAPPER: Request current Map: generate cur map\n";
        cout << "  sizeXmm sizeYmm      " << sizeXmm << " " << sizeYmm << "\n";
        cout << "  offsetXmm offsetYmm  " << offsetXmm << " " << offsetYmm << "\n";
        cout << "  cellSize             " << localState.cellSize << "\n";
        cout << "  growingType          " << localState.growingType << "\n";
        cout << "  mapId                " << mapId << "\n";
      #endif

      // wait for the next laserscan
      status = laserClient->getUpdateWait(laserscan);

      if (status==CHS::SMART_OK) {
        // received laser scan
        // delete actual map for each cycle
        status = resultMap->clearMap();

        if (curLtmState==0) {
          // don't use the ltm for preoccupation
        } else {
          // use ltm for preoccupation
          ltmLock.acquire();
          status = resultMap->setLtmOccupation(localState.curLtmThreshold,*ltmGridMap);
          ltmLock.release();
        }

        // now update map with current laserscan
        status = resultMap->update(laserscan);
        gm = (Smart::CommGridMap)(*resultMap);
      } else {
        // canceled laser scan query, no current map available
        gm = (Smart::CommGridMap)(*resultMap);
      }

      std::cout<<"MAPPER: Request currentmap module is active"<<endl;
    } else {

      // module is not active
      resultMap = new Smart::SmartCurrentGridMap();
      gm = (Smart::CommGridMap)(*resultMap);

      std::cout<<"MAPPER: Request currentmap module is not active"<<endl;
    }
     curGridMapQueryServer->answer(entry.id,gm);

    // delete the map of the current request
    delete resultMap;

    // check whether one has to unlock state
    if (stateValue == CHS::SMART_OK) {
      // got state, therefore release
      state->release("buildcurrentmap");
    }
  }// end while
  return 0;
}




/**
 * Main function.
 * \param argc arguments counter.
 * \param argv arguments vector.
 */
int main (int argc, char *argv[])
{
  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();
    CHS::SmartParameter parameter;
    char *growing;
    int p1,p2,p3,p4,p5;
    struct MapperStateStruct localState;

    // read parameters
    try
    {
      parameter.addFile(argc, argv, "ini");
    } 
    catch(CHS::ParameterError e) 
    {
      try  
      {
        parameter.addFile(INI_PARAMETERS_FILE);
      } 
      catch(CHS::ParameterError e) 
      {
        std::cerr << "No parameter file found." << std::endl;
        return 1;
      }
    }
    mapperStateLock.acquire();
    globalState.cellSize        = (int)(parameter.getDouble("map","cellsize"));
    globalState.curInterval     = (int)(parameter.getDouble("current","interval"));
    globalState.curLtmState     = (int)(parameter.getDouble("current","ltm_state"));
    globalState.curLtmThreshold = (int)(parameter.getDouble("current","ltm_threshold"));
    globalState.curEmpty        = (int)(parameter.getDouble("current","empty"));
    globalState.ltmInterval     = (int)(parameter.getDouble("ltm","interval"));
    globalState.ltmKalman       = (int)(parameter.getDouble("ltm","kalman"));

    localState = globalState;

    //globalState.ltmFilename     = (char *)parameter.getString("ltm","filename").c_str();

    growing = (char *)parameter.getString("current","growing").c_str();
    globalState.growingType = MAPPER_GROWING_NO;
    if (strcmp(growing,"circle16")==0) globalState.growingType = MAPPER_GROWING_CIRCLE_16;
    if (strcmp(growing,"circle8")==0)  globalState.growingType = MAPPER_GROWING_CIRCLE_8;
    if (strcmp(growing,"star16")==0)   globalState.growingType = MAPPER_GROWING_STAR_16;
    if (strcmp(growing,"star12")==0)   globalState.growingType = MAPPER_GROWING_STAR_12;
    if (strcmp(growing,"no")==0)       globalState.growingType = MAPPER_GROWING_NO;

    p1 = (int)(parameter.getDouble("current","size_x"));
    p2 = (int)(parameter.getDouble("current","size_y"));
    p3 = (int)(parameter.getDouble("current","offset_x"));
    p4 = (int)(parameter.getDouble("current","offset_y"));
    p5 = (int)(parameter.getDouble("current","map_id"));

    // create current map
    currentGridMap = new Smart::SmartCurrentGridMap( p1, p2, p3, p4,
                                                     globalState.cellSize,
                                                     globalState.growingType,
                                                     p5);

    p1 = (int)(parameter.getDouble("ltm","size_x"));
    p2 = (int)(parameter.getDouble("ltm","size_y"));
    p3 = (int)(parameter.getDouble("ltm","offset_x"));
    p4 = (int)(parameter.getDouble("ltm","offset_y"));
    p5 = (int)(parameter.getDouble("ltm","map_id"));

    // create longterm map
    ltmGridMap = new Smart::SmartLtmGridMap( p1, p2, p3, p4,
                                             globalState.cellSize,
                                             p5);
 

    mapperStateLock.release();


    // component
    component = new CHS::SmartComponent("smartMapper",argc,argv);

    ParameterHandler sendParameterHandler;

    // laserClient 
    laserClient = new CHS::PushNewestClient<Smart::CommMobileLaserScan>(component);

    // parameterServer
    parameterServer = new CHS::SendServer<Smart::CommMapperParameter>(component, "mapperParameter", sendParameterHandler);

    // connect laserClient
    laserClient->connect("smartLaserServer","scan");
    laserClient->subscribe();

    // server for current grid map
    currentGridMapServer = new CHS::PushNewestServer<Smart::CommGridMap>(component, "currentGridMap");

    // query server for longterm map 
    LtmMapperQueryServerHandler ltmMapperQueryServerHandler;
    ltmGridMapQueryServer = new CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap>(component,"ltmGridMapQuery",ltmMapperQueryServerHandler);

    
    // query server for current map 
    CurMapperQueryServerHandler curMapperQueryServerHandler;
    curGridMapQueryServer = new CHS::QueryServer<Smart::CommGridMapRequest,Smart::CommGridMap>(component,"curGridMapQuery",curMapperQueryServerHandler);

    // state
    stateHandler = new PrintStateChangeHandler();
    state = new CHS::SmartStateServer(component,*stateHandler);

    if (state->defineStates("buildbothmaps"  ,"buildcurrentmap")  != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("buildbothmaps"  ,"buildltmmap")      != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("buildcurrentmap","buildcurrentmap")  != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("buildltmmap"    ,"buildltmmap")      != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;

    if (state->activate() != CHS::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;


    // timer triggers ltm calculation
    // ACE_Time_Value(seconds, microseconds) values from ltmInterval in ms
    TriggerLtmTimerCallback periodicTrigger;	
    component->getTimerThread().scheduleTimer(periodicTrigger, (void*) 0, ACE_Time_Value(0,0), 
                                              ACE_Time_Value(localState.ltmInterval/1000,(localState.ltmInterval%1000)*1000));


    // thread for building the current map
    // it will use the currentGridMapServer to push the newest map
    CurrentMapperThread currentMapperThread;
    currentMapperThread.open();

    // thread for building the longterm map
    LtmMapperThread ltmMapperThread;
    ltmMapperThread.open();

    

    CurrentMapRequestThread currentMapRequestThread;
    currentMapRequestThread.open();


    std::cout << "start...\n";

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

