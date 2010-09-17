// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Christian Schlegel, Andreas Steck, Matthias Lutz
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
//  This file is part of the "SmartSoft smartPlannerBreadthFirstSearch component".
//  It provides planning services based on grid maps.
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
#include "commPlannerParameter.hh"
#include "commGridMap.hh"
#include "commPlannerGoal.hh"
#include "commPlannerEvent.hh"
#include "smartPlannerMap.hh"

#include <iostream>
#include <sstream>

#define DEBUGPRINT 0


class PrintStateChangeHandler;

// Necessary dummy that libsmartBasicComm will be linked.
// Problem is that the linkt to smartTimeStamp.idl in smartGridMapComm leads to smartBasicComm
// removing this will cause strange problems
//Smart::CommTimeStamp dummy;

// -----------------------------------------------------------------
// global data
// -----------------------------------------------------------------
CHS::SmartMutex plannerGoalLock;
CHS::SmartMutex plannerGlobalLock;


// ConditionVariable trigger
CHS::SmartMutex plannerTriggerLock;
CHS::SmartConditionMutex plannerTrigger(plannerTriggerLock);


GoalFifoStructPtr goalFifoHead;
GoalFifoStructPtr goalFifoTail;

struct PlannerStateStruct {
  double         trigger;
  int            id;
} globalState,localState;


CHS::SmartComponent *component;

CHS::PushNewestClient<Smart::CommGridMap> *curMapClient;
CHS::PushTimedClient<Smart::CommBaseState> *base;
CHS::SendServer<Smart::CommPlannerParameter> *parameterServer;
CHS::PushNewestServer<Smart::CommPlannerGoal> *plannerUpdateNewestServer;

Smart::PlannerEventState plannerEventState;
CHS::EventServer<Smart::CommPlannerEventParameter,Smart::CommPlannerEventResult,Smart::PlannerEventState> *plannerEventServer;

PrintStateChangeHandler *stateHandler;
CHS::SmartStateServer *state;

struct timeval currentTime;
struct timeval beginNoPathTime;
int            noPathFlag;



//
// handler of the send service of this component
//
//
class ParameterHandler : public CHS::SendServerHandler<Smart::CommPlannerParameter>
{
public:
  void handleSend(const Smart::CommPlannerParameter& cmd) throw()
    {
      int status;
      Smart::PlannerTagType tag;
      int parameter1;
      int parameter2;
      int parameter3;
      int parameter4;
      cmd.get(tag,parameter1,parameter2,parameter3,parameter4);

  switch (tag) {
    case Smart::PLANNER_DELETE_GOAL:
      plannerGoalLock.acquire();
      status = goalFifoFree(goalFifoHead,goalFifoTail);
      plannerGoalLock.release();
      std::cout << "PLANNER_DELETE_GOAL smartPlannerGoal deleted.\n";
      break;
    case Smart::PLANNER_SET_DESTINATION_LINE:
      plannerGoalLock.acquire();
      status = goalFifoWrite(goalFifoHead,goalFifoTail,PLANNER_LINE,
                             parameter1,parameter2,parameter3,parameter4);
      plannerGoalLock.release();
      std::cout << "PLANNER_SET_DESTINATION_LINE new smartPlannerGoal destination line created.\n"
                  << "                     Parameters: " << parameter1 << " " << parameter2
                  << " " << parameter3 << " " << parameter4 <<" \n";
      break;
    case Smart::PLANNER_SET_DESTINATION_CIRCLE:
      plannerGoalLock.acquire();
      status = goalFifoWrite(goalFifoHead,goalFifoTail,PLANNER_CIRCLE,
                             parameter1,parameter2,parameter3,0.0);
      plannerGoalLock.release();
      std::cout << "PLANNER_SET_DESTINATION_CIRCLE new smartPlannerGoal destination circle created.\n"
                  << "                     Parameters: " << parameter1 << " " << parameter2
                  << " " << parameter3 << " " <<" \n";
      break;
    case Smart::PLANNER_ID:
      plannerGlobalLock.acquire();
      globalState.id = (int)parameter1;
      plannerGlobalLock.release();
      std::cout << "PLANNER_ID smartPlanner ID set.\n"
                  << "                     Parameters: " << parameter1 <<" \n";

      break;
    default:
      break;
   }
  }
};



// ------------------------------------------------------------------
// separat task to do path planning
// ------------------------------------------------------------------


