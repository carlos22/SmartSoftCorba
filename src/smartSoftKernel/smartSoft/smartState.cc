// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
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
//
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#include "smartState.hh"


/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////

//
// static handle method called by CORBA object
//
void CHS::SmartStateClient::hndAnswSetMainState(void* ptr,int status)
{
  SmartStateClient *lthis = (SmartStateClient *)ptr;

  lthis->mutex.acquire();
  lthis->result = static_cast<CHS::StatusCode> (status);
  lthis->mutex.release();

  lthis->cond.signal();
}


//
// default constructor
//
CHS::SmartStateClient::SmartStateClient(void) throw(CHS::SmartError)
{
  std::cerr << "CommPattern (state): ERROR: Entered default constructor SmartStateClient" << std::endl;
}


//
// standard constructor
//
CHS::SmartStateClient::SmartStateClient(SmartComponent* m,const std::string& srv) throw(CHS::SmartError)
{
  int flag;

  component = m;

  // Create the CORBA client object
  corbaClient = new StateClient_impl((void *)this,&SmartStateClient::hndAnswSetMainState);

  // activate object to obtain the object reference, need not be registered since object reference
  // is sent to server when state is set
  h = corbaClient->_this();

  // Now connect to the server side of the state pattern
  std::cout << "Try to connect to (" << srv << ":state) ..." << std::endl;
  try {

    CosNaming::Name name;
    name.length(3);
    name[0].id = CORBA::string_dup("orocos");
    name[1].id = CORBA::string_dup(srv.c_str());
    name[2].id = CORBA::string_dup("state");

    do {
      try {
        corbaServant = resolve_name<SmartStateServerPattern>(component->namingService.in(), name);
        flag = 1;
      } catch (const CosNaming::NamingContext::NotFound &) {
        std::cout << "CommPattern (state): INFO: not yet found in naming service ..." << std::endl;
        sleep(1);
        flag = 0;
      }
    } while (flag==0);
  } catch (const CORBA::Exception & e) {
    std::cerr << "CommPattern (state): ERROR: Uncaught CORBA exception: ";
    CHS::operator<<(std::cerr, e) << std::endl;
    throw 0;
  }
  std::cout << "... connected to (" << srv << ":state)" << std::endl;
}


//
// destructor
//
CHS::SmartStateClient::~SmartStateClient(void) throw()
{
  // client does not bind object at naming service,
  // therefore no unbind of objects needed here
}


//
//
//
CHS::StatusCode CHS::SmartStateClient::setWaitState(const std::string& mainstate) throw()
{
  CHS::StatusCode status;

  try {
    corbaServant->setState(mainstate.c_str(),h);
  } catch (CORBA::SystemException &e) {
    std::cerr << "SmartStateClient::setWaitState(): Corba::SystemException : ";
    CHS::operator<<(std::cerr, e) << std::endl;
    return CHS::SMART_ERROR_COMMUNICATION;
  }


  status = component->waitForCond(cond);
  if (status != SMART_OK) return status;

  // now get the status returned from the server
  mutex.acquire();
  status = result;
  mutex.release();

  return status;
}


CHS::StatusCode CHS::SmartStateClient::getWaitCurrentState(std::string& mainstate) throw()
{
  std::cerr << "############# getWaitCurrentState NOT YET IMPLEMENTED !!!" << std::endl;

  return SMART_ERROR;
}


CHS::StatusCode CHS::SmartStateClient::getWaitMainStates(std::list<std::string>& mainstates) throw()
{
  StateList_var l;
  CHS::StatusCode status;

  try {
    status = static_cast<CHS::StatusCode>( corbaServant->getMainStates(l) );
  } catch (CORBA::SystemException &e) {
    std::cerr << "SmartStateClient::getWaitMainStates(): Corba::SystemException : ";
    CHS::operator<<(std::cerr, e) << std::endl;
    // could not get the list of main states because of comm. problems
    return CHS::SMART_ERROR_COMMUNICATION;
  }

  mainstates.clear();
  for (unsigned int i=0;i<l->length();i++) {
    std::string help(l[i]);
    mainstates.push_back(help);
  }

  return status;
}


