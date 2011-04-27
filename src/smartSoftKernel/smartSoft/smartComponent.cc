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

#include "smartComponent.hh"

////////////////////////////////////////////////////////////
//
// functions for component
//
////////////////////////////////////////////////////////////

//
// constructor
//
CHS::SmartComponent::SmartComponent( const std::string &name,
                                     int &argc,
                                     char **argv,
                                     const char *compileDate,
                                     const char *compileTime  )
{
  // condition list handling
  condList    = 0;
  cntCondList = 0;
  condCancel  = false;

  listCVwithMemory    = 0;
  listCVwithoutMemory = 0;

  statusComponentBlocking = true;

  componentName = name;

  // Initialize orb
  orb = CORBA::ORB_init(argc, argv);

  // Get reference to Root POA.
  obj = orb->resolve_initial_references ("RootPOA");
  poa = PortableServer::POA::_narrow (obj.in ());

  // Activate POA manager
  mgr = poa->the_POAManager ();
  mgr->activate ();

  // Resolve Naming Service. Needed by the different Servers
  namingService = resolve_init<CosNaming::NamingContext>(orb.in(),"NameService");

  //
  // this is the place where to put in policy list for POA
  //

  // start our timer thread.
  if (timerThread.activate (THR_NEW_LWP) == -1) {
    throw std::logic_error("Could not create timerThread");
  }
  
  // register signal handler within TAO/ACE
  TAO_ORB_Core_instance()->reactor()-> register_handler (SIGINT, this);

  std::cout << "\nComponent " << name << " started\ncompiled: " << compileDate << " " << compileTime << "\n\n";
}


//
// destructor
//
CHS::SmartComponent::~SmartComponent( void )
{
  try {
    CosNaming::Name name;
    name.length(2);
    name[0].id = CORBA::string_dup("orocos");
    name[1].id = CORBA::string_dup(componentName.c_str());
    namingService->unbind(name);
  } catch (CosNaming::NamingContext::NotFound &) {
    // valid if component didn't register any servers (client only components)
  } catch (CORBA::Exception & e) {
    std::cerr << "Error while unbinding orocos/" << componentName << " from naming service: ";
    CHS::operator<<(std::cerr, e) << std::endl;
  } catch (...) {
    std::cerr << "Unexpected exception" << std::endl;
  }

  poa->destroy(1,1);
  orb->destroy();
}

//
// default signal handler
//
int CHS::SmartComponent::handle_signal (int signum, siginfo_t *, ucontext_t *)
{
  // ##### try to prevent race condition. use a real synchronisation class here..
  static bool run=false;
  if (run){
    return 0;
  }
  run = true;
  std::cerr << "Component Signal " << signum << " received" << std::endl;
  if (signum == SIGINT) {
    std::cerr << "SIGINT received. handle this signal correctly by overriding SmartComponent::handle_signal()" << std::endl
	 << "calling exit(0), ressources won't be freed" << std::endl;
    exit(0);
  }
  return 0;
};

//
// starts the communication stuff
//
CHS::StatusCode CHS::SmartComponent::run( void )
{
  try {
    // Accept requests
    orb->run();
  }
  catch (const CORBA::Exception & e) {
    std::cerr << "Uncaught CORBA exception in run() from component " << componentName << ": ";
    CHS::operator<<(std::cerr, e) << std::endl;

    return CHS::SMART_ERROR_UNKNOWN;
  }
  return CHS::SMART_OK;
}

std::string CHS::SmartComponent::getComponentName(void) throw()
{
  return componentName;
}

CHS::StatusCode CHS::SmartComponent::blocking(const bool b) throw()
{
  CHS::StatusCode result = CHS::SMART_OK;
  SmartCVwithMemoryStruct    *ptr1;
  SmartCVwithoutMemoryStruct *ptr2;

  mutex.acquire();

  statusComponentBlocking = b;

  for (ptr1 = listCVwithMemory; ptr1 != 0; ptr1 = ptr1->next) {
    ptr1->cond->blockingComponent(statusComponentBlocking);
  } 
  for (ptr2 = listCVwithoutMemory; ptr2 != 0; ptr2 = ptr2->next) {
    ptr2->cond->blockingComponent(statusComponentBlocking);
  } 

  if (b == false) {
    //
    // startCancelCond()
    //
    CondListStruct            *ptr;
    CntCondListStruct         *pptr;

    // set flag
    condCancel = true;
    // signal all waiting conditions
    ptr = condList;
    while( ptr != 0 ) {
      (ptr->cond)->signalAll();
      ptr = ptr->next;
    }
    pptr = cntCondList;
    while( pptr != 0 ) {
      (pptr->cond)->signal();
      pptr = pptr->next;
    }
  }

  if (b == true) {
    //
    // stopCancelCond()
    //

    // reset flag
    condCancel = false;
  }

  mutex.release();

  return result;
}


