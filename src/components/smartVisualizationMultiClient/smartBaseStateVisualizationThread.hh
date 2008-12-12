// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// --------------------------------------------------------------------------

#ifndef SMART_BASE_STATE_VISUALIZATION_THREAD_HH
#define SMART_BASE_STATE_VISUALIZATION_THREAD_HH

#include "commBaseState.hh"
#include "smartBaseStatePaintMethod.hh"
#include "smartVisualizationThread.hh"

namespace Smart {

class BaseStateVisualizationThread: public VisualizationThreadBase
{
public:
  BaseStateVisualizationThread
  (
    CHS::SmartComponent *component,
    const std::string &component_name,
    CHS::SendClient<Smart::CommPropertySet> &registration_client,
    CHS::SendClient<Faw::TransmittablePainter> &drawing_client
  )
  : VisualizationThreadBase(component,component_name,registration_client,drawing_client)
  {
  }

  virtual BaseStateVisualizationThread *clone() const;

  int svc();
  
protected:
  virtual void set_additional_parameters(const CHS::SmartParameter &parameters, const std::string &grp);

private:
  Faw::DefaultPaintMethod<Smart::CommBaseState> _paint_method;  
};

} // namespace Smart

#endif
