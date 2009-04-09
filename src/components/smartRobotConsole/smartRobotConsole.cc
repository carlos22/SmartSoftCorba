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
//  This file is part of the "SmartSoft robot console component".
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
//  This component is for testing purposes only!
//  
// --------------------------------------------------------------------------



#include "smartSoft.hh"
#include "commMapperParameter.hh"
#include "commPlannerParameter.hh"
#include "commForkLiftCommand.hh"
#include "commCdlParameter.hh"
#include "commSpeechOutputMessage.hh"
#include "commCdlGoalEvent.hh"
#include "commPlannerEvent.hh"

#include <sstream>

CHS::SmartComponent *component;


CHS::SmartMutex speechLock;

class GoalEventThread : public CHS::SmartTask
{
public:
  GoalEventThread() {};
  ~GoalEventThread() {};
  int svc(void);
};

int GoalEventThread::svc(void)
{
  //Cdl Goal Event!!
  CHS::EventClient<Smart::CommCdlGoalEventParameter,Smart::CommCdlGoalEventResult> cdlGoalEventClient(component, "smartCdlServer","cdlGoalEvent");


  Smart::CommCdlGoalEventParameter parameter;
  Smart::CommCdlGoalEventResult    result;
  CHS::EventId               id;

  CHS::SendClient<Smart::CommSpeechOutputMessage> smartSpeechSendClient(component, "smartSpeechOutputServer" ,"text2speech");

  CHS::StatusCode status;
  int res;
  int param;
  //
  // continuous event
  //

  param = CDL_GOAL_UNKNOWN;
  parameter.set(param);


  speechLock.acquire();

  while(true)
  {
    speechLock.acquire();
    status = cdlGoalEventClient.activate(CHS::single, parameter, id);

    status = cdlGoalEventClient.getEvent(id, result);
    if (status == CHS::SMART_OK) {
      result.get(res);
      smartSpeechSendClient.send(Smart::CommSpeechOutputMessage("I reached the goal!"));
      status = cdlGoalEventClient.deactivate(id);
    }
  }



  return 0;
};




class PlannerNoPathEventThread : public CHS::SmartTask
{
public:
  PlannerNoPathEventThread() {};
  ~PlannerNoPathEventThread() {};
  int svc(void);
};

int PlannerNoPathEventThread::svc(void)
{
  //Cdl Goal Event!!
  CHS::EventClient<Smart::CommPlannerEventParameter,Smart::CommPlannerEventResult> eventClient(component, "smartPlanner","plannerEvent");
  CHS::SendClient<Smart::CommMapperParameter> smartMapperParameterSendClient(component,"smartMapper","mapperParameter");
  Smart::CommPlannerEventParameter parameter;
  Smart::CommPlannerEventResult result;
  CHS::EventId               id;
  Smart::CommMapperParameter cmdMap;
  std::string str;
 

  CHS::StatusCode status;
  int res;
  int param = PLANNER_NO_ERROR;
  //
  // continuous event
  //
  parameter.set(param);

  status = eventClient.activate(CHS::continuous, parameter, id);

  while(true)
  {
    status = eventClient.getEvent(id, result);
    if (status == CHS::SMART_OK) {
      result.get(res);
      if(res == PLANNER_NO_PATH)
      {
        cout<<"Planner delete MAP!!"<<std::endl; 

          str = "CURLOADLTM";
          cmdMap.set(str);
          smartMapperParameterSendClient.send(cmdMap);

      }  
    }
  }

  status = eventClient.deactivate(id);


  return 0;
};



class ConsoleThread : public CHS::SmartTask
{
public:
  ConsoleThread() {};
  ~ConsoleThread() {};

  int svc(void);

private:

};

