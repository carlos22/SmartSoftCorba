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
//  This file is part of the "SmartSoft robot console component".
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
#include "commMapperParameter.hh"
#include "commForkLiftCommand.hh"
#include "commCdlParameter.hh"

CHS::SmartComponent *component;


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
        std::cout << "not implemented yet !!!\n\n";       
        break;
      } // case 3 



      // 4 - Planner parameter
      case 4:
      {
        std::cout << "not implemented yet !!!\n\n";       
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
        std::cout << "STRATEGY REACTIVE/JOYSTICK/TURN             : set the cdl strategy\n";
        std::cout << "FREEBEHAVIOR ACTIVATE/DEACTIVATE            : freebehavior in stall situation\n";
        std::cout << "LOOKUPTABLE DEFAULT/SECOND                  : set cdl lookup table\n";
        std::cout << "TRANSVEL ?vmin ?vmax                        : set translation velocity min,max in mm/s\n";
        std::cout << "ROTVEL ?wmin ?wmax (deg/sec)                : set rotational velocity min,max in deg/s\n";
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