/////////////////////////////////////////////////////////////////////////
// internal functions
/////////////////////////////////////////////////////////////////////////

CHS::StatusCode CHS::SmartComponent::waitForCond( SmartCondClass& cond )
{
  CHS::StatusCode  result = CHS::SMART_OK;
  CondListStruct   *ptr   = 0;
  CondListStruct   *hPtr  = 0;

  // test already canceling or insert in list
  mutex.acquire();
  if( condCancel ) {
    result = CHS::SMART_CANCELLED;
  } else {
    ptr = new CondListStruct;
    ptr->cond = &cond;
    ptr->next = condList;
    condList = ptr;
  }
  mutex.release();

  if( result != CHS::SMART_OK ) return result;

  // wait
  cond.wait();

  mutex.acquire();
  // test result
  if( condCancel ) result = CHS::SMART_CANCELLED;

  // delete from list
  if( ptr == condList ) {
    condList = ptr->next;
    delete ptr;
  } else {
    hPtr = condList;
    while( hPtr->next != ptr ) hPtr = hPtr->next;
    hPtr->next = hPtr->next->next;
    delete ptr;
  }
  mutex.release();

  return result;
}

CHS::StatusCode CHS::SmartComponent::waitForCond( SmartCntCondClass& cond )
{
  CHS::StatusCode   result = CHS::SMART_OK;
  CntCondListStruct *ptr   = 0;
  CntCondListStruct *hPtr  = 0;

  // test already canceling or insert in list
  mutex.acquire();
  if( condCancel ) {
    result = CHS::SMART_CANCELLED;
  } else {
    ptr = new CntCondListStruct;
    ptr->cond = &cond;
    ptr->next = cntCondList;
    cntCondList = ptr;
  }
  mutex.release();

  if( result != CHS::SMART_OK ) return result;

  // wait
  cond.wait();

  mutex.acquire();
  // test result
  if( condCancel ) result = CHS::SMART_CANCELLED;

  // delete from list
  if( ptr == cntCondList ) {
    cntCondList = ptr->next;
    delete ptr;
  } else {
    hPtr = cntCondList;
    while( hPtr->next != ptr ) hPtr = hPtr->next;
    hPtr->next = hPtr->next->next;
    delete ptr;
  }
  mutex.release();

  return result;
}





CHS::StatusCode CHS::SmartComponent::addCV(SmartCVwithMemory* cond) throw()
{
  CHS::StatusCode result = CHS::SMART_OK;
  SmartCVwithMemoryStruct *cptr;

  mutex.acquire();

  cptr             = new SmartCVwithMemoryStruct;
  cptr->cond       = cond;
  cptr->next       = listCVwithMemory;
  listCVwithMemory = cptr;

  cptr->cond->blockingComponent(statusComponentBlocking);

  mutex.release();

  return result;
}

CHS::StatusCode CHS::SmartComponent::removeCV(SmartCVwithMemory* cond) throw()
{
  CHS::StatusCode result = CHS::SMART_OK;
  SmartCVwithMemoryStruct *cptr;
  SmartCVwithMemoryStruct *hcptr;

  mutex.acquire();
  
  // normally a condition variable is at most once in the list but you never know ...

  // 
  // first remove entries from head as long as they match the condition variable
  //
  while ((listCVwithMemory != 0) && (listCVwithMemory->cond == cond)) {
    cptr = listCVwithMemory;
    listCVwithMemory = listCVwithMemory->next;
    delete cptr;
  }

  //
  // now search through the rest of the list which might be empty already. If the
  // length is greater than zero, than the first element never matches and never
  // gets deleted
  //
  cptr = listCVwithMemory;
  if (cptr == 0) {
    // list contains no element anymore => ready
  } else {
    // list can contain one or more elements with the first element matching never
    while (cptr->next != 0) {
      if (cptr->next->cond == cond) {
        hcptr      = cptr->next;
        cptr->next = cptr->next->next;
        delete hcptr;
      } else {
        cptr = cptr->next;
      }
    }
  }

  mutex.release();

  return result;
}

