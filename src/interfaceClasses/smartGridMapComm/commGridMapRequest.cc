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
//  This file is part of the "SmartSoft Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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

#include "commGridMapRequest.hh"

using namespace Smart;

CommGridMapRequest::CommGridMapRequest()
{
  _gridmaprequest.id              = 0;
  _gridmaprequest.xOffsetMM       = 0;
  _gridmaprequest.yOffsetMM       = 0;
  _gridmaprequest.xSizeMM         = 0;
  _gridmaprequest.ySizeMM         = 0;
  _gridmaprequest.curLtmState     = false;
  _gridmaprequest.curLtmThreshold = 0;
}


CommGridMapRequest::CommGridMapRequest(const SmartIDL::GridMapRequest &gridmaprequest)
: _gridmaprequest(gridmaprequest)
{
}


void CommGridMapRequest::get(CORBA::Any &a) const
{
  a <<= _gridmaprequest;
}


CommGridMapRequest::~CommGridMapRequest()
{
}


void CommGridMapRequest::set(const CORBA::Any &a)
{
  SmartIDL::GridMapRequest *tmp_gridmaprequest = 0;
  if(a >>= tmp_gridmaprequest)
  {
    _gridmaprequest = *tmp_gridmaprequest;
  }
}


int CommGridMapRequest::get_parameter( unsigned int &id,
                                       int &xOffsetMM,
                                       int &yOffsetMM,
                                       unsigned int &xSizeMM,
                                       unsigned int &ySizeMM,
                                       bool &curLtmState,
                                       unsigned int &curLtmThreshold ) const
{
  id              = _gridmaprequest.id;
  xOffsetMM       = _gridmaprequest.xOffsetMM;
  yOffsetMM       = _gridmaprequest.yOffsetMM;
  xSizeMM         = _gridmaprequest.xSizeMM;
  ySizeMM         = _gridmaprequest.ySizeMM;
  curLtmState     = _gridmaprequest.curLtmState;
  curLtmThreshold = _gridmaprequest.curLtmThreshold;
  return 0;
}

int CommGridMapRequest::set_parameter( unsigned int id,
                                       int xOffsetMM,
                                       int yOffsetMM,
                                       unsigned int xSizeMM,
                                       unsigned int ySizeMM,
                                       bool curLtmState,
                                       unsigned int curLtmThreshold )
{
  _gridmaprequest.id              = id;
  _gridmaprequest.xOffsetMM       = xOffsetMM;
  _gridmaprequest.yOffsetMM       = yOffsetMM;
  _gridmaprequest.xSizeMM         = xSizeMM;
  _gridmaprequest.ySizeMM         = ySizeMM;
  _gridmaprequest.curLtmState     = curLtmState;
  _gridmaprequest.curLtmThreshold = curLtmThreshold;
  return 0;
}