CHS::StatusCode CHS::SmartStateClient::getWaitSubStates(const std::string& mainstate,std::list<std::string>& substates) throw()
{
  StateList_var l;
  CHS::StatusCode status;

  try {
    status = static_cast<CHS::StatusCode>( corbaServant->getSubStates(mainstate.c_str(),l) );
  } catch (CORBA::SystemException &e) {
    std::cerr << "SmartStateClient::getWaitSubStates(): Corba::SystemException : ";
    CHS::operator<<(std::cerr, e) << std::endl;
    return CHS::SMART_ERROR_COMMUNICATION;
  }

  substates.clear();
  for (unsigned int i=0;i<l->length();i++) {
    std::string help(l[i]);
    substates.push_back(help);
  }

  return status;
}



/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

// Documentation on the state management:
// (see also document on OROCOS communication patterns, C. Schlegel)
//
// Since the state management is crucial in the state server class, I
// shortly explain how the state management is organized. Since at the
// server side, only substates are acquired and released, the "stateList"
// contains all valid substates. Each substate can be found only once
// in that list. The list always contains at least the predefined
// "nonneutral" substate. Each substate contains a list which contains
// all the mainstates to which that particular substate belongs.
// The "nonneutral" substate contains every mainstate but the "neutral"
// mainstate since the "nonneutral" substate is active in every mainstate
// but the "neutral" mainstate.
//
// Activating a particular set of substates by the name of the mainstate
// therefore simply goes through the list of substates to look whether the
// mainstate is contained in the mainstate list of that substate. If yes,
// activate that substate ...
//
// The reason for that structure is that this structure is easy to handle
// with respect to activation / deactivation of sets of substates when
// not all substates have to be deactivated due a state change. Since the
// substates contain the condition variable for signalling, I tried to
// avoid multiple representations of the same substate !
//

//
// static handle method called by CORBA object
//

void CHS::SmartStateServer::hndSetMainState(void *ptr, const SmartStateClientPattern_ptr client,std::string mainstate)
{
  SmartStateServer* lthis = (SmartStateServer *)ptr;
  SmartStateEntry entry;

  entry.state  = mainstate;
  entry.action = SSA_CHANGE_STATE;
  entry.client = SmartStateClientPattern::_duplicate(client);

  lthis->mutex.acquire();

  if (lthis->running == false) {
    //
    // state management not yet activated, send immediately status
    //
    entry.client->answer(SMART_NOTACTIVATED);

    CORBA::release(entry.client);
  } else {
    //
    // enqueue the state change request
    //
    lthis->stateQueue.enqueue_tail(entry);
    lthis->stateQueueSemaphore->release();
  }

  lthis->mutex.release();
}


int CHS::SmartStateServer::hndGetMainStates(void *ptr, StateList_out mainstates)
{
  SmartStateServer* lthis = (SmartStateServer *)ptr;
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<std::string>           stateResults;
  std::list<std::string>::iterator mIterator;
  std::list<std::string>::iterator rIterator;

  StateList_var l = new StateList;
  CORBA::ULong count;
  int flag;
  CHS::StatusCode result;

  lthis->mutex.acquire();

  if (lthis->running == false) {
    //
    // state management not yet activated
    //

    result = SMART_NOTACTIVATED;
  } else {
    //
    // collect all the mainstates distributed over the substates, the standard "neutral"
    // state is pre-added
    //
    std::string neutral("neutral");

    count = 0;

    count++;
    l->length(count);
    l[count-1] = CORBA::string_dup(neutral.c_str());

    stateResults.push_back(neutral);

    //
    // now search for all mainstates distributed over the substate list
    // (not programmed very efficiently due to lack of time, could be
    //  improved but is not a time-critical function :-))
    //
    for (iterator=lthis->stateList.begin(); iterator != lthis->stateList.end(); ++iterator) {
      for (mIterator=iterator->mainstates.begin();mIterator!=iterator->mainstates.end();++mIterator) {
        // now check wether this mainstate is already in the result list ...
        flag = 0;
        for (rIterator=stateResults.begin();rIterator!=stateResults.end();++rIterator) {
          if (*rIterator == *mIterator) {
            flag = 1;
          }
        }
        if (flag == 0) {
          // found new mainstate, add it to the list ...
          stateResults.push_back(*mIterator);

          count++;
          l->length(count);
          l[count-1] = CORBA::string_dup(mIterator->c_str());
        }
      }
    }
    result = SMART_OK;
  }

  // now take away dynamically allocated state list from StateList_var before it goes out of scope
  mainstates = l._retn();

  lthis->mutex.release();

  return result;
}

