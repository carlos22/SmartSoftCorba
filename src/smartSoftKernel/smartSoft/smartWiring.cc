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

#include "smartWiring.hh"

/////////////////////////////////////////////////////////////////////////
//
// master part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
CHS::WiringMaster::WiringMaster(void) throw(CHS::SmartError)
{
  std::cerr << "CommPattern (WiringMaster): ERROR: Entered default constructor WiringMaster" << std::endl;

  throw(CHS::SmartError(CHS::SMART_ERROR,"CommPattern (WiringMaster): ERROR: Entered default constructor WiringMaster"));
}

//
// standard constructor
//
CHS::WiringMaster::WiringMaster(SmartComponent* m) throw(CHS::SmartError) : component(m)
{
}

//
// destructor
//
CHS::WiringMaster::~WiringMaster(void) throw()
{
  // client does not bind object at naming service,
  // therefore no unbind of objects needed here
}

//
//
//
CHS::StatusCode CHS::WiringMaster::connect(const std::string& slavecmpt,const std::string& slaveprt, const std::string& servercmpt,const std::string& serversvc) throw()
{
  CHS::QueryClient<SmartCommWiring,SmartCommWiring> client(component);
  SmartCommWiring request,answer;
  CHS::StatusCode status;

  // connect manually since connect with constructor is retrying
  status = client.connect(slavecmpt,"wiring");
  switch (status) {
    case CHS::SMART_OK:
      // everything ok
      break;
    case CHS::SMART_SERVICEUNAVAILABLE:
      // the servercmpt is unknown or does not provide a wiring slave
      return CHS::SMART_UNKNOWNCOMPONENT;
      break;
    case CHS::SMART_INCOMPATIBLESERVICE:
      // the "wiring" service of the servercmpt is not compatible to the master
      return CHS::SMART_UNKNOWNCOMPONENT;
      break;
    case CHS::SMART_ERROR_COMMUNICATION:
      // problems while connecting to the slave
      return CHS::SMART_ERROR_COMMUNICATION;
      break;
    default:
      // problems while connecting to the slave
      return CHS::SMART_ERROR;
      break;
  }

  request.setCommand("connect",slaveprt,servercmpt,serversvc);

  status = client.query(request,answer);
  if (status != CHS::SMART_OK) return status;

  answer.getStatus(status);

  return status;
}
  
//
//
//
CHS::StatusCode CHS::WiringMaster::disconnect(const std::string& slavecmpt,const std::string& slaveprt) throw()
{
  CHS::QueryClient<SmartCommWiring,SmartCommWiring> client(component);
  SmartCommWiring request,answer;
  CHS::StatusCode status;

  // connect manually since connect with constructor is retrying
  status = client.connect(slavecmpt,"wiring");
  switch (status) {
    case CHS::SMART_OK:
      // everything ok
      break;
    case CHS::SMART_SERVICEUNAVAILABLE:
      // the servercmpt is unknown or does not provide a wiring slave
      return CHS::SMART_UNKNOWNCOMPONENT;
      break;
    case CHS::SMART_INCOMPATIBLESERVICE:
      // the "wiring" service of the servercmpt is not compatible to the master
      return CHS::SMART_UNKNOWNCOMPONENT;
      break;
    case CHS::SMART_ERROR_COMMUNICATION:
      // problems while connecting to the slave
      return CHS::SMART_ERROR_COMMUNICATION;
      break;
    default:
      // problems while connecting to the slave
      return CHS::SMART_ERROR;
      break;
  }

  request.setCommand("disconnect",slaveprt,"","");

  status = client.query(request,answer);
  if (status != CHS::SMART_OK) return status;

  answer.getStatus(status);

  return status;
}


/////////////////////////////////////////////////////////////////////////
//
// handler of slave part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
CHS::WiringHandler::WiringHandler(void) throw(CHS::SmartError)
{
  std::cerr << "CommPattern (WiringHandler): ERROR: Entered default constructor WiringHandler" << std::endl;

  throw(CHS::SmartError(CHS::SMART_ERROR,"CommPattern (WiringHandler): ERROR: Entered default constructor WiringHandler"));
}

//
// standard constructor
//
CHS::WiringHandler::WiringHandler(CHS::WiringSlave *s) throw()
  : wiringslave(s)
{
  //
}

//
// destructor
//
CHS::WiringHandler::~WiringHandler() throw()
{
  //
}

