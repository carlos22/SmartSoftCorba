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

#include "smartCommWiring.hh"

CHS::SmartCommWiring::SmartCommWiring()
{
}

CHS::SmartCommWiring::~SmartCommWiring()
{
}

void CHS::SmartCommWiring::get(CORBA::Any &a) const
{
  a <<= wiring;
}

void CHS::SmartCommWiring::set(const CORBA::Any &a)
{
  SmartWiring *w;

  a >>= w;

  wiring = *w;
}

void CHS::SmartCommWiring::setCommand(const std::string cmd,const std::string slaveprt,const std::string servercmpt,const std::string serversvc)
{
  wiring.command         = cmd.c_str();
  wiring.slaveport       = slaveprt.c_str();
  wiring.servercomponent = servercmpt.c_str();
  wiring.serverservice   = serversvc.c_str();
}

void CHS::SmartCommWiring::getCommand(std::string& cmd,std::string& slaveprt,std::string& servercmpt,std::string& serversvc) const
{
  cmd        = wiring.command;
  slaveprt   = wiring.slaveport;
  servercmpt = wiring.servercomponent;
  serversvc  = wiring.serverservice;
}

void CHS::SmartCommWiring::setStatus(const CHS::StatusCode status)
{
  wiring.status = status;
}

void CHS::SmartCommWiring::getStatus(CHS::StatusCode& status) const
{
  status = (CHS::StatusCode)wiring.status;
}