int CHS::SmartStateServer::hndGetSubStates(void *ptr, std::string mainstate, StateList_out substates)
{
  SmartStateServer* lthis = (SmartStateServer *)ptr;
  StateList_var l = new StateList;
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<std::string>::iterator mIterator;
  CORBA::ULong count;
  CHS::StatusCode result;

  lthis->mutex.acquire();

  if (lthis->running == false) {
    //
    // state management not yet activated
    //

    result = CHS::SMART_NOTACTIVATED;
  } else if (mainstate == "neutral") {
    //
    // "neutral" has no substates
    //
    l->length(0);
    substates = l._retn();

    result = CHS::SMART_OK;
  } else {
    count = 0;
    for (iterator = lthis->stateList.begin(); iterator != lthis->stateList.end(); ++iterator) {
      for (mIterator=iterator->mainstates.begin();mIterator!=iterator->mainstates.end();++mIterator) {
        if (*mIterator == mainstate) {
          // found mainstate in this substate, therefore add substate to result list
          count++;
          l->length(count);
          l[count-1] = CORBA::string_dup(iterator->name.c_str());

          // now end for loop since we have already added this substate ...
          break;
        }
      }
    }

    // now take away dynamically allocated state list from StateList_var before it goes out of scope
    substates = l._retn();

    if (count==0) {
      // couldn't find the mainstate
      result = CHS::SMART_UNKNOWNSTATE;
    } else {
      result = CHS::SMART_OK;
    }
  }

  lthis->mutex.release();

  return result;
}

//
// default constructor
//
// CHS::SmartStateServer::SmartStateServer(void) throw(CHS::SmartError) 
// {
//   std::cerr << "CommPattern (state): ERROR: Entered default constructor SmartStateServer" << std::endl;
// 
//   throw(CHS::SmartError(CHS::SMART_ERROR,"CommPattern (stateServer): ERROR: Entered default constructor SmartStateServer"));
// }