class PlannerBreadthFirstSearchThread : public CHS::SmartTask
{
public:
  PlannerBreadthFirstSearchThread(){};
  ~PlannerBreadthFirstSearchThread(){};
  int svc(void);
};

int PlannerBreadthFirstSearchThread::svc(void)
{
  GoalFifoStructPtr goalFifoPtr;

  Smart::CommBaseState base_state;
  Smart::CommGridMap currentGridMap;
  Smart::CommPlannerGoal plannerGoal;
  
  int robotX,robotY,robotZ,robotA;    // current position of the robot
  
  double x1g,y1g,x2g,y2g;
  double xNextGoal, yNextGoal;        // intermediate goal point for the next step
  double xGoal, yGoal;                // final goal point which has to be reached
  int    type;
  CHS::StatusCode status;
  int    generalstatus;
  int status2;
  double timeDiff;
 

  bool           aStatus;
  int            aXOffsetCells,aYOffsetCells,aXOffsetMM,aYOffsetMM;
  unsigned int   aXSizeCells,aYSizeCells,aXSizeMM,aYSizeMM,aCellSizeMM;
  unsigned int   aMapId;
  struct timeval aTime;

 
  Smart::PlannerMapClass   *plannerMap;
  plannerMap = new Smart::PlannerMapClass(0, // axsizecells,
                                   0, // aysizecells,
                                   0, // acellsizemm,
                                   0, // axoffsetmm,
                                   0); // ayoffsetmm);



  // --------------------------------------------------------------------
  // now start processing
  // --------------------------------------------------------------------
  while(1)
  {
    // ------------------------------------------------
    // wait for activation
    // ("pathplanning")
    // ------------------------------------------------
    status = state->acquire("pathplanning");
    std::cout<<"State active in processing-loop: "<< status <<endl;

    // wait for the next timed trigger
    plannerTriggerLock.acquire();
    plannerTrigger.wait();
    plannerTriggerLock.release();

    // ------------------------------------------------------------------
    // access the global configuration information
    // ------------------------------------------------------------------
    plannerGlobalLock.acquire();
    localState = globalState;
    plannerGlobalLock.release();

    // ------------------------------------------------------------------
    // make sure the fifo for breadth first search is empty
    // ------------------------------------------------------------------
    status2        = fifoFree();
    generalstatus = PLANNER_NO_ERROR;

    // ------------------------------------------------------------------
    // get next current map and put it into the planner map class
    // ------------------------------------------------------------------

//std::cout << "before acquiring curMapClient - line " << __LINE__ << std::endl;
    status = curMapClient->getUpdate(currentGridMap);
//std::cout << "after acquiring curMapClient - line " << __LINE__ << std::endl;

    currentGridMap.get_parameter(aMapId,aStatus,aTime,aXOffsetMM,aYOffsetMM,aXOffsetCells,aYOffsetCells,aCellSizeMM,aXSizeMM,aYSizeMM,aXSizeCells,aYSizeCells);
//std::cout << "after getParameter - line " << __LINE__ << std::endl;

    if(localState.id != aMapId){
      // map id not vaild
      generalstatus = PLANNER_WRONG_MAPID; 
      std::cout << "PLANNER: map id not correct, do nothing (planner/map :" << localState.id 
           << " " << aMapId << "\n";
    } else {

      status2 = plannerMap->setMapperMap(currentGridMap);
      if (status2 != 0) {
         std::cout << "PLANNER: fatal error, can't allocate required map memory" << std::endl;
         exit(0);
      }

      // ----------------------------------------------------------------
      // convert from mapper to planner cell values
      // ---------------------------------------------------------------- 
      status2 = plannerMap->convert();
//std::cout << "after map convert() - line " << __LINE__ << std::endl;
     
      // ----------------------------------------------------------------
      // mark goal region in the map
      // ----------------------------------------------------------------
      plannerGoalLock.acquire();
      goalFifoPtr = goalFifoHead;
      status2 = goalFifoNext(goalFifoHead,goalFifoTail,&goalFifoPtr,&type,&x1g,&y1g,&x2g,&y2g);
      plannerGoalLock.release();
//std::cout << "after goal next fifo - line " << __LINE__ << std::endl;

      if (status2 != 0) {
        // --------------------------------------------------------------
        // there are no goal lines available
        // --------------------------------------------------------------
        generalstatus = PLANNER_NO_GOAL_AVAILABLE;
      } else {
        generalstatus = PLANNER_GOAL_NOT_MARKED;
        while (status2==0) {
          switch (type) {
            case PLANNER_LINE:
//std::cout << "before bresenham - line " << __LINE__ << std::endl;
              status2 = plannerMap->bresenham(x1g,y1g,x2g,y2g,MODE_GOAL);
//std::cout << "after bresenham - line " << __LINE__ << std::endl;
              break;
            case PLANNER_CIRCLE:
//std::cout << "before circle - line " << __LINE__ << std::endl;
//std::cout << "line: " << __LINE__ << "  plannerMap->circle("<<x1g<<", "<<y1g<<", "<<x2g<<", "<<MODE_GOAL<<")\n";
              status2 = plannerMap->circle(x1g,y1g,x2g,MODE_GOAL);
//std::cout << "after circle - line " << __LINE__ << std::endl;
              break;
            default:
              status2 = 1;
              break;
          }

          if (status2 == 0) {
            generalstatus = PLANNER_GOAL_OK;
          }
//std::cout << "before plannerGoalLock.acquire() - line " << __LINE__ << std::endl;
          plannerGoalLock.acquire();
//std::cout << "before goalFifoNext - line " << __LINE__ << std::endl;
          status2 = goalFifoNext(goalFifoHead,goalFifoTail,&goalFifoPtr,&type,&x1g,&y1g,&x2g,&y2g);
          plannerGoalLock.release();
//std::cout << "after goal next fifo - line " << __LINE__ << std::endl;

        }//while(status==0)
        if (generalstatus != PLANNER_GOAL_OK) {
          // ------------------------------------------------------------
          // had some problems to mark the goal region
          // generalstatus is PLANNER_GOAL_NOT_MARKED
          // ------------------------------------------------------------
          std::cout << "PLANNER: fatal error, problems marking the goal cells" << std::endl;
        } else {
          // ------------------------------------------------------------
          // successfully marked the goal cells in the map
          // ------------------------------------------------------------

          // ------------------------------------------------------------
          // get roboter position from base server
          // ------------------------------------------------------------
          status = base->getUpdate(base_state);
          if(status!=CHS::SMART_OK) {
            std::cerr << "WARNING: failed to get current base state (" << CHS::StatusCodeConversion(status) << "), pushing invalid scan" << std::endl;
          }

          robotX = base_state.get_base_position().get_x();
          robotY = base_state.get_base_position().get_y();
          robotA = base_state.get_base_position().get_base_alpha();


          // ROBOTERPOSITION
          status2 = plannerMap->markCurrentPosition(robotX, robotY);
          if (status2 != 0) {
            switch (status2) {
              case 1:
                generalstatus = PLANNER_START_OCCUPIED_OBSTACLE;
                std::cout << "PLANNER: fatal error, current robot position already occupied by an obstacle" << std::endl;
                break;
              case 2:
                generalstatus = PLANNER_START_OCCUPIED_GOAL;
                std::cout << "PLANNER: fatal error, current robot position already marked as goal" << std::endl;
                break;
              default:
                generalstatus = PLANNER_UNKNOWN_ERROR;
                std::cout << "PLANNER: fatal error, unknown error code" << std::endl;
                break;
            }
          } else {
            // ------------------------------------------------------------
            // no error occured during making the current position
            // ------------------------------------------------------------
//std::cout << "before wavefrontflood - line " << __LINE__ << std::endl;
            status2 = plannerMap->waveFrontFlood();
//std::cout << "after wavefrontflood - line " << __LINE__ << std::endl;
            if (status2 != 0) {
              // ------------------------------------------------------------
              // was not able to find a path, no valid goal point
              // ------------------------------------------------------------
              generalstatus = PLANNER_NO_PATH;
              std::cout << "PLANNER: fatal error, no path found since no valid goal point" << std::endl;

	      plannerGoal.set_goal(0.0,0.0,0.0,0.0,0.0,0.0,localState.id,1);
              plannerUpdateNewestServer->put(plannerGoal);

            } else {
              // ------------------------------------------------------------
              // found path, calculate next way point
              // ------------------------------------------------------------
              generalstatus = PLANNER_PATH_FOUND;
              //std::cout << "PLANNER befor waveFrontOptimizeFirstSegment - line " << __LINE__ << std::endl;
              status2 = plannerMap->waveFrontOptimizeFirstSegment(robotX,robotY,xNextGoal,yNextGoal);
              //std::cout << "PLANNER before waveFrontFindGoal - line " << __LINE__ << std::endl;
              status2 = plannerMap->waveFrontFindGoal(robotX,robotY,xGoal,yGoal);
              //std::cout << "PLANNER Robot position pos ("<<robotX << ","<<robotY<<")"<<std::endl;
              std::cout << "PLANNER  next goal (" << xNextGoal << "," << yNextGoal << ") goal (" << xGoal << "," << yGoal << ")" << std::endl;
           
              plannerGoal.set_goal(xNextGoal,yNextGoal,0.0,xGoal,yGoal,0.0,localState.id,0);
              plannerUpdateNewestServer->put(plannerGoal);

              generalstatus = PLANNER_NO_ERROR;
            }
          } //else if (status2 != 0)
        } //else if (generalstatus != PLANNER_GOAL_OK)
      } //else if (status2 != 0)
    } // else  if(localState.id != aMapId)

   // ------------------------------------------------------------
   // report on status
   // ------------------------------------------------------------
   switch (generalstatus) {
     case PLANNER_NO_ERROR:
       std::cout << "PLANNER: ok" << std::endl;
       break;
     case PLANNER_UNKNOWN_ERROR:
       std::cout << "PLANNER: unknown error" << std::endl;
       exit(0);
       break;
     case PLANNER_NO_GOAL_AVAILABLE:
       std::cout << "PLANNER: no goal available" << std::endl;
       break;
     case PLANNER_GOAL_OK:
       std::cout << "PLANNER: goal ok" << std::endl;
       break;
     case PLANNER_GOAL_NOT_MARKED:
       std::cout << "PLANNER: goal not marked" << std::endl;
       break;
     case PLANNER_START_OCCUPIED_OBSTACLE:
       std::cout << "PLANNER: start cell occupied by an obstacle" << std::endl;
       break;
     case PLANNER_START_OCCUPIED_GOAL:
       std::cout << "PLANNER: start cell occupied by goal" << std::endl;
       break;
     case PLANNER_NO_PATH:
       std::cout << "PLANNER: no path" << std::endl;
       break;
     case PLANNER_PATH_FOUND:
       std::cout << "PLANNER: path found" << std::endl;
       break;
     case PLANNER_WRONG_MAPID:
       std::cout << "PLANNER: wrong map id" << std::endl;
       break;
     default:
       std::cout << "PLANNER: default (unknown error type)" << std::endl;
     exit(0);
       break;
   } //switch (generalstatus)
   
   // -------------------------------------------------------------
   // now check whether generalstatus should be reported via event
   // -------------------------------------------------------------
   
   if (generalstatus == PLANNER_NO_PATH) {
     //
     // special treatment for "no path event" since this fires only
     // when it lasts for at least 5 seconds
     //
     if (noPathFlag == 0) {
       // first occurence of noPath situation
       noPathFlag = 1;
       gettimeofday(&beginNoPathTime,0);
     } else {
       // not the first occurence of noPath situation
       gettimeofday(&currentTime,0);
       timeDiff  = (double)(currentTime.tv_usec - beginNoPathTime.tv_usec)/1000000.0;
       timeDiff += (double)(currentTime.tv_sec  - beginNoPathTime.tv_sec);

       if (timeDiff > 5.0) {
         noPathFlag = 0;

         // ---------------------------------------------------
         // put event into object
         // ---------------------------------------------------
         plannerEventState.set(PLANNER_NO_PATH);
         plannerEventServer->put(plannerEventState);
       }
     }
   } else {
     plannerEventState.set(generalstatus);
     plannerEventServer->put(plannerEventState);
   }
   
   // -------------------------------------------------------------
   // unlock state at end of while loop
   // -------------------------------------------------------------
   status = state->release("pathplanning");


   #if DEBUGSAVE
     sprintf(filename,"plamap%d.xpm",filecounter++);
     map->saveXPM(filename);
   #endif

   #if DEBUGMODE
     gettimeofday(&timeEnd,(struct timezone*)NULL);
     subTime(timeEnd,timeStart);

     cout << "Needed path planning time: " << timeEnd.tv_sec << " " << timeEnd.tv_usec << "\n";

     flag = 0;

     do {
       cout << "One cycle in debug mode\n";
       cout << "-1- continue\n";
       cin >> input;
       switch(input) {
         case 1:
           flag = 1;
           break;
         default:
           cout << "Wrong input\n";
           break;
       }
     } while(flag==0);
   #endif

  /////////////////////////////////////////////////////////
  // Save planner map for debug purposes
  ///////////////////////////////////////////////////////// 
  char                 filename2[255];
  sprintf(filename2,"test-planner-map-%d.xpm",2);
  std::ofstream myFile2 ( filename2 , std::ios::out | std::ios::binary);
  plannerMap->save_xpm( myFile2 );
  myFile2.close();
  ////////////////////////////////////////////////////////


 }// while(1)
  return 0;
} // 

