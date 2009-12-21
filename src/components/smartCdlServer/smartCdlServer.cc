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
//  This file is part of the "SmartSoft CDL component".
//  It provides navigation services based on the CDL
//  Curvature Distance Lookup approach.
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

#include <stdio.h>
#include <unistd.h>     
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "smartSoft.hh"
#include "commNavigationVelocity.hh"
#include "commMobileLaserScan.hh"
#include "commCdlParameter.hh"
#include "commPlannerGoal.hh"
#include "commCdlGoalEvent.hh"
#include "commTrackingGoal.hh"
#include "smartCdlLookup.hh"

#define INI_PARAMETERS_FILE "smartCdlServer.ini"

class PrintStateChangeHandler;


// ------------------------------------------------------------
// global data
// ------------------------------------------------------------
struct CdlStateStruct {
  Smart::CdlTagType strategy;             // strategy used to select v,w
  Smart::CdlTagType goalSpec;             // where to get the goal specification
  Smart::CdlTagType lookupTable;          // indicates which lookup table to use
  Smart::CdlTagType freeBehavior;         // indicates whether to activate free behavior

  bool   plannerClientSubscribed;  // These flags indicate whether that

  bool   plannerInit;              // findServer : planner
  bool   joystickInit;             // findServer : joystick

  int    id;                       // used for synchronization

//  double ignoreCircleX;            // ignore all laser scan points which are
//  double ignoreCircleY;            // inside that circle. If radius is less
//  double ignoreCircleR;            // or equal to zero, no points are discarded.

  double approachDistance;         // distance to stop
  double rotateError;              // angular error to stop rotation in place

  double transAcc;                 // translational acceleration
  double rotAcc;                   // rotational acceleration
  double tcalc;                    // time step prediction into future
  double ttrigger;                 // time step execution cycle
  double vmin;                     // current run: minimum allowed velocity
  double vmax;                     // current run: maximum allowed velocity
  double wmin;                     // current run: minimum allowed velocity
  double wmax;                     // current run: maximum allowed velocity

  double goalX;                    // x coordinate of goal
  double goalY;                    // y coordinate of goal
  double goalA;                    // a coordinate of goal

} globalState,localState;

CHS::SmartMutex cdlGlobalLock;

CdlLookupClass cdlLookupDefault;
CdlLookupClass cdlLookupSecond;
CdlLookupClass *cdlLookup;

CHS::SmartMutex cdlTriggerLock;
CHS::SmartConditionMutex cdlTrigger(cdlTriggerLock);

// TODO
//CdlCircleClass cdlCircle;
//CdlCdlEventStateClass cdlEventState;

Smart::CdlGoalEventState cdlGoalEventState;
CHS::EventServer<Smart::CommCdlGoalEventParameter,Smart::CommCdlGoalEventResult,Smart::CdlGoalEventState>
*cdlGoalEventServer;



CHS::SmartComponent *component;
CHS::PushNewestClient<Smart::CommMobileLaserScan> *laserClient;
CHS::PushNewestClient<Smart::CommPlannerGoal> *plannerClient;
CHS::SendClient<Smart::CommNavigationVelocity> *navigationVelocitySendClient;

CHS::SendServer<Smart::CommCdlParameter> *parameterServer;
CHS::SendServer<Smart::CommNavigationVelocity> *navigationVelocitySendServer; // for Joystick input

CHS::PushNewestClient<Smart::CommTrackingGoal> *trackingClient;

PrintStateChangeHandler *stateHandler;
CHS::SmartStateServer *state;

char *curvatureDefaultFile;
char *lookupDefaultFile;
char *accelDefaultFile;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
double angle00(double a) 
{
  if (a < -M_PI || a >= M_PI) 
  {
    a = a - 2*M_PI * floor(a / (2*M_PI));
    while(a < -M_PI) a += 2*M_PI;
    while(a >= M_PI) a -= 2*M_PI;
  }

  return(a);
}


// -------------------------------------------------------------
// derive the state class and provide handler functions
// -------------------------------------------------------------
class PrintStateChangeHandler : public CHS::StateChangeHandler
{
public:
  void handleEnterState(const std::string & s) throw()
    {
      std::cout << "    enterHandler  activate   " << s << std::endl;
      // TODO subscribe/unsubscribe vision/planner/track/cylinder
    };
  void handleQuitState(const std::string & s) throw()
    {
      std::cout << "    quitHandler   deactivate " << s << std::endl;
      // TODO unsubscribe vision/planner/track/cylinder


      // avoid emergency stop -> if cdl sends no more navigationVelocity-Object to the base,
      // the watchdog of the base (AROS) will arise and thus doing an emergency stop.
      Smart::CommNavigationVelocity vel;
      vel.set_v(0, 0.001);
      vel.set_omega(0);
      navigationVelocitySendClient->send(vel);
    };
};