//
// standard constructor
//
CHS::SmartStateServer::SmartStateServer(SmartComponent* m, CHS::StateChangeHandler & hnd) throw(CHS::SmartError)
  : component(m), changeHandler(hnd)
{
  // Create the CORBA server object
  corbaServant = new StateServer_impl((void *)this,
                                      &SmartStateServer::hndSetMainState,
                                      &SmartStateServer::hndGetMainStates,
                                      &SmartStateServer::hndGetSubStates);

  // activate object to obtain the object reference
  SmartStateServerPattern_var tm = corbaServant->_this();

  CosNaming::Name name;

  name.length(1);
  name[0].id = CORBA::string_dup("orocos");
  try {
    CosNaming::NamingContext_var orocos = component->namingService->bind_new_context(name);
  } catch (const CosNaming::NamingContext::AlreadyBound &) {
    // Fine, context already exists
  }

  std::string server = component->getComponentName();
  name.length(2);
  name[1].id = CORBA::string_dup(server.c_str());
  try {
    CosNaming::NamingContext_var compname = component->namingService->bind_new_context(name);
  } catch (const CosNaming::NamingContext::AlreadyBound &) {
    // Fine, context already exists
  }

  name.length(3);
  name[2].id = CORBA::string_dup("state");
  try {
    component->namingService->bind(name,tm.in());
  } catch (const CosNaming::NamingContext::AlreadyBound &) {
    std::cerr << "CommPattern (state): WARNING: service " << name[2].id << " in " << name[1].id << " rebound !" << std::endl;
    component->namingService->rebind(name,tm.in());
  }

  //
  //
  //
  stateQueueSemaphore = new SmartSemaphore(0);

  desiredState.action = SSA_UNDEFINED;
  desiredState.state  = "neutral";
  desiredState.client = 0;

  currentState = std::string("neutral");

  stateUpdateThread.init(this,stateQueueSemaphore);
  stateUpdateThread.open();

  //
  //
  //
  SmartSubStateEntry nonneutral;

  nonneutral.name   = "nonneutral";
  nonneutral.cnt    = 0;
  nonneutral.action = STATE_ACTION_NONE;
  nonneutral.state  = STATE_DEACTIVATED;
  nonneutral.mainstates.clear();

  stateList.push_back(nonneutral);

  running = false;
}

//
// default destructor
//
CHS::SmartStateServer::~SmartStateServer(void) throw()
{
  delete stateQueueSemaphore;

  // unbind from naming service
  try {
    CosNaming::Name name;
    name.length(3);
    name[0].id = CORBA::string_dup("orocos");
    name[1].id = CORBA::string_dup(component->getComponentName().c_str());
    name[2].id = CORBA::string_dup("state");

    component->namingService->unbind(name);
  } catch (const CosNaming::NamingContext::NotFound &) {
    std::cerr << "could not unbind SmartStateServer "
	 << component->getComponentName() << "/"
	 << service << " from naming service" << std::endl;
  } catch (const CORBA::Exception & e) {
    std::cerr << "CORBA Error while unbinding from Naming Service: ";
    CHS::operator<<(std::cerr, e) << std::endl;
    // do not rethrow, continue destruction of object
  }

  std::cout << "TO BE DONE: shut down state thread ...." << std::endl;
  // ##### shutdown state thread
}