CHS::StatusCode CHS::SmartComponent::addCV(SmartCVwithoutMemory* cond) throw()
{
  CHS::StatusCode result = CHS::SMART_OK;
  SmartCVwithoutMemoryStruct *cptr;

  mutex.acquire();

  cptr                = new SmartCVwithoutMemoryStruct;
  cptr->cond          = cond;
  cptr->next          = listCVwithoutMemory;
  listCVwithoutMemory = cptr;

  cptr->cond->blockingComponent(statusComponentBlocking);

  mutex.release();

  return result;
}

CHS::StatusCode CHS::SmartComponent::removeCV(SmartCVwithoutMemory* cond) throw()
{
  CHS::StatusCode result = CHS::SMART_OK;
  SmartCVwithoutMemoryStruct *cptr;
  SmartCVwithoutMemoryStruct *hcptr;

  mutex.acquire();

  // normally a condition variable is at most once in the list but you never know ...

  //
  // first remove entries from head as long as they match the condition variable
  //
  while ((listCVwithoutMemory != 0) && (listCVwithoutMemory->cond == cond)) {
    cptr = listCVwithoutMemory;
    listCVwithoutMemory = listCVwithoutMemory->next;
    delete cptr;
  }

  //
  // now search through the rest of the list which might be empty already. If the
  // length is greater than zero, than the first element never matches and never
  // gets deleted
  //
  cptr = listCVwithoutMemory;
  if (cptr == 0) {
    // list contains no element anymore => ready
  } else {
    // list can contain one or more elements with the first element matching never
    while (cptr->next != 0) {
      if (cptr->next->cond == cond) {
        hcptr      = cptr->next;
        cptr->next = cptr->next->next;
        delete hcptr;
      } else {
        cptr = cptr->next;
      }
    }
  }

  mutex.release();

  return result;
}






/////////////////////////////////////////////////////////////////////////
//
// additional functions
//
/////////////////////////////////////////////////////////////////////////