//
// handler of the send service of this component
//
class ParameterHandler : public CHS::SendServerHandler<Smart::CommCdlParameter>
{
public:
  void handleSend(const Smart::CommCdlParameter& cmd) throw()
  {
    Smart::CdlTagType tag;
    //char                 filename[255];
    int                  p1,p2,p3,p4,p5;

    cmd.get(tag,p1,p2,p3,p4,p5);

    switch (tag)
    {
      ///////////////////////////////////////////////////////////////////// 
      // CDL_SET_MODE_STRATEGY
      case Smart::CDL_SET_MODE_STRATEGY:
      {
        cdlGlobalLock.acquire();
        globalState.strategy = (Smart::CdlTagType)p1;
        cdlGlobalLock.release();
        std::cout << "CDL_SET_MODE_STRATEGY: ";
        if( (Smart::CdlTagType)p1 == Smart::CDL_REACTIVE )
        {
          std::cout << "CDL_REACTIVE selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_JOYSTICK )
        {
          std::cout << "CDL_JOYSTICK selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_TURN )
        {
          std::cout << "CDL_TURN selected\n";
        }
	else if( (Smart::CdlTagType)p1 == Smart::CDL_APPROACH_HALT )
        {
          std::cout << "CDL_APPROACH_HALT selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_ROTATE )
        {
          std::cout << "CDL_ROTATE selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_FOLLOW )
        {
          std::cout << "CDL_FOLLOW selected\n";
        }
        else
        {
          std::cout << "unsupported parameter\n";
        }


        break;
      }


      ///////////////////////////////////////////////////////////////////// 
      // CDL_SET_FREE_BEHAVIOR
      case Smart::CDL_SET_FREE_BEHAVIOR:
      {
        cdlGlobalLock.acquire();
        globalState.freeBehavior = (Smart::CdlTagType)p1;
        cdlGlobalLock.release();
        std::cout << "CDL_SET_FREE_BEHAVIOR: ";
        if( (Smart::CdlTagType)p1 == Smart::CDL_NO_FREE_BEHAVIOR )
        {
          std::cout << "CDL_NO_FREE_BEHAVIOR selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_FREE_BEHAVIOR )
        {
          std::cout << "CDL_FREE_BEHAVIOR selected\n";
        }
        else
        {
          std::cout << "unsupported parameter\n";
        }

        break;
      }


      ///////////////////////////////////////////////////////////////////// 
      // CDL_SET_LOOKUP_TABLE
      case Smart::CDL_SET_LOOKUP_TABLE:
      {
        cdlGlobalLock.acquire();
        globalState.lookupTable = (Smart::CdlTagType)p1;
        cdlGlobalLock.release();
        std::cout << "CDL_SET_LOOKUP_TABLE: ";
        if( (Smart::CdlTagType)p1 == Smart::CDL_DEFAULT_LOOKUP )
        {
          std::cout << "CDL_DEFAULT_LOOKUP selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_SECOND_LOOKUP )
        {
          std::cout << "CDL_SECOND_LOOKUP selected\n";
        }
        else
        {
          std::cout << "unsupported parameter\n";
        }

        break;
      }


      ///////////////////////////////////////////////////////////////////// 
      // CDL_SET_TRANS_VELOCITY 
      case Smart::CDL_SET_TRANS_VELOCITY:
      {
        cdlGlobalLock.acquire();
 
        // set vmin
        if( p1 < CDL_V_TRA_MIN)
        {
          globalState.vmin = CDL_V_TRA_MIN;
        }
        else
        {
          globalState.vmin = p1;
        }

        // set vmax
        if( p2 > CDL_V_TRA_MAX)
        {
          globalState.vmax = CDL_V_TRA_MAX;
        }
        else
        {
          globalState.vmax = p2;
        }

        cdlGlobalLock.release();

        std::cout << "CDL_SET_TRANS_VELOCITY: "
                  << "Parameters: " << p1 << " " << p2 << " \n";
        break;
      }


      ///////////////////////////////////////////////////////////////////// 
      // CDL_SET_ROT_VELOCITY
      case Smart::CDL_SET_ROT_VELOCITY:
      {
        cdlGlobalLock.acquire();

        // set wmin
        if( p1 < CDL_V_ROT_MIN)
        {
          globalState.wmin = CDL_V_ROT_MIN*M_PI/180.0;
        }
        else
        {
          globalState.wmin = p1*M_PI/180.0;
        }

        // set vmax
        if( p2 > CDL_V_ROT_MAX)
        {
          globalState.wmax = CDL_V_ROT_MAX*M_PI/180.0;
        }
        else
        {
          globalState.wmax = p2*M_PI/180.0;
        }

        cdlGlobalLock.release();

        std::cout << "CDL_SET_ROT_VELOCITY: "
                  << "Parameters: " << p1 << " " << p2 << " \n";
        break;
      }


      /////////////////////////////////////////////////////////////////////
      // CDL_SET_MODE_GOAL
      case Smart::CDL_SET_MODE_GOAL:
      {
        cdlGlobalLock.acquire();
        globalState.goalSpec = (Smart::CdlTagType)p1;
        cdlGlobalLock.release();
        std::cout << "CDL_SET_MODE_GOAL: ";
        if( (Smart::CdlTagType)p1 == Smart::CDL_ABSOLUTE )
        {
          std::cout << "CDL_ABSOLUTE selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_PLANNER )
        {
          std::cout << "CDL_PLANNER selected\n";
        }
        else if( (Smart::CdlTagType)p1 == Smart::CDL_PERSON )
        {
          std::cout << "CDL_PERSON selected\n";
        }
        else
        {
          std::cout << "unsupported parameter\n";
        }

        break;
      }


      /////////////////////////////////////////////////////////////////////
      // CDL_SET_GOAL
      case Smart::CDL_SET_GOAL:
      {
        cdlGlobalLock.acquire();

	globalState.goalX =p1;
        globalState.goalY =p2;
        globalState.goalA =p3*M_PI/180.0;

        //<lutz>
        cdlGoalEventState.set(CDL_GOAL_NOT_REACHED);
        cdlGoalEventServer->put(cdlGoalEventState);
        std::cout<<"CDL EVENT CDL_GOAL_NOT_REACHED!"<<std::endl;
        //<lutz>

        cdlGlobalLock.release();
        std::cout << "CDL_SET_GOAL: "
                  << "Parameters: x:" << p1 << " y:" << p2 << " a:"<< p3 <<" \n";
        break;
      }

	
      /////////////////////////////////////////////////////////////////////
      // CDL_SET_APPROACH_DIST
      case Smart::CDL_SET_APPROACH_DIST:
      {
        cdlGlobalLock.acquire();

        globalState.approachDistance =p1;
        
	cdlGlobalLock.release();

        std::cout << "CDL_SET_APPROACH_DIST: "
                  << "Parameters: " << p1 <<" \n";
        break;
      }



      /////////////////////////////////////////////////////////////////////
      // CDL_SET_ID
      case Smart::CDL_SET_ID:
      {
        cdlGlobalLock.acquire();

        globalState.id =p1;

        cdlGlobalLock.release();
        //<lutz>
        //cdlGoalEventState.set(CDL_GOAL_NOT_REACHED);
        //cdlGoalEventServer->put(cdlGoalEventState);
        //std::cout<<"CDL EVENT CDL_GOAL_NOT_REACHED!"<<std::endl;
        //<lutz>
        std::cout << "CDL_SET_ID: "
                  << "Parameters: " << p1 <<" \n";
        break;
      }



      ///////////////////////////////////////////////////////////////////// 
      // default
      default:
      {
        break;
      }

    } // switch
  } // handle
}; // class


// -------------------------------------------------------------
// class NavigationVelocitySendHandler
// -------------------------------------------------------------
class NavigationVelocitySendHandler: public CHS::SendServerHandler<Smart::CommNavigationVelocity>
{
public:
  /**
   * Handler method for receiving navigation velocity orders.
   */
  void handleSend(const Smart::CommNavigationVelocity &cmd) throw()
  {
    // read the received CommNavigationVelocity object
    int v = cmd.get_v();
    int omega = (int)( cmd.get_omega()*180.0/M_PI);

    // TODO
    if( globalState.strategy == Smart::CDL_JOYSTICK )
    {
      cdlLookup->setDesiredTranslationalSpeed(v);
      //std::cout << "NavigationVelocitySendHandler::cdlLookup->setDesiredTranslationalSpeed = " << v << std::endl;
      cdlLookup->setHeading(omega);
      //std::cout << "NavigationVelocitySendHandler::cdlLookup->setHeading = " << omega << std::endl;
    }
  }
};


// ------------------------------------------------------------
// trigger cdl processing
// ------------------------------------------------------------

class TriggerCdlTimerCallback: public CHS::TimerHandler
{

public: 
        // ----------------------------------------------------
        // used to trigger the Cdl processing
        // ----------------------------------------------------

        virtual void timerExpired(const ACE_Time_Value & absolute_time, const void * arg)
        {

        cdlTriggerLock.acquire();
        cdlTrigger.signal();
        cdlTriggerLock.release();
        }

};


// -------------------------------------------------------------
// separat task to lookup cdl
// -------------------------------------------------------------
class CDLThread : public CHS::SmartTask
{
public:
  CDLThread() {};
  ~CDLThread() {};
  int svc(void);
};

int CDLThread::svc(void)
{
  Smart::CommMobileLaserScan    scan;
  Smart::CommNavigationVelocity vel;
  CHS::StatusCode               status;

  Smart::CommPlannerGoal        plannerGoal;

/* <asteck>
  SmartBasePositionClass spos;
  VisionVehUpdNewClass   visionGoal;
  TrackVehUpdNewClass    trackGoal;
  CylGoalClass           cylinderGoal;
*/ // </asteck>


  double v,w,vres,wres,vaccres,waccres;
  double vmin,vmax,wmin,wmax;
  double distance,heading,transSpeed;
  double visionSpeed,visionAngle,visionDistance;
  double x,y,z,a;
  double alpha1,alpha2,alpha3;
  double goalX,goalY,goalA;
  double wayPointX,wayPointY,wayPointA;
  long   goalId;

  double trackAngle,trackDistance,trackX,trackY,trackA;
  bool trackFlag;
  bool approachFlag;
  CdlEvalFunctionType evalFunction;
  long goalFlag;

  static int scannumber=0;

  struct timeval beginStalledTime;
  struct timeval currentTime;
  int            stalledFlag;       // 0 not stalled, 1 stalled
  double         timeDiff;
  int            turnDirection;

  Smart::CommTrackingGoal trackingGoal;

  stalledFlag = 0;

  // ------------------------------------------------------------
  // now start processing
  // ------------------------------------------------------------
  while(1) 
  {
    // ----------------------------------------------------------
    // wait for activation
    // ("moverobot")
    // ----------------------------------------------------------
    status = state->acquire("moverobot");
    if (status == CHS::SMART_OK)
    {
      // wait for the next timed trigger
      
      cdlTriggerLock.acquire();
      cdlTrigger.wait();
      cdlTriggerLock.release();

      // ----------------------------------------------------------
      // access the global configuration information
      // ----------------------------------------------------------
      cdlGlobalLock.acquire();
/*
      if (globalState.saveFlag == 1) 
      {
        smartBase.getPos(spos);
        spos.get(globalState.savedPosX,globalState.savedPosY,globalState.savedPosA);
        globalState.saveFlag = 0;
      }
*/
      localState = globalState;
      cdlGlobalLock.release();

      // ----------------------------------------------------------
      // the module has been activated
      // ----------------------------------------------------------

      // select the correct lookup table
      if (localState.lookupTable == Smart::CDL_SECOND_LOOKUP) 
      {
        cdlLookup = &cdlLookupSecond;
      } 
      else 
      {
        cdlLookup = &cdlLookupDefault;
      }

      // we have to ask for the current velocities


      //smartBase.getRotateVelocity(w);     ???
      //smartBase.getTranslateVelocity(v);  ???
      //smartBase.getPos(spos);             ???
  
      // dont wait for scan (PushNewest)
      status = laserClient->getUpdate( scan );
      if (status != CHS::SMART_OK) 
      {
        std::cout << "blocking wait status " << CHS::StatusCodeConversion(status) << " not ok => retry ..." << std::endl;
      }
      else
      {
        v = scan.get_base_state().get_base_velocity().get_v();
        w = scan.get_base_state().get_base_velocity().get_omega_base();

        x = scan.get_base_state().get_base_position().get_x();
        y = scan.get_base_state().get_base_position().get_y();
        a = scan.get_base_state().get_base_position().get_base_alpha();

        // --------------------------------------------------------
        // cdl-loop
        // --------------------------------------------------------
        switch(localState.strategy) 
        {

          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_ROTATE
          /////////////////////////////////////////////////////////////////////////////////////////
          case Smart::CDL_ROTATE:
          {
            // ----------------------------------------------------
            // head into the given direction and report this by
            // the goal reached event
            // ----------------------------------------------------

            approachFlag = false;

            cdlLookup->setLaserscan(scan);

            cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);

            switch(localState.goalSpec) {
              case Smart::CDL_ABSOLUTE:
                // -------------------------------------------------
                // heading is given by absolute position
                // -------------------------------------------------
                cdlLookup->setGoalPosition(localState.goalX,localState.goalY);
                heading  = angle00(atan2(localState.goalY-y,localState.goalX-x)-a);
                approachFlag=true;
                break;
              default:
               approachFlag=false;
                break;
            }
            if (approachFlag==false) {
              vres = 0.0;
              wres = 0.0;
            } else {
              if ((heading < localState.rotateError) && (heading >-localState.rotateError)) 
              {
                // heading ok, stop turning
                vres = 0.0;
                wres = 0.0;

                cout << "Heading OK, stop\n";

                // -------------------------------------------
                // put event into object
                // -------------------------------------------
                //<lutz>
                if(w == 0)
                {
                //<lutz>
                cdlGoalEventState.set(CDL_GOAL_REACHED);
                cdlGoalEventServer->put(cdlGoalEventState);
                std::cout<<"CDL EVENT CDL_GOAL_REACHED FIRED!"<<std::endl;
                }
              } else {
               // not inside goal region
               cdlLookup->setMaxDistance(CDL_MAX_DISTANCE);
               cdlLookup->calculateSpeedValues(v,w,x,y,a,0.0,0.0,
                                            localState.wmin,localState.wmax,
                                            CDL_STRATEGY_11,CDL_EVAL_STANDARD,
                                            vres,wres,vaccres,waccres);
               cout << "Heading NOT OK\n";
              }
            }

            break;
          } // case CDL_ROTATE






          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_REACTIVE
          /////////////////////////////////////////////////////////////////////////////////////////
          case Smart::CDL_REACTIVE:
          {
            // ----------------------------------------------------
            // drive into that direction which allows high
            // translational velocity and provides large
            // remaining travel distance
            // ----------------------------------------------------

            cdlLookup->setLaserscan(scan);

            cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);
            cdlLookup->setMaxDistance(CDL_MAX_DISTANCE);
            cdlLookup->setDesiredTranslationalSpeed(localState.vmax);

            cdlLookup->calculateSpeedValues( v, w, x, y, a, localState.vmin, localState.vmax, localState.wmin, localState.wmax, CDL_STRATEGY_2, CDL_EVAL_STANDARD, vres, wres, vaccres, waccres);


            break;
          } // case CDL_REACTIVE



          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_JOYSTICK
          /////////////////////////////////////////////////////////////////////////////////////////
          case Smart::CDL_JOYSTICK:
          {

            cdlLookup->setLaserscan(scan);

            cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);
            cdlLookup->setMaxDistance(CDL_MAX_DISTANCE);

            cdlLookup->calculateSpeedValues( v, w, x, y, a, localState.vmin, localState.vmax, localState.wmin, localState.wmax,
                                             CDL_STRATEGY_5, CDL_EVAL_STANDARD, vres, wres, vaccres, waccres);


            break;
          } // case CDL_JOYSTICK



          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_TURN
          /////////////////////////////////////////////////////////////////////////////////////////
          case Smart::CDL_TURN:
          {
            // ----------------------------------------------------

            cdlLookup->setLaserscan(scan);

            cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);
            cdlLookup->setMaxDistance(CDL_MAX_DISTANCE);
            cdlLookup->setDesiredTranslationalSpeed(localState.vmax);

            cdlLookup->calculateSpeedValues( v, w, x, y, a, localState.vmin, localState.vmax, localState.wmin, localState.wmax,
                                             CDL_STRATEGY_11, CDL_EVAL_STANDARD, vres, wres, vaccres, waccres);

            break;
          }



          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_APPROACH_HALT
          /////////////////////////////////////////////////////////////////////////////////////////

          case Smart::CDL_APPROACH_HALT:
          {
          
          approachFlag = false;

          cdlLookup->setLaserscan(scan);
          cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);

          switch(localState.goalSpec) {
            
           case Smart::CDL_ABSOLUTE:
            // goal specified directly in this module

            approachFlag = true;
            evalFunction = CDL_EVAL_STOPPING;
            goalX     = localState.goalX;
            goalY     = localState.goalY;
            cdlLookup->setGoalPosition(goalX,goalY);
            cdlLookup->setEvalStopping(3000.0,2000.0,800.0,200.0,50.0,2000.0);
            vmin = localState.vmin;
            vmax = localState.vmax;
            wmin = localState.wmin;
            wmax = localState.wmax;
            break;

           case Smart::CDL_PLANNER:

            // goal specification from planner
            // don't use getWait because the cycle time of the cdl process
            // is higher than the planner cycle time.
            //
            status =  plannerClient->getUpdate(plannerGoal);
            plannerGoal.get_goal(wayPointX,wayPointY,wayPointA,goalX,goalY,goalA,goalId,goalFlag);

            //std::cout<<"CDL localState.id: "<<localState.id<<" goalID: "<<goalId<<std::endl;
            if (goalId != localState.id) {
              // received not yet the actual data
              approachFlag=false;
            } else if (goalFlag != 0) {
              // currently no valid goal available
              approachFlag=false;
            } else {
              approachFlag=true;
              cdlLookup->setGoalPosition(wayPointX,wayPointY);
              distance = sqrt((wayPointX-goalX)*(wayPointX-goalX)
                             +(wayPointY-goalY)*(wayPointY-goalY));
              if (distance < 100.0) {
                //std::cout<<"CDL_EVAL_STOPPING"<<endl;
                // approach final destination
                // the final goal point and the next way point are very close to
                // each other
                evalFunction = CDL_EVAL_STOPPING;
                if (localState.lookupTable==Smart::CDL_SECOND_LOOKUP) {
                  cdlLookup->setEvalStopping(2500.0,1000.0,
                                              500.0, 200.0,200.0,
                                             1500.0);
                } else {
                  cdlLookup->setEvalStopping(1000.0,500.0,
                                              800.0, 400.0,100.0,
                                             1500.0);
                  //cdlLookup->setEvalStopping(2500.0,1000.0,
                    //                          800.0, 300.0,100.0,
                      //                       1500.0);

                }
              } else {
                // approach intermediate way point
                //std::cout<<"CDL_EVAL_PASSING"<<endl;
                evalFunction = CDL_EVAL_PASSING;
                if (localState.lookupTable==Smart::CDL_SECOND_LOOKUP) {
                  cdlLookup->setEvalPassing(2500.0,1000.0,
                                             500.0, 500.0,500.0,
                                            1500.0);
                } else {
                  cdlLookup->setEvalPassing(2500.0,1000.0,
                                             800.0, 600.0,600.0,
                                            1500.0);
                  //cdlLookup->setEvalPassing(2500.0,1000.0,
                    //                         800.0, 400.0,200.0,
                      //                      1500.0);
                }
              }
            }
            vmin = localState.vmin;
            vmax = localState.vmax;
            wmin = localState.wmin;
            wmax = localState.wmax;
            break;

           default:
            approachFlag = false;
            vmin         = 0.0;
            vmax         = 0.0;
            wmin         = 0.0;
            wmax         = 0.0;
            break;
          }//switch(localState.goalSpec)

          if (approachFlag==false) {
            vres = 0.0;
            wres = 0.0;
          } else {

            distance = sqrt((goalX-x)*(goalX-x)+(goalY-y)*(goalY-y));
            std::cout<<"Distance to Goal: "<<distance<<" approachDistance: "<<localState.approachDistance<<std::endl;
            if (distance < localState.approachDistance) {
		// goal reached, stop robot
		vres = 0.0;
            	wres = 0.0;

                // this stop is intended, since the goal has been reached
                stalledFlag = 0;

                // -------------------------------------------
                // put event into object
                // -------------------------------------------
                cdlGoalEventState.set(CDL_GOAL_REACHED);
                cdlGoalEventServer->put(cdlGoalEventState); 
                std::cout<<"CDL EVENT CDL_GOAL_REACHED FIRED!"<<std::endl;

                std::cout << "GOAL REACHED !!!!!!!! actpos " << x << " " << y << " " << a*180.0/M_PI << "\n";
                std::cout << "             goal   " << localState.goalX << " " << localState.goalY << "\n";

            } else {
                //cdlLookup->calculateSpeedValues(v, w, x, y, a, localState.vmin, localState.vmax, localState.wmin, localState.wmax, CDL_STRATEGY_6, evalFunction, vres, wres, vaccres, waccres);
                cdlLookup->setDesiredTranslationalSpeed(localState.vmax); 
                cdlLookup->calculateSpeedValues(v, w, x, y, a, localState.vmin, localState.vmax, localState.wmin, localState.wmax, CDL_STRATEGY_12, evalFunction, vres, wres, vaccres, waccres);
                std::cout << "vres = " << vres << "; wres = " << wres/M_PI*180.0 << std::endl;
                double gainDist = distance / 750.0;
                if(gainDist>1.0) gainDist=1.0;

                vres *= gainDist;
                if(vres > 20 && vres < 150) vres = 150;
                std::cout << "vres = " << vres << "; wres = " << wres/M_PI*180.0 << std::endl;
                std::cout << "---------------------------------------------------------------------------\n\n\n";
            }
          }


            break;
          } //case Smart::CDL_APPROACH_HALT




          /////////////////////////////////////////////////////////////////////////////////////////
          // CDL_FOLLOW
          /////////////////////////////////////////////////////////////////////////////////////////


          case Smart::CDL_FOLLOW:
          // ----------------------------------------------------
          // try to drive into the given direction with the
          // given translational velocity
          // ----------------------------------------------------
          cdlLookup->setLaserscan(scan);

          switch(localState.goalSpec) 
          {
            case Smart::CDL_PERSON:

              trackingClient->getUpdate(trackingGoal);
              trackingGoal.get( trackAngle, trackDistance, trackX, trackY, trackFlag);

              std::cout << "Track x: " << trackX << "; y: " << trackY << "; Angle: " << ((trackAngle*180.0)/M_PI) << "; dist: " << trackDistance << "; flag: " << trackFlag << "\n";

             if (trackFlag == false) 
             {
                // no valid goal information from track server
                std::cout << "no valid goal information\n";
                vres=0.0;
                wres=0.0;
             } 
             else 
             {
                cdlLookup->setParameterRobot(localState.tcalc,localState.transAcc,localState.rotAcc);
                cdlLookup->setHeading(trackAngle);
                cdlLookup->setMaxDistance(CDL_MAX_DISTANCE);
                cdlLookup->setDesiredTranslationalSpeed(localState.vmax);
                //cdlLookup->calculateSpeedValues(v,w,0.0,0.0,0.0,localState.vmin,localState.vmax,localState.wmin,localState.wmax,CDL_STRATEGY_2,CDL_EVAL_STANDARD,vres,wres,vaccres,waccres);
                cdlLookup->calculateSpeedValues(v,w,0.0,0.0,0.0,localState.vmin,localState.vmax,localState.wmin,localState.wmax,CDL_STRATEGY_7,CDL_EVAL_STANDARD,vres,wres,vaccres,waccres);

                //if( trackDistance < localState.approachDistance) 
                if( trackDistance < localState.approachDistance && fabs(trackAngle-w) < (10.0/180.0*M_PI)   ) 
                {
                  // goal reached, stop robot
                  vres = 0.0;
                  wres = 0.0;

                  // this stop is intended, since the goal has been reached
                  stalledFlag = 0;
                }
             }

            break;
            default:
              // set everything to harmless values
              cdlLookup->setHeading(0.0);
              cdlLookup->setMaxDistance(0.0);
              cdlLookup->setDesiredTranslationalSpeed(0.0);
              cdlLookup->calculateSpeedValues(v,w,0.0,0.0,0.0,localState.vmin,localState.vmax,localState.wmin,localState.wmax,CDL_STRATEGY_1,CDL_EVAL_STANDARD,vres,wres,vaccres,waccres);
            break;
          }
          //smartBase.setAccelerations(localState.rotAcc,localState.transAcc);

          break; //case follow


          /////////////////////////////////////////////////////////////////////////////////////////
          // default -> no strategy selected
          /////////////////////////////////////////////////////////////////////////////////////////
          default:
          {
            break;
          } // case default

        } // switch(localState.strategy)

        // ----------------------------------------------------------
        // now send commands directly to the base
        // ----------------------------------------------------------

        // TODO this is for backward driving with Joystick -> can surely be done better !!
        double speed = cdlLookup->getDesiredTranslationalSpeed();
        if( localState.strategy == Smart::CDL_JOYSTICK && speed <= 0 )
        {
          if( speed < localState.vmin ) speed = localState.vmin;
          vel.set_v(speed, 0.001);
          double wspeed = cdlLookup->getHeading();
          if( wspeed < -20.0 ) wspeed = -20.0;
          if( wspeed >  20.0 ) wspeed =  20.0;
          vel.set_omega( wspeed );
        }
        else
        {
          vel.set_v(vres, 0.001);
          vel.set_omega(wres);
        }

        navigationVelocitySendClient->send(vel);
        //std::cout << "send " << vel << std::endl;



        // -----------------------------------------------------------
        // now check whether stalled behaviour occured
        // -----------------------------------------------------------
        if (localState.freeBehavior == Smart::CDL_FREE_BEHAVIOR) 
        {
          if ((vres == 0.0) && (wres == 0.0) && (v == 0.0) && (w == 0.0)) 
          {
            // robot stops and cannot move anymore
            if (stalledFlag == 0) 
            { 
              // first occurence of stalling situation
              stalledFlag = 1;
              gettimeofday(&beginStalledTime,0);
            } 
            else 
            {
              // not the first occurence of stalling situation
              gettimeofday(&currentTime,0);
              timeDiff  = (double)(currentTime.tv_usec - beginStalledTime.tv_usec)/1000000.0;
              timeDiff += (double)(currentTime.tv_sec  - beginStalledTime.tv_sec);

              // if (timeDiff > 2.0) 
              if (timeDiff > 0.5) 
              {
                stalledFlag = 0;
                if (localState.lookupTable == Smart::CDL_SECOND_LOOKUP) 
                {
                  //status = cdlLookup->freeBehavior(700.0,turnDirection);
                  cdlLookup->freeBehavior(50.0,turnDirection);
                } 
                else 
                {
                  //status = cdlLookup->freeBehavior(500.0,turnDirection);
                  cdlLookup->freeBehavior(50.0,turnDirection);
                }

                switch (turnDirection) 
                {
                  case CDL_FREE_TURN_LEFT:
                  {
                    cout << "STALLED ==> turn left\n";
                    wres = 30.0*M_PI/180.0;
                    vres = 30.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    sleep(1);
                    //smartBase.setVelocities(0.0,0.0);
                    wres = 0.0;
                    vres = 0.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    break;
                  }
 
                  case CDL_FREE_TURN_RIGHT:
                  {
                    cout << "STALLED ==> turn right\n";
                    wres = -30.0*M_PI/180.0;
                    vres = 30.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    sleep(1);
                    wres = 0.0;
                    vres = 0.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    break;
                  }

                  case CDL_FREE_NO_TURN:
                  {
                    //cout << "STALLED ==> no chance to free robot\n";
                    cout << "STALLED ==> free no turn ?!\n";
                    wres = 0.0;
                    vres = -30.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    sleep(1);
                    wres = 0.0;
                    vres = 0.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    break;
                  }

                  case CDL_FREE_BOTH:
                  {
                    cout << "STALLED ==> both free ?!\n";
                    wres = -30.0*M_PI/180.0;
                    vres = 0.0;
                    //wres = 0.0;
                    //vres = -30.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    sleep(1);
                    wres = 0.0;
                    vres = 0.0;
                    vel.set_v(vres, 0.001);
                    vel.set_omega(wres);
                    navigationVelocitySendClient->send(vel);
                    break;
                  }
                } // switch(turnDirection)
              } // if (timeDiff > 2.0)
            } // if (stalledFlag == 0)
          } // if ((vres == 0.0) && (wres == 0.0) && (v == 0.0) && (w == 0.0))
        } // if (localState.freeBehavior == CDL_FREE_BEHAVIOR)
      } // if (status != CHS::SMART_OK) <<<---- status = laserClient->getUpdate( scan );

    } // if (status == CHS::SMART_OK) <<<---- status = state->acquire("moverobot");
    // -----------------------------------------------------
    // unlock state at end of while loop
    // -----------------------------------------------------
    status = state->release("moverobot");

  } // while(1)

  return 0;
} // CDLThread::svc






class CdlGoalTestHandler:
  public CHS::EventTestHandler<Smart::CommCdlGoalEventParameter, Smart::CommCdlGoalEventResult, Smart::CdlGoalEventState>
{
public:
  bool testEvent(Smart::CommCdlGoalEventParameter& p, Smart::CommCdlGoalEventResult& r,const Smart::CdlGoalEventState& s) throw()
  {

   int oldState,newState;

   p.get(oldState);
   s.get(newState);
   if ((oldState!=newState) && (newState==CDL_GOAL_REACHED)) {
     p.set(newState);
     r.set(newState);
     std::cout<<"Event Testhandel true!"<<std::endl;
     return true;
   } else {
   //  p.set(newState);
     return false;
   }

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
    // -------------------------------------------------------------
    // print copyright message
    // -------------------------------------------------------------
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << "| Curvature Distance Lookup Server                                     |\n";
    std::cout << "| ZAFH Servicerobotik Ulm                                              |\n";
    std::cout << "| Copyright (C) 2008 Christian Schlegel, Andreas Steck                 |\n";
    std::cout << "------------------------------------------------------------------------\n\n";

    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();
    CHS::SmartParameter parameter;

    ///////////////////////////////////////////
    // parameters
    std::string param_laserName;
    std::string param_baseName;
    std::string param_plannerName;
    std::string param_curvatureDefaultFile;
    std::string param_lookupDefaultFile;
    std::string param_accelDefaultFile;
    std::string param_curvatureSecondFile;
    std::string param_lookupSecondFile;
    std::string param_accelSecondFile;

    int status;
    //TODO Tmp variable:
    CHS::StatusCode status_code;


    ///////////////////////////////////////////
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

/* TODO    
    laserName                = (char *)parameter.getString("server","laser");
    baseName                 = (char *)parameter.getString("server","base");
    plannerName              = (char *)parameter.getString("server","planner");
    visionName               = (char *)parameter.getString("server","vision");
    trackName                = (char *)parameter.getString("server","track");
    cylinderName             = (char *)parameter.getString("server","cylinder");
*/
    parameter.getString("cdl","curvature_default_file", param_curvatureDefaultFile);
    std::cout << "param_curvatureDefaultFile = " << param_curvatureDefaultFile << std::endl;

    parameter.getString("cdl","lookup_default_file", param_lookupDefaultFile);
    std::cout << "param_lookupDefaultFile = " << param_lookupDefaultFile << std::endl;

    parameter.getString("cdl","accel_default_file", param_accelDefaultFile);
    std::cout << "param_accelDefaultFile = " << param_accelDefaultFile << std::endl;

    parameter.getString("cdl","curvature_second_file", param_curvatureSecondFile);
    std::cout << "param_curvatureSecondFile = " << param_curvatureSecondFile << std::endl;

    parameter.getString("cdl","lookup_second_file", param_lookupSecondFile);
    std::cout << "param_lookupSecondFile = " << param_lookupSecondFile << std::endl;

    parameter.getString("cdl","accel_second_file", param_accelSecondFile);
    std::cout << "param_accelSecondFile = " << param_accelSecondFile << std::endl;

    parameter.getTruthValue("server","plannerInit", globalState.plannerInit);
    std::cout << "globalState.plannerInit = " << globalState.plannerInit << std::endl;

    parameter.getTruthValue("server","joystickInit", globalState.joystickInit);
    std::cout << "globalState.joystickInit = " << globalState.joystickInit << std::endl;

    parameter.getDouble("cdl","translation_acc", globalState.transAcc);
    std::cout << "globalState.transAcc = " << globalState.transAcc << std::endl;

    parameter.getDouble("cdl","rotation_acc", globalState.rotAcc);
    std::cout << "globalState.rotAcc = " << globalState.rotAcc << " deg" << std::endl;
    globalState.rotAcc *= M_PI/180.0;

    parameter.getDouble("cdl","deltattrigger", globalState.ttrigger);
    std::cout << "globalState.ttrigger = " << globalState.ttrigger << std::endl;

    parameter.getDouble("cdl","deltatcalc", globalState.tcalc);
    std::cout << "globalState.tcalc = " << globalState.tcalc << std::endl;

    parameter.getDouble("cdl","vmin", globalState.vmin);
    std::cout << "globalState.vmin = " << globalState.vmin << std::endl;

    parameter.getDouble("cdl","vmax", globalState.vmax);
    std::cout << "globalState.vmax = " << globalState.vmax << std::endl;

    parameter.getDouble("cdl","wmin", globalState.wmin);
    std::cout << "globalState.wmin = " << globalState.wmin << " deg" << std::endl;
    globalState.wmin *= M_PI/180.0;

    parameter.getDouble("cdl","wmax", globalState.wmax);
    std::cout << "globalState.wmax = " << globalState.wmax << " deg" << std::endl;
    globalState.wmax *= M_PI/180.0;

//<lutz>
    parameter.getDouble("cdl","goalX", globalState.goalX);
    std::cout << "globalState.goalX = " << globalState.goalX << std::endl;
    
    parameter.getDouble("cdl","goalY", globalState.goalY);
    std::cout << "globalState.goalY = " << globalState.goalY << std::endl;
    
    parameter.getDouble("cdl","goalA", globalState.goalA);
    std::cout << "globalState.goalA = " << globalState.goalA << std::endl;
    globalState.goalA *= M_PI/180.0;
//<lutz>


    parameter.getDouble("cdlrotate","error", globalState.rotateError);
    std::cout << "globalState.rotateError = " << globalState.rotateError << " deg" << std::endl;
    globalState.rotateError *= M_PI/180.0;

 

    // ----------------------------------------------------------
    // global states
    // ----------------------------------------------------------
    globalState.lookupTable  = Smart::CDL_DEFAULT_LOOKUP;
    globalState.strategy     = Smart::CDL_NEUTRAL;
    globalState.goalSpec     = Smart::CDL_NEUTRAL;
    globalState.freeBehavior = Smart::CDL_NO_FREE_BEHAVIOR;
    globalState.id           = 0;

    cdlLookup = &cdlLookupDefault;

    globalState.plannerClientSubscribed  = false;

    cdlGlobalLock.release();

    

    // ----------------------------------------------------------
    // Loding lookup tables
    // ----------------------------------------------------------
    cout << "\nLoading lookup tables ...\n";

    status = cdlLookupDefault.loadCurvatureIndexAscii( (char*)param_curvatureDefaultFile.c_str() );
    if (status!=0) {
      cerr << "Error loading default curvature file\n";
      exit(0);
    }
    status = cdlLookupDefault.loadDistAngleLookupBin( (char*)param_lookupDefaultFile.c_str() );
    if (status!=0) {
      cerr << "Error loading lookup default file\n";
      exit(0);
    }
    status = cdlLookupDefault.loadAccLookupBin( (char*)param_accelDefaultFile.c_str() );
    if (status!=0) {
      cerr << "Error loading acceleration default file\n";
      exit(0);
    }

    status = cdlLookupSecond.loadCurvatureIndexAscii( (char*)param_curvatureSecondFile.c_str() );
    if (status!=0) {
      cerr << "Error loading second curvature file\n";
      exit(0);
    }
    status = cdlLookupSecond.loadDistAngleLookupBin( (char*)param_lookupSecondFile.c_str() );
    if (status!=0) {
      cerr << "Error loading lookup second file\n";
      exit(0);
    }
    status = cdlLookupSecond.loadAccLookupBin( (char*)param_accelSecondFile.c_str() );
    if (status!=0) {
      cerr << "Error loading acceleration second file\n";
      exit(0);
    }
    cout << "... lookup tables loaded successfully\n";




    // component
    component = new CHS::SmartComponent("smartCdlServer",argc,argv);

    // base SendClient
    navigationVelocitySendClient = new CHS::SendClient<Smart::CommNavigationVelocity>(component,"smartPioneerBaseServer","navigationvelocity");

    // laser PushNewestClient
    laserClient = new CHS::PushNewestClient<Smart::CommMobileLaserScan>(component);
    laserClient->connect("smartLaserServer","scan");
    laserClient->subscribe();

    plannerClient = new CHS::PushNewestClient<Smart::CommPlannerGoal>(component);
    status_code =  plannerClient->connect("smartPlanner","plannerGoal");
    std::cout << "status code returned from connect to planner " << CHS::StatusCodeConversion(status_code);
    plannerClient->subscribe();

    // trackingClient
    trackingClient = new CHS::PushNewestClient<Smart::CommTrackingGoal>(component);
    status_code =  trackingClient->connect("smartLaserTrackingServer","trackingGoal");
    std::cout << "status code returned from connect to planner " << CHS::StatusCodeConversion(status_code);
    trackingClient->subscribe();

    // state StateServer
    stateHandler = new PrintStateChangeHandler();
    state = new CHS::SmartStateServer(component,*stateHandler);

    if (state->defineStates("moverobot" ,"moverobot") != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->activate() != CHS::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;

    // parameter SendServer
    ParameterHandler sendParameterHandler;
    parameterServer = new CHS::SendServer<Smart::CommCdlParameter>(component, "cdlParameter", sendParameterHandler);

    // joystick SendServer
    NavigationVelocitySendHandler navigationVelocitySendHandler;
    CHS::ThreadQueueSendHandler<Smart::CommNavigationVelocity> threadNavigationVelocitySendHandler(navigationVelocitySendHandler);
    if( globalState.joystickInit == true )
    {
      navigationVelocitySendServer = new CHS::SendServer<Smart::CommNavigationVelocity>(component, "navigationvelocity", 
                                                                                        threadNavigationVelocitySendHandler);
    }


    // Cdl Goal Event Server
    CdlGoalTestHandler cdlGoalTest;
    cdlGoalEventServer = new CHS::EventServer<Smart::CommCdlGoalEventParameter,Smart::CommCdlGoalEventResult,Smart::CdlGoalEventState>(component, "cdlGoalEvent",cdlGoalTest);


    // timer triggers cdl processing
    // ACE_Time_Value(seconds, microseconds) values from ttrigger in s
    TriggerCdlTimerCallback periodicTrigger;
    double fractpart,intpart;
    fractpart = modf(globalState.ttrigger, &intpart);
    component->getTimerThread().scheduleTimer(periodicTrigger, (void*) 0, ACE_Time_Value(0,0),
                                              ACE_Time_Value(intpart, (int)(fractpart*1000*1000)));

    
    // start cdl thread
    CDLThread cdlThread;
    cdlThread.open();

    // run
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