//
// internal use only
//
void CHS::SmartStateServer::updateState(void)
{
  std::list<SmartSubStateEntry>::iterator sIterator;
  std::list<std::string>::iterator             mIterator;

  int currentFlag;
  int desiredFlag;
  int lockedFlag;

  int callQuitHandler;
  int callEnterHandler;
  std::list<SmartSubStateEntry>::iterator hIterator;

  int validFlag;

  mutex.acquire();

  if (desiredState.action == SSA_CHANGE_STATE) {
    //
    // there is a pending state change request
    //

    //
    // now check whether all substates to be deactivated
    // are not locked anymore. If all those substates are
    // not acquired anymore, perform the state change. Since
    // this is all done protected by the mutex, no new locks
    // on substates can be set meanwhile. If there are still
    // any substates acquired, postpone the state change.
    //

    if (desiredState.state == currentState) {
      //
      // do nothing since this state is already set
      //
      desiredState.action = SSA_UNDEFINED;

      if (desiredState.client != 0) {
        //
        // if no action is pending, one should not have a client ptr here ....
        //

        desiredState.client->answer(SMART_OK);

        CORBA::release(desiredState.client);
        desiredState.client = 0;
      }

    } else {
      //
      // check whether requested main state is valid at all
      //
      validFlag = 0;
      for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
        for (mIterator=sIterator->mainstates.begin();mIterator!=sIterator->mainstates.end();++mIterator) {
          if (desiredState.state == *mIterator) {
            validFlag = 1;
          }
        }
      }
      if ((validFlag == 1) || (desiredState.state == "neutral") || (desiredState.state == "deactivated")) {
        //
        // required main state is valid and current state is different from desired state
        //
        for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
          // check which substates belong to the current and to the desired main state

          sIterator->action = STATE_ACTION_NONE;

          desiredFlag = 0;
          currentFlag = 0;
          for (mIterator=sIterator->mainstates.begin();mIterator!=sIterator->mainstates.end();++mIterator) {
            if (desiredState.state == *mIterator) {
              desiredFlag = 1;
            }
            if (currentState == *mIterator) {
              currentFlag = 1;
            }
          }
          if ((currentFlag == 1) && (desiredFlag == 0)) {
            // add this substate to the deactivation list
            sIterator->action = STATE_ACTION_DEACTIVATE;
          }
          if ((currentFlag == 0) && (desiredFlag == 1)) {
            // add this substate to the activation list
            sIterator->action = STATE_ACTION_ACTIVATE;
          }
        }

        //
        // now check whether all substates to be deactivated are not
        // locked anymore
        //
        lockedFlag = 0;
        for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
          if ((sIterator->action == STATE_ACTION_DEACTIVATE) && (sIterator->cnt != 0)) {
            lockedFlag = 1;
          }
        }

        if (lockedFlag == 0) {
          // all substates to be released are not locked anymore,
          // therefore perform state change
          if (desiredState.state == std::string("deactivated")) {
            //
            // check whether to reach the neutral state has been
            // enforced. If so, again allow acquiring states which
            // does not cause any problems since states can only be
            // acquired again after we leave this completely mutex 
            // protected block.
            //

            component->blocking(true);
            desiredState.state = std::string("neutral");
          }

          // now call the quit / enter handler. The quitHandler for the nonneutral
          // substate is called AFTER every other quitHandler and the enterHandler
          // for the nonneutral substate BEFORE any other enterHandler

std::cout << "Performing state change: " << currentState << " -> " << desiredState.state << std::endl;

          callQuitHandler  = 0;
          callEnterHandler = 0;
          for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
            if (sIterator->action == STATE_ACTION_DEACTIVATE) {
              if (sIterator->name == "nonneutral") {
                // save iterator for call AFTER every deactivation
                hIterator = sIterator;
                callQuitHandler = 1;
              } else {
                this->changeHandler.handleQuitState(sIterator->name);
                sIterator->state = STATE_DEACTIVATED;
              }
            }
            if ((sIterator->action == STATE_ACTION_ACTIVATE) && (sIterator->name == "nonneutral")) {
              // save iterator for activation call BEFORE every other activation.
              // can use the same hIterator since the nonneutral substates occurs
              // only once in the list of substates
              hIterator = sIterator;
              callEnterHandler = 1;
            }
          }

          if (callQuitHandler == 1) {
            this->changeHandler.handleQuitState(hIterator->name);
            hIterator->state = STATE_DEACTIVATED;
          }
          if (callEnterHandler == 1) {
            this->changeHandler.handleEnterState(hIterator->name);
            hIterator->state = STATE_ACTIVATED;
          }

          for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
            if (sIterator->action == STATE_ACTION_ACTIVATE) {
              if (sIterator->name == "nonneutral") {
                // has already been called BEFORE every other activation
              } else {
                this->changeHandler.handleEnterState(sIterator->name);
                sIterator->state = STATE_ACTIVATED;
              }
            }
          }

          for (sIterator=stateList.begin();sIterator != stateList.end(); ++sIterator) {
            if (sIterator->state == STATE_ACTIVATED) {
              //
              // signal every blocking acquire that this substate is available now !
              //

              // see comment in acquire()-method why we signal every activated substate

              sIterator->cond.signalAll();
            }
          }

          //
          // performed state change
          //
          currentState = desiredState.state;

          desiredState.action = SSA_UNDEFINED;

          if (desiredState.client != 0) {
            //
            // there is a state change pending, therefore send answer
            //
            // (Since this method is left without any action if there are still 
            //  locks on required substates, we have to remember for the next call 
            //  to notify the master as soon as we have finished the state change.
            //  If we arrive here, we successfully finished the state change and
            //  as long as we have a client ptr here, we know that there is a 
            //  master waiting for that notification).

            desiredState.client->answer(SMART_OK);

            CORBA::release(desiredState.client);
            desiredState.client = 0;
          } else {
            //
            //
            //
            std::cout << "UPDATE STATE: no client registered ..." << std::endl;
          }
        }
      } else {
        //
        // requested a state change to an invalid mainstate
        //
        desiredState.action = SSA_UNDEFINED;

        if (desiredState.client != 0) {
          //
          // there is a state change pending, therefore send answer
          //
          desiredState.client->answer(SMART_UNKNOWNSTATE);

          CORBA::release(desiredState.client);
          desiredState.client = 0;
        }
      }
    }

  } else if (desiredState.action == SSA_UNDEFINED) {
    //
    // no pending action, nothing has to be done ...
    //
    desiredState.action = SSA_UNDEFINED;

    if (desiredState.client != 0) {
      //
      // if no action is pending, one should not have a client ptr here ....
      //

std::cerr << "### strange: no action pending, but have client ptr" << std::endl;

      desiredState.client->answer(SMART_ERROR);

      CORBA::release(desiredState.client);
      desiredState.client = 0;
    }
  } else {
    //
    // unknown pending action => ignore and return error to master in case of available ptr
    //
    desiredState.action = SSA_UNDEFINED;

    if (desiredState.client != 0) {
      //
      // there is a client ptr available, therefore client wants to get informed
      //

std::cerr << "### strange: unknown action pending" << std::endl;

      desiredState.client->answer(SMART_ERROR);

      CORBA::release(desiredState.client);
      desiredState.client = 0;
    }
  }

  mutex.release();
}