//
// smartsoft semaphore like condition variable
//
CHS::SmartCntCondClass::SmartCntCondClass()
{
  v = 0;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCntCondClass::~SmartCntCondClass()
{
  delete cond;
}

CHS::SmartCntCondClass::SmartCntCondClass(const CHS::SmartCntCondClass& s)
{
  v = s.v;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCntCondClass& CHS::SmartCntCondClass::operator=(const CHS::SmartCntCondClass& s)
{
  if (this != &s) {
    delete cond;

    v = s.v;
    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void CHS::SmartCntCondClass::signal(void)
{
  mutex.acquire();
  v=1;
  cond->signal();
  mutex.release();
}

void CHS::SmartCntCondClass::wait(void)
{
  mutex.acquire();
  if (v == 0) {
    cond->wait();
  }
  v=0;
  mutex.release();
}

int CHS::SmartCntCondClass::test(void)
{
  int res;

  mutex.acquire();
  res = v;
  mutex.release();

  return res;
}







//
// smartsoft  condition variable
//
CHS::SmartCondClass::SmartCondClass()
{
  v = 0;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCondClass::~SmartCondClass()
{
  delete cond;
}

CHS::SmartCondClass::SmartCondClass(const CHS::SmartCondClass& s)
{
  v = s.v;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCondClass& CHS::SmartCondClass::operator=(const CHS::SmartCondClass& s)
{
  if (this != &s) {
    delete cond;

    v = s.v;
    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

int CHS::SmartCondClass::signalOne(void)
{
  int res;

  mutex.acquire();
  res = v--;
  cond->signal();
  mutex.release();

  return res;
}

int CHS::SmartCondClass::signalAll(void)
{
  int res;

  mutex.acquire();
  res = v;
  v   = 0;
  cond->broadcast();
  mutex.release();

  return res;
}

int CHS::SmartCondClass::wait(void)
{
  mutex.acquire();
  v++;
  cond->wait();
  mutex.release();

  return 0;
}








//
//
//
CHS::SmartCVwithoutMemory::SmartCVwithoutMemory()
{
  statusBlockingComponent = true;
  statusBlockingPattern   = true;
  statusBlockingUser      = true;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCVwithoutMemory::~SmartCVwithoutMemory()
{
  delete cond;
}

CHS::SmartCVwithoutMemory::SmartCVwithoutMemory(const CHS::SmartCVwithoutMemory& s)
{
  // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
  statusBlockingComponent = s.statusBlockingComponent;
  statusBlockingPattern   = s.statusBlockingPattern;
  statusBlockingUser      = s.statusBlockingUser;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCVwithoutMemory& CHS::SmartCVwithoutMemory::operator=(const CHS::SmartCVwithoutMemory& s)
{
  if (this != &s) {
    delete cond;

    // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
    statusBlockingComponent = s.statusBlockingComponent;
    statusBlockingPattern   = s.statusBlockingPattern;
    statusBlockingUser      = s.statusBlockingUser;

    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void CHS::SmartCVwithoutMemory::blockingComponent(const bool b)
{
  mutex.acquire();
  statusBlockingComponent = b;
  if (statusBlockingComponent == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithoutMemory::blockingPattern(const bool b)
{
  mutex.acquire();
  statusBlockingPattern = b;
  if (statusBlockingPattern == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithoutMemory::blockingUser(const bool b)
{
  mutex.acquire();
  statusBlockingUser = b;
  if (statusBlockingUser == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithoutMemory::signal()
{
  mutex.acquire();
  cond->broadcast();
  mutex.release();
}

CHS::StatusCode CHS::SmartCVwithoutMemory::wait()
{
  int flag;
  CHS::StatusCode result = CHS::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if ((statusBlockingPattern == true) && (statusBlockingUser == true) && (statusBlockingComponent == true)) {
      // wait for the next signal
      cond->wait();
      // signal can be either from change of blocking mode or a true signal
      if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
        // still both blocking therefore true signal
        flag   = 1;
        result = CHS::SMART_OK;
      } else {
        // non blocking mode ...
        flag   = 1;
        result = CHS::SMART_CANCELLED;
      }
    } else {
      // return immediately
      flag   = 1;
      result = CHS::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}


//
//
//
CHS::SmartCVwithMemory::SmartCVwithMemory()
{
  signalled               = false;
  statusBlockingComponent = true;
  statusBlockingPattern   = true;
  statusBlockingUser      = true;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCVwithMemory::~SmartCVwithMemory()
{
  delete cond;
}

CHS::SmartCVwithMemory::SmartCVwithMemory(const CHS::SmartCVwithMemory& s)
{
  // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
  signalled               = s.signalled;
  statusBlockingComponent = s.statusBlockingComponent;
  statusBlockingPattern   = s.statusBlockingPattern;
  statusBlockingUser      = s.statusBlockingUser;

  cond = new SmartConditionMutex(mutex);
}

CHS::SmartCVwithMemory& CHS::SmartCVwithMemory::operator=(const CHS::SmartCVwithMemory& s)
{
  if (this != &s) {
    delete cond;

    // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
    signalled               = s.signalled;
    statusBlockingComponent = s.statusBlockingComponent;
    statusBlockingPattern   = s.statusBlockingPattern;
    statusBlockingUser      = s.statusBlockingUser;

    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void CHS::SmartCVwithMemory::blockingComponent(const bool b)
{
  mutex.acquire();
  statusBlockingComponent = b;
  if (statusBlockingComponent == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithMemory::blockingPattern(const bool b)
{
  mutex.acquire();
  statusBlockingPattern = b;
  if (statusBlockingPattern == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithMemory::blockingUser(const bool b)
{
  mutex.acquire();
  statusBlockingUser = b;
  if (statusBlockingUser == false) {
    cond->broadcast();
  }
  mutex.release();
}

void CHS::SmartCVwithMemory::signal()
{
  mutex.acquire();
  signalled = true;
  cond->broadcast();
  mutex.release();
}

CHS::StatusCode CHS::SmartCVwithMemory::wait()
{
  int flag;
  CHS::StatusCode result = CHS::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if (signalled == true) {
      flag      = 1;
      signalled = false;
      result    = CHS::SMART_OK;
    } else if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
      cond->wait();
    } else {
      flag   = 1;
      result = CHS::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}

void CHS::SmartCVwithMemory::reset()
{
  mutex.acquire();
  signalled = false;
  mutex.release();
}