//
//
//
void CHS::WiringHandler::handleQuery(QueryServer<SmartCommWiring,SmartCommWiring> & server, const QueryId id, const SmartCommWiring & request) throw()
{
  SmartCommWiring answer;

  answer = wiringslave->handleWiring(request);

  server.answer(id,answer);
}




/////////////////////////////////////////////////////////////////////////
//
// slave part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
CHS::WiringSlave::WiringSlave(void) throw(CHS::SmartError)
{
  std::cerr << "CommPattern (WiringSlave): ERROR: Entered default constructor WiringSlave" << std::endl;

  throw(CHS::SmartError(CHS::SMART_ERROR,"CommPattern (WiringSlave): ERROR: Entered default constructor WiringSlave"));
}

//
//
//
CHS::WiringSlave::WiringSlave(SmartComponent* m) throw(CHS::SmartError) : component(m)
{
  // reset port list
  ports = 0;

  handler = new WiringHandler(this);

  wiring  = new QueryServer<SmartCommWiring,SmartCommWiring>(component,"wiring",*handler);
}

//
//
//
CHS::WiringSlave::~WiringSlave(void) throw()
{
  PortList *lptr;

  mutex.acquire();

  // free memory
  while ( ports != 0 ) {
    lptr  = ports;
    ports = ports->next;
    delete lptr;
  }

  mutex.release();
}

//
//
//
CHS::SmartCommWiring CHS::WiringSlave::handleWiring(const CHS::SmartCommWiring& request)
{
  CHS::SmartCommWiring answer;
  CHS::StatusCode      status;
  std::string          command,portname,servername,servicename;
  PortList             *lptr = 0;

  request.getCommand(command,portname,servername,servicename);
  
  mutex.acquire();

  if (command == "connect") {
    //
    // look for the portname
    //  
    for( lptr = ports; lptr != 0; lptr = lptr->next )
      if( lptr->portname == portname)
        break;

    if (lptr != 0) {
      //
      // portname known, now call the connect method of the client object
      //
      status = (lptr->cPtr)(lptr->tPtr,servername,servicename);      

      answer.setStatus(status);
    } else {
      answer.setStatus(CHS::SMART_UNKNOWNPORT);
    }
  } else if (command == "disconnect") {
    //
    // look for the portname
    //
    for( lptr = ports; lptr != 0; lptr = lptr->next )
      if( lptr->portname == portname)
        break;

    if (lptr != 0) {
      //
      // portname known, now call the disconnect method of the client object
      //
      status = (lptr->dPtr)(lptr->tPtr);

      answer.setStatus(status);
    } else {
      answer.setStatus(CHS::SMART_UNKNOWNPORT);
    }
  } else {
    answer.setStatus(CHS::SMART_ERROR);
  }

  mutex.release();

  return answer;
}


//
//
//
CHS::StatusCode CHS::WiringSlave::add(const std::string& port, void* tPtr, CHS::StatusCode (*cPtr)(void*,const std::string&,const std::string&), CHS::StatusCode (*dPtr)(void*))
{
  PortList        *lptr;
  CHS::StatusCode result;

  mutex.acquire();

  // first check, whether portname is already in use
  for( lptr = ports; lptr != 0; lptr = lptr->next )
    if( lptr->portname == port)
      break;

  if (lptr != 0) {
    //
    // portname already used
    //
    result = CHS::SMART_PORTALREADYUSED;
  } else {
    //
    // add client to list of ports
    //
    lptr           = new PortList;
    lptr->portname = port;
    lptr->tPtr     = tPtr;
    lptr->cPtr     = cPtr;
    lptr->dPtr     = dPtr;
    lptr->next     = ports;
    ports          = lptr;

    result = CHS::SMART_OK;
  }

  mutex.release();

  return result;
}

//
//
//
CHS::StatusCode CHS::WiringSlave::remove(const std::string& port)
{
  PortList        *lptr, *hlptr;
  CHS::StatusCode result;

  mutex.acquire();

  // first check, whether portname is known
  for( lptr = ports; lptr != 0; lptr = lptr->next )
    if( lptr->portname == port)
      break;

  if (lptr != 0) {
    //
    // found port to be removed and now delete list entry
    //
    if ( lptr == ports) {
      ports = lptr->next;
      delete lptr;
    } else {
      for (hlptr = ports; hlptr && hlptr->next != 0; hlptr = hlptr->next) {
        if( hlptr->next == lptr) {
          hlptr->next = hlptr->next->next;
          delete lptr;
        }
      }
    }
    result = CHS::SMART_OK;
  } else {
    result = CHS::SMART_UNKNOWNPORT;
  }

  mutex.release();

  return result;
}