//
// internal use only
//
void CHS::SmartStateServer::updateStateFromThread(void)
{
  mutex.acquire();

  if (desiredState.client != 0) {
    std::cerr << "WARNING: state change in progress (now killed)" << std::endl;
    // #### hier muß man dem alten master eine entsprechenden status als Antwort schicken !!!!
  }

  stateQueue.dequeue_head(desiredState);

  mutex.release();

  //
  // check whether to cancel waiting calls
  //
  if (desiredState.state == "deactivated") {
    component->blocking(false);
  }

  //
  // now perform the state update
  //
  updateState();
}

//
//
//
CHS::StatusCode CHS::SmartStateServer::defineStates(const std::string& mainstate,const std::string& substate) throw()
{
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator substatePtr;
  SmartSubStateEntry newEntry;
  int countS;
  std::list<std::string>::iterator mIterator;
  int countM;

  CHS::StatusCode result;

  mutex.acquire();
  if (running == true) {
    result = SMART_ACTIVATED;
  } else {
    if ((mainstate == "neutral") || (substate == "nonneutral")) {
      //
      // not allowed to change reserved states
      //
      result = SMART_NOTALLOWED;
    } else {
      //
      // regular state ...
      //

      //
      // first check whether this mainstate is already assigned to the predefined
      // "nonneutral" substate. Every mainstate is automatically assigned to the
      // "nonneutral" substate.
      //
      countS = 0;
      for (iterator=stateList.begin();iterator != stateList.end(); ++iterator) {
        if (iterator->name == "nonneutral") {
          substatePtr = iterator;
          countS++;
        }
      }
      if (countS == 0) {
        //
        // "nonneutral" substate does not exist => error
        //
        result = SMART_ERROR;
      } else if (countS == 1) {
        //
        // found substate "nonneutral", substatePtr points to it, now check whether
        // mainstate is already contained
        //
        countM = 0;
        for (mIterator = substatePtr->mainstates.begin(); mIterator != substatePtr->mainstates.end(); ++mIterator) {
          if (*mIterator == mainstate) countM++;
        }

        if (countM == 0) {
          //
          // mainstate not yet bound with this substate
          //
          substatePtr->mainstates.push_back(mainstate);

          result = SMART_OK;
        } else if (countM == 1) {
          //
          // mainstate already assigned, do nothing
          //
          result = SMART_OK;
        } else {
          //
          // mainstate exists multiple times => error
          //
          result = SMART_ERROR;
        }
      } else {
        //
        // substate exists multiple times => error
        //
        result = SMART_ERROR;
      }


      //
      // if no error occured so far continue to check the substate
      //
      if (result == SMART_OK) {

        //
        // now check whether substate is already in list
        //
        countS = 0;
        for (iterator=stateList.begin();iterator != stateList.end(); ++iterator) {
          if (iterator->name == substate) {
            substatePtr = iterator;
            countS++;
          }
        }
        if (countS == 0) {
          //
          // substate does not yet exist, generate new entry
          //
          newEntry.name   = substate;
          newEntry.cnt    = 0;
          newEntry.action = STATE_ACTION_NONE;
          newEntry.state  = STATE_DEACTIVATED;
          newEntry.mainstates.push_back(mainstate);

          stateList.push_back(newEntry);

          result = SMART_OK;
        } else if (countS == 1) {
          //
          // substate already exists, substatePtr points to it, now check whether mainstate
          // is already contained
          //
          countM = 0;
          for (mIterator = substatePtr->mainstates.begin(); mIterator != substatePtr->mainstates.end(); ++mIterator) {
            if (*mIterator == mainstate) countM++;
          }

          if (countM == 0) {
            //
            // mainstate not yet bound with this substate
            //
            substatePtr->mainstates.push_back(mainstate);

            result = SMART_OK;
          } else if (countM == 1) {
            //
            // mainstate already assigned, do nothing
            //
            result = SMART_OK;
          } else {
            //
            // mainstate exists multiple times => error
            //
            result = SMART_ERROR;
          }
        } else {
          //
          // substate exists multiple times => error
          //
          result = SMART_ERROR;
        }
      }

    }
  }

  mutex.release();

  return result;
}