/**
 *trigger Planner class
 */
class TriggerPlannerTimerCallback : public CHS::TimerHandler
{

public:

        // ---------------------------------------------
        //used to trigger the Planner calculation
        // ---------------------------------------------

        virtual void timerExpired(const ACE_Time_Value & absolute_time, const void * arg){

        plannerTriggerLock.acquire();
        plannerTrigger.signal();
        plannerTriggerLock.release();

        }
};


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



class PlannerTestHandler:
  public CHS::EventTestHandler<Smart::CommPlannerEventParameter, Smart::CommPlannerEventResult, Smart::PlannerEventState>
{
public:
  bool testEvent(Smart::CommPlannerEventParameter& p, Smart::CommPlannerEventResult& r,const Smart::PlannerEventState& s) throw()
  {

   int oldState,newState;

   p.get(oldState);
   s.get(newState);
   if (oldState==newState) 
   {
     return false;
   } 
   else 
   {
     p.set(newState);
     r.set(newState);
     return true;
   }

    return false;
  }
};




/**
 * Main function.
 * \param argc arguments counter.
 * \param argv arguments vector.
 */
int main (int argc, char *argv[])
{
  try 
  {
    int status;
    status = goalFifoInit(&goalFifoHead,&goalFifoTail);

    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();


    // component
    component = new CHS::SmartComponent("smartPlanner",argc,argv);



    // PushNewestClient receiving current map fom smartMapper
    curMapClient = new CHS::PushNewestClient<Smart::CommGridMap>(component);

    std::cout << "connect status    : " <<
     CHS::StatusCodeConversion(curMapClient->connect("smartMapper","currentGridMap")) << std::endl;
    std::cout << "subscribe  status : " <<
     CHS::StatusCodeConversion(curMapClient->subscribe()) << std::endl;


    // PushTimedClient receiving base state fom smartPioneerBaseServer
    base = new CHS::PushTimedClient<Smart::CommBaseState>(component, "smartPioneerBaseServer", "basestate");
     CHS::StatusCode status1 = base->subscribe(1);
    if(status1!=CHS::SMART_OK)
    {
      std::cerr << "ERROR: failed to connect to base state service "<< std::endl;
      std::cerr << "ERROR: (" << CHS::StatusCodeConversion(status1) << ")" << std::endl;
    }

    // server for planner parameter
    ParameterHandler sendParameterHandler;
    parameterServer = new CHS::SendServer<Smart::CommPlannerParameter>(component, "plannerParameter",sendParameterHandler);

    // server for planner goal
    plannerUpdateNewestServer = new CHS::PushNewestServer<Smart::CommPlannerGoal>(component, "plannerGoal");


    // planner Event Server
    PlannerTestHandler plannerTest;
    plannerEventServer = new CHS::EventServer<Smart::CommPlannerEventParameter,Smart::CommPlannerEventResult,Smart::PlannerEventState>(component, "plannerEvent",plannerTest); 

    // state
    stateHandler = new PrintStateChangeHandler();
    state = new CHS::SmartStateServer(component,*stateHandler);

    if (state->defineStates("pathplanning"  ,"pathplanning")  != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    
    if (state->activate() != CHS::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;


    // timer triggers planner calculation
    // ACE_Time_Value(seconds, microseconds) values from plannerInterval in ms
    TriggerPlannerTimerCallback periodicTrigger;
    // trigger planner calculation every: 1sec
    localState.trigger= 1000;
    component->getTimerThread().scheduleTimer(periodicTrigger, (void*) 0, ACE_Time_Value(0,0),
                                              ACE_Time_Value((int)localState.trigger/1000,((int)localState.trigger%1000)*1000));


    // thread for building the longterm map
    PlannerBreadthFirstSearchThread plannerBreadthFirstSearchThread;
    plannerBreadthFirstSearchThread.open();

    
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