int ConsoleThread::svc(void)
{
  CHS::StatusCode status;
  int mainMenuItem;

  std::cout << "------------------------------------------------------------------------\n";
  std::cout << "| smartRobotConsole                                                    |\n";
  std::cout << "| ZAFH Servicerobotik Ulm                                              |\n";
  std::cout << "| Copyright (C) 2008 Christian Schlegel, Andreas Steck                 |\n";

  while(true)
  {
    // main menu
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << "Main Menu:\n";

    std::cout << " 01 - Mapper state\n";
    std::cout << " 02 - Mapper parameter\n";
    std::cout << " 03 - Planner state\n";
    std::cout << " 04 - Planner parameter\n";
    std::cout << " 05 - ForkLift command\n";
    std::cout << " 06 - CDL state\n";
    std::cout << " 07 - CDL parameter\n";
    std::cout << " 08 - Demos\n";
    std::cout << " <ctrl> + <c> for exit\n";
    std::cout << "------------------------------------------------------------------------\n";

    std::cout << "\nplease choose number:  ";
    cin >> mainMenuItem;

    // sub menu
    switch( mainMenuItem )
    {
      // 1 - Mapper state
      case 1:
      {
        std::string stateServerName = "smartMapper";
        std::list<std::string> mainstates;
        CHS::SmartStateClient stateClient(component, stateServerName);

        // first get list of main states 
        status = stateClient.getWaitMainStates(mainstates);
        if(status != CHS::SMART_OK) 
        {
          std::cout << "could not connect to " << stateServerName << "\n\n";
        }
        else
        {
          // print list of main states 
          unsigned int itemNumber = 1;

          std::cout << "\nlist of possible main states: " << std::endl;
          for (std::list<std::string>::iterator iterator1=mainstates.begin();iterator1 != mainstates.end(); ++iterator1) 
          {
            std::cout << setw(2) << itemNumber << " -- mainstate :  " << *iterator1 << std::endl;
            itemNumber++;
          }
          std::cout << setw(2) << itemNumber << " -- return to main menu" << std::endl;

          // ask which state should be sent
          std::cout << "\nplease choose number:  ";
          cin >> itemNumber;

          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            stateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }

        } // else
        
        break;
      } // case 1



      // 2 - Mapper parameter
      case 2:
      {
        Smart::CommMapperParameter cmd;
        std::string inString;

        CHS::SendClient<Smart::CommMapperParameter> smartMapperParameterSendClient(component,"smartMapper","mapperParameter");

        std::cout << std::endl;
        std::cout << "CURPARAMETER ?xsize ?ysize ?xoff ?yoff ?id  : create current map (size/offset)\n";
        std::cout << "CURLTM DISABLE/ENABLE ?threshold            : preoccupy cur out of ltm each cycle\n";
        std::cout << "CUREMPTY ACCUMULATE/EMPTY                   : accumulate/clear cur map each cycle\n";
        std::cout << "CURSAVE ?number                             : save current map (xml)\n";
        std::cout << "CURLOAD ?number                             : load current map (xml)\n";
        std::cout << "CURLOADLTM                                  : load current map out of ltm\n";
        std::cout << "CURSAVEXPM ?number                          : save current map (xpm)\n\n";
        std::cout << "LTMPARAMETER ?xsize ?ysize ?xpos ?ypos ?id  : create longterm map\n";
        std::cout << "LTMINITIALIZE ?init                         : clear map and initialize\n";
        std::cout << "LTMSAVE ?number                             : save longterm map (xml)\n";
        std::cout << "LTMLOAD ?number                             : load longterm map (xml)\n";
        std::cout << "LTMSAVEXPM ?number                          : save longterm map (xpm)\n\n";
        std::cout << "m to return to main menu" << std::endl;

        std::cout << std::endl;
        std::cout << "usage: command(param1)(param2)(param3)\n";
        std::cout << "       ltmparameter(80000)(80000)(-20000)(-40000)(0)\n";

        std::cout << "\nplease enter command:  ";

        cin >> inString;
        std::cout << "your input: " << inString << std::endl;

        if( strcasecmp("m", inString.c_str() ) != 0 )
        {

          cmd.set(inString);
          smartMapperParameterSendClient.send(cmd);
          std::cout << "send.\n\n";
        }
        break;
      } // case 2




      // 3 - Planner state
      case 3:
      {
      
        std::string stateServerName = "smartPlanner";
        std::list<std::string> mainstates;
        CHS::SmartStateClient stateClient(component, stateServerName);

        // first get list of main states
        status = stateClient.getWaitMainStates(mainstates);
        if(status != CHS::SMART_OK)
        {
          std::cout << "could not connect to " << stateServerName << "\n\n";
        }
        else
        {
          // print list of main states
          unsigned int itemNumber = 1;

          std::cout << "\nlist of possible main states: " << std::endl;
          for (std::list<std::string>::iterator iterator1=mainstates.begin();iterator1 != mainstates.end(); ++iterator1)
          {
            std::cout << setw(2) << itemNumber << " -- mainstate :  " << *iterator1 << std::endl;
            itemNumber++;
          }
          std::cout << setw(2) << itemNumber << " -- return to main menu" << std::endl;

          // ask which state should be sent
          std::cout << "\nplease choose number:  ";
          cin >> itemNumber;

          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            stateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }

        } // else

        break;
      } // case 3 



      // 4 - Planner parameter
      case 4:
      {
        Smart::CommPlannerParameter cmd;
        std::string inString;

        CHS::SendClient<Smart::CommPlannerParameter> smartPlannerParameterSendClient(component,"smartPlanner","plannerParameter");

        std::cout << std::endl;
        std::cout << "ID ?id  : set Planner_ID\n";
        std::cout << "DELETEGOAL       : delete all specified goals in path planner\n";
        std::cout << "SETDESTINATIONLINE ?xstart ?ystart ?xend ?yend : set destination line in path planner\n";
        std::cout << "SETDESTINATIONCIRCLE ?xcenter ?ycenter ?radius : set destination circle in path planner\n";
        std::cout << "m to return to main menu" << std::endl;

        std::cout << std::endl;
        std::cout << "usage: command(param1)(param2)(param3)\n";
        std::cout << "       setdestinationcircle(8000)(8000)(150)\n";

        std::cout << "\nplease enter command:  ";

        cin >> inString;
        std::cout << "your input: " << inString << std::endl;

        if( strcasecmp("m", inString.c_str() ) != 0 )
        {

          cmd.set(inString);
          smartPlannerParameterSendClient.send(cmd);
          std::cout << "send.\n\n";
        }
        break;

      } // case 4



      // 5 - Forklift command
      case 5:
      {
        Smart::CommForkLiftCommand cmd;
        int  userPosition;

        CHS::SendClient<Smart::CommForkLiftCommand> smartForkLiftCommandSendClient(component,"smartForkLiftServer","forkliftcommand");

        std::cout << std::endl;
        std::cout << "please enter forklift position in mm\n";
        std::cout << "-1 to return to main menu" << std::endl;
        std::cout << "position: ";

        cin >> userPosition;
        std::cout << "your input: " << userPosition << std::endl;

        if( userPosition >= 0 ) 
        { 
          cmd.set_position( userPosition );
          smartForkLiftCommandSendClient.send(cmd);
          std::cout << "send.\n\n";
        }

        break;
      } // case 5



      // 6 - CDL state
      case 6:
      {
        std::string stateServerName = "smartCdlServer";
        std::list<std::string> mainstates;
        CHS::SmartStateClient stateClient(component, stateServerName);

        // first get list of main states 
        status = stateClient.getWaitMainStates(mainstates);
        if(status != CHS::SMART_OK)
        {
          std::cout << "could not connect to " << stateServerName << "\n\n";
        }
        else
        {
          // print list of main states 
          unsigned int itemNumber = 1;

          std::cout << "\nlist of possible main states: " << std::endl;
          for (std::list<std::string>::iterator iterator1=mainstates.begin();iterator1 != mainstates.end(); ++iterator1)
          {
            std::cout << setw(2) << itemNumber << " -- mainstate :  " << *iterator1 << std::endl;
            itemNumber++;
          }
          std::cout << setw(2) << itemNumber << " -- return to main menu" << std::endl;

          // ask which state should be sent
          std::cout << "\nplease choose number:  ";
          cin >> itemNumber;

          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            stateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }

        } // else


        break;
      } // case 6



      // 7 - CDL parameter
      case 7:
      {
        Smart::CommCdlParameter cmd;
        std::string inString;
        CHS::SendClient<Smart::CommCdlParameter> smartCdlParameterSendClient(component,"smartCdlServer","cdlParameter");
        std::cout << std::endl;
        std::cout << "STRATEGY REACTIVE/JOYSTICK/APPROACH_HALT\n";
        std::cout << "         TURN/ROTATE                        : set the cdl strategy\n";
        std::cout << "FREEBEHAVIOR ACTIVATE/DEACTIVATE            : freebehavior in stall situation\n";
        std::cout << "LOOKUPTABLE DEFAULT/SECOND                  : set cdl lookup table\n";
        std::cout << "TRANSVEL ?vmin ?vmax                        : set translation velocity min,max in mm/s\n";
        std::cout << "ROTVEL ?wmin ?wmax (deg/sec)                : set rotational velocity min,max in deg/s\n";
        std::cout << "GOALMODE ABSOLUTE/PLANNER                   : set the cdl goal mode\n";
        std::cout << "GOALREGION ?x ?y ?h (mm,mm,deg)             : set cdl goal x,y,heading\n";
        std::cout << "APPROACHDIST ?dist (mm)                     : set cdl goal approach distance in mm\n";
        std::cout << "ID ?id                                      : set CDL_ID\n";
        std::cout << "m to return to main menu" << std::endl;

        std::cout << std::endl;
        std::cout << "usage: command(param1)(param2)(param3)\n";
        std::cout << "       strategy(reactive) or transvel(0)(500)\n";

        std::cout << "\nplease enter command:  ";

        cin >> inString;
        std::cout << "your input: " << inString << std::endl;

        if( strcasecmp("m", inString.c_str() ) != 0 )
        {

          cmd.set(inString);
          smartCdlParameterSendClient.send(cmd);
          std::cout << "send.\n\n";
        }
        break;
      } // case 7


      // 8 - Demo
      case 8:
      {
        
	unsigned int itemNumber;
        std::list<std::string> mainstates;

        Smart::CommCdlParameter cmd;
        Smart::CommMapperParameter cmdMap;
        int input;
        std::string str;

        Smart::CommPlannerParameter cmd_planner;

        std::cout << std::endl;
        std::cout << "(1) Demo 1 Introduction robot speechoutput\n";
        std::cout << "(2) Demo 2 Planner-CDL GOTO \n";
        std::cout << "(3) Demo 3 CDL Reactive Mode \n";
        std::cout << "(4) Demo 4 Cdl Joystick Mode \n";
        std::cout << "(0) to return to main menu" << std::endl;

        std::cout << std::endl;
        std::cout << "usage: 1\n";

        std::cout << "\nplease enter command:  ";

        cin >> input;
        std::cout << "your input: " << input << std::endl;
        

	switch(input){
          /////////////////////////////////
          // Demo 1 welcome the audience
          /////////////////////////////////
          case 1:
          {
            // Turn towards the audience
            CHS::SendClient<Smart::CommCdlParameter> smartCdlParameterSendClient(component,"smartCdlServer","cdlParameter");
            CHS::SmartStateClient cdlStateClient(component, "smartCdlServer"); 

            str = "STRATEGY(ROTATE)";
            cmd.set(str);
            smartCdlParameterSendClient.send(cmd);
            str = "LOOKUPTABLE(DEFAULT)";
            cmd.set(str);
            smartCdlParameterSendClient.send(cmd);
            str = "GOALMODE(ABSOLUTE)";
            cmd.set(str);
            smartCdlParameterSendClient.send(cmd);

            str = "GOALREGION(-1000)(0)(0)";
            cmd.set(str);
            smartCdlParameterSendClient.send(cmd);
            itemNumber = 2;


            // CDL State --> GO!
            itemNumber = 2;
            cdlStateClient.getWaitMainStates(mainstates);
            // check input
            if( itemNumber <= mainstates.size() )
            {
              // iterate to selected state
              std::list<std::string>::iterator iterator1 = mainstates.begin();
              for(unsigned int i=1; i < itemNumber; i++)
              {
                ++iterator1;
              }
              // send selected state
              cdlStateClient.setWaitState( *iterator1 );
              std::cout << "state <" << *iterator1 << "> sent\n";
            }
           

            //Cdl Goal Event!!
            CHS::EventClient<Smart::CommCdlGoalEventParameter,Smart::CommCdlGoalEventResult> cdlGoalEventClient(component, "smartCdlServer","cdlGoalEvent");


            Smart::CommCdlGoalEventParameter parameter;
            Smart::CommCdlGoalEventResult    result;
            CHS::EventId               id;

            CHS::StatusCode status;
            int b;
            int a = 0;
            //
            // continuous event
            //
            parameter.set(a);

            status = cdlGoalEventClient.activate(CHS::single, parameter, id);

 
            status = cdlGoalEventClient.getEvent(id, result);
            if (status == CHS::SMART_OK) {
              result.get(b);
              std::cout << "event id: " << id << " event fired: " << b << "\n";
            }


            status = cdlGoalEventClient.deactivate(id);

            //Speechoutput
            CHS::SendClient<Smart::CommSpeechOutputMessage> smartSpeechSendClient(component, "smartSpeechOutputServer" ,"text2speech");
            smartSpeechSendClient.send(Smart::CommSpeechOutputMessage("Welcome to the smartsoft robotic framework!"));           
            
            // CDL State --> STOP!
            itemNumber = 1;
            cdlStateClient.getWaitMainStates(mainstates);
            // check input
            if( itemNumber <= mainstates.size() )
            {
              // iterate to selected state
              std::list<std::string>::iterator iterator1 = mainstates.begin();
              for(unsigned int i=1; i < itemNumber; i++)
              {
                ++iterator1;
              }
              // send selected state
              cdlStateClient.setWaitState( *iterator1 );
              std::cout << "state <" << *iterator1 << "> sent\n";
            }
            break;
          }
          ////////////////////////////////////
          // Demo 2 move with planner and CDL
          ////////////////////////////////////
          case 2:
          {
          int cdlId = 0;
          std::string inString;
          CHS::SendClient<Smart::CommCdlParameter> smartCdlParameterSendClient(component,"smartCdlServer","cdlParameter");
          CHS::SmartStateClient mapperStateClient(component, "smartMapper");
          CHS::SendClient<Smart::CommPlannerParameter> smartPlannerParameterSendClient(component,"smartPlanner","plannerParameter");
          CHS::SmartStateClient plannerStateClient(component, "smartPlanner");
          CHS::SendClient<Smart::CommMapperParameter> smartMapperParameterSendClient(component,"smartMapper","mapperParameter");
          CHS::SmartStateClient cdlStateClient(component, "smartCdlServer");


          GoalEventThread goalEventThread;

          std::cout<<"\nActivate speech output? (y/n): ";
          cin>>inString;
          if( strcasecmp("n", inString.c_str() ) != 0 )
          {
            std::cout<<"Speach output activated!\n";
            //Event Thread
            goalEventThread.open();
          }

          PlannerNoPathEventThread plannerNoPathEventThread;
          plannerNoPathEventThread.open();
          
          str = "CURLTM(DISABLE)(10)";
          cmdMap.set(str);
          smartMapperParameterSendClient.send(cmdMap);

          str = "CUREMPTY(ACCUMULATE)";
          cmdMap.set(str);
          smartMapperParameterSendClient.send(cmdMap);

          // set build both Map
          itemNumber = 2;
          // first get list of main states
          mapperStateClient.getWaitMainStates(mainstates);
           if( itemNumber <= mainstates.size() )
           {
             // iterate to selected state
             std::list<std::string>::iterator iterator1 = mainstates.begin();
             for(unsigned int i=1; i < itemNumber; i++)
             {
               ++iterator1;
              }
             // send selected state
             mapperStateClient.setWaitState( *iterator1 );
             std::cout << "state <" << *iterator1 << "> sent\n";
           }


          // CDL SETUP
          str = "STRATEGY(APPROACH_HALT)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "FREEBEHAVIOR(ACTIVATE)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "LOOKUPTABLE(DEFAULT)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "TRANSVEL(0)(400)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "GOALMODE(PLANNER)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "APPROACHDIST(100)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);

          // Planner Setup
          itemNumber = 2;
          plannerStateClient.getWaitMainStates(mainstates);
          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            plannerStateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }

          ////////////////////
          //Goal 1 first goal
          ////////////////////
          
          str = "DELETEGOAL";
          cmd_planner.set(str);
          smartPlannerParameterSendClient.send(cmd_planner);

          //wait for connections to ensure a clean prompt
          sleep(1);

          std::cout<<"Enter first goal (x)(y): ";
          cin>>inString;
          str = "SETDESTINATIONCIRCLE";
          str.append(inString);
          str.append("(100)");
          cmd_planner.set(str);
          smartPlannerParameterSendClient.send(cmd_planner);

          //wait for communication planner-cdl
          sleep(1);
          speechLock.release();

          itemNumber = 2;

          // CDL State --> GO!
          itemNumber = 2;
          cdlStateClient.getWaitMainStates(mainstates);
          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            cdlStateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }
    


          ////////////////////////////
          //Goal X
          ///////////////////////////
          

          while(true){
            std::cout<<"Enter another Goal? (y/n): ";
            cin>>inString;
            if( strcasecmp("n", inString.c_str() ) != 0 )
            {
              str = "DELETEGOAL";
              cmd_planner.set(str);
              smartPlannerParameterSendClient.send(cmd_planner);

              std::cout<<"Next Goal (x)(y):";
              cin>>inString;
              str = "SETDESTINATIONCIRCLE";
              str.append(inString);
              str.append("(100)");
              cmd_planner.set(str);
              smartPlannerParameterSendClient.send(cmd_planner);
               
              //wait for communication planner-cdl
              sleep(1);
              speechLock.release();

               
            }
            else
            { 
              // CDL State --> STOP!
              itemNumber = 1;
              cdlStateClient.getWaitMainStates(mainstates);
              // check input
              if( itemNumber <= mainstates.size() )
              {
                // iterate to selected state
                std::list<std::string>::iterator iterator1 = mainstates.begin();
                for(unsigned int i=1; i < itemNumber; i++)
                {
                  ++iterator1;
                }
                // send selected state
                cdlStateClient.setWaitState( *iterator1 );
                std::cout << "state <" << *iterator1 << "> sent\n";
              }
              break;  
            }

          }//while(true)

          } //case 2:
          ///////////////////////////////////////
          // Demo 3 move with CDL in reactive mode
          ///////////////////////////////////////
          case 3:
          {
           // Reaktives CDL
           CHS::SendClient<Smart::CommCdlParameter> smartCdlParameterSendClient(component,"smartCdlServer","cdlParameter");

          // CDL SETUP
          str = "STRATEGY(REACTIVE)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "FREEBEHAVIOR(ACTIVATE)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "LOOKUPTABLE(DEFAULT)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "TRANSVEL(0)(800)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          
          std::cout<<"To start the demo set CDL in moverobot state!\n";

           break;
          }
          ///////////////////////////////////////////////
          // Demo 4 move with CDL controlled by joystick
          ///////////////////////////////////////////////
          case 4:
          {
           //joystick demo!! 

          CHS::SendClient<Smart::CommCdlParameter> smartCdlParameterSendClient(component,"smartCdlServer","cdlParameter");
          CHS::SmartStateClient cdlStateClient(component, "smartCdlServer");
          // CDL SETUP
          str = "STRATEGY(JOYSTICK)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "FREEBEHAVIOR(DEACTIVATE)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "LOOKUPTABLE(DEFAULT)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "TRANSVEL(-200)(500)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);
          str = "ROTVEL(-50)(50)";
          cmd.set(str);
          smartCdlParameterSendClient.send(cmd);


          // CDL State --> GO!
          itemNumber = 2;
          cdlStateClient.getWaitMainStates(mainstates);
          // check input
          if( itemNumber <= mainstates.size() )
          {
            // iterate to selected state
            std::list<std::string>::iterator iterator1 = mainstates.begin();
            for(unsigned int i=1; i < itemNumber; i++)
            {
              ++iterator1;
            }
            // send selected state
            cdlStateClient.setWaitState( *iterator1 );
            std::cout << "state <" << *iterator1 << "> sent\n";
          }

          break;
          }
          default:
          {
            break;
          }

	}
        break;
      } // case 8


      // default
      default:
      {
        std::cout << "unsupported menu item !!!\n\n";
        break;
      }

    } // switch( mainMenuItem )

  } // while(true)

  return 0;
}


int main (int argc, char *argv[])
{
  try 
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    component = new CHS::SmartComponent("smartRobotConsole",argc,argv);

    ConsoleThread consoleThread;
    consoleThread.open();

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