CHS::StatusCode CHS::SmartStateServer::activate() throw()
{
  mutex.acquire();
  running = true;
  mutex.release();

  return SMART_OK;
}


CHS::StatusCode CHS::SmartStateServer::acquire(const std::string& substate) throw()
{
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  CHS::StatusCode result;

  mutex.acquire();

  if (running == false) {
    //
    // not yet activated
    //
    result = SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator=stateList.begin();iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ...
      //
      if (desiredState.action == SSA_CHANGE_STATE) {
        //
        // state change requested, therefore do not acquire substate until the
        // requested state change has been processed
        //
        mutex.release();

        // IMPORTANT:
        //
        // if state change is requested but required substate is already active, one has
        // to wait until the state change has been performed. Since the substate has been
        // active before the mainstate changed, one would wait here until the next activation
        // of this substate since normally only the newly activated substates are signalled.
        // To avoid this problem, we signal all active substates after a main state change
        // and can therefore safely wait here until the current main state change has been
        // performed.

        sIterator->cond.wait();

        mutex.acquire();

        //
        // state change completed since we got signal
        //
        (sIterator->cnt)++;

        result = SMART_OK;
      } else {
        //
        // no state change in progress
        //
        if (sIterator->state == STATE_ACTIVATED) {
          //
          // requested substate is already active
          //
          (sIterator->cnt)++;

          result = SMART_OK;
        } else {
          //
          // requested substate not active => wait
          //
          mutex.release();

          sIterator->cond.wait();

          mutex.acquire();

          (sIterator->cnt)++;

          result = SMART_OK;
        }
      }
    } else {
      //
      // unknown substate
      //
      result = SMART_UNKNOWNSTATE;
    }
  }

  mutex.release();

  return result;
}


CHS::StatusCode CHS::SmartStateServer::tryAcquire(const std::string& substate) throw()
{
  //TODO: tryAcquire not really tested
  std::cout << "SmartStateServer::tryAcquire ERROR: NOT YET IMPLEMENTED !!!" << std::endl;

  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  CHS::StatusCode result;

  mutex.acquire();

  if (running == false) {
    //
    // not yet activated
    //
    result = SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator=stateList.begin();iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ...
      //
      if (desiredState.action == SSA_CHANGE_STATE) {
        //
        // state change requested, therefore do not acquire substate until the
        // requested state change has been processed
        //
        //mutex.release();

        // IMPORTANT:
        //
        // if state change is requested but required substate is already active, one has
        // to wait until the state change has been performed. Since the substate has been
        // active before the mainstate changed, one would wait here until the next activation
        // of this substate since normally only the newly activated substates are signalled.
        // To avoid this problem, we signal all active substates after a main state change
        // and can therefore safely wait here until the current main state change has been
        // performed.

        //sIterator->cond.wait();

        //mutex.acquire();

        //
        // state change completed since we got signal
        //
        //(sIterator->cnt)++;

        result = SMART_NOTACTIVATED;
      } else {
        //
        // no state change in progress
        //
        if (sIterator->state == STATE_ACTIVATED) {
          //
          // requested substate is already active
          //
          (sIterator->cnt)++;

          result = SMART_OK;
        } else {
          //
          // requested substate not active => wait
          //
          //mutex.release();

          //sIterator->cond.wait();

          //mutex.acquire();

          //(sIterator->cnt)++;

          result = SMART_NOTACTIVATED;
        }
      }
    } else {
      //
      // unknown substate
      //
      result = SMART_UNKNOWNSTATE;
    }
  }

  mutex.release();

  return result;
}
    // Acquire specified substate if available, otherwise return immediately.
    // SMART_OK                  : everything is ok
    // SMART_UNKNOWNSTATE        : returns immediately if the requested state is unknown --OK
    // SMART_NOTACTIVATED        : state object of component not yet activated --OK
    // SMART_ERROR_COMMUNICATION : communication problems
    // SMART_ERROR               : something went wrong
    //
    // (hint: tryAcquire(nonneutral) can be used e.g. in a query handler to decide whether
    //        to process the query or to immediately return an appropriate status code
    //        indicating the components inactivity.)


CHS::StatusCode CHS::SmartStateServer::release(const std::string& substate) throw()
{
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  CHS::StatusCode result;

  mutex.acquire();

  if (running == false) {
    //
    // not yet activated
    //
    result = SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator=stateList.begin();iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ... now check whether the substate is active
      //
      if (sIterator->state == STATE_ACTIVATED) {
        //
        // state is active
        //
        if (sIterator->cnt > 0) {
          //
          // release the substate
          //
          (sIterator->cnt)--;

          result = SMART_OK;
        } else {
          //
          // do nothing since state can not be released any more
          // occurs if you have more releases than acquires
          //

          result = SMART_OK;
        }
      } else {
        //
        // not activated, you never should release a deactivated state,
        // but this is not critical, simply do nothing
        //

        result = SMART_OK;
      }
    } else {
      //
      // substate is not valid
      //

      result = SMART_UNKNOWNSTATE;
    }
  }

  mutex.release();

  //
  // the following call to updateState is necessary since a state change request
  // might still be pending and can now be processed due the release of the last
  // locked substate
  //
  if (result == SMART_OK) {
    this->updateState();
  }

  return result;
}


//
//
//
void CHS::StateUpdateThread::init(SmartStateServer *ptr,SmartSemaphore *sem)
{
  stateServer         = ptr;
  stateQueueSemaphore = sem;
}

int CHS::StateUpdateThread::svc(void)
{
  while(1) {
    stateQueueSemaphore->acquire();
    stateServer->updateStateFromThread();
  }
  return 0;
}

