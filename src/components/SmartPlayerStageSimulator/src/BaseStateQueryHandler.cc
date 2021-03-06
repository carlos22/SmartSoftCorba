//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
//  Copyright (C) 2009 Andreas Steck, Matthias Lutz
//
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2.1
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this library; if not, write to the Free Software Foundation, Inc.,
//  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#include "BaseStateQueryHandler.hh"
#include "gen/SmartPlayerStageSimulator.hh"

#include <iostream>

void BaseStateQueryHandler::handleQuery(CHS::QueryServer<
		CommBasicObjects::CommVoid, CommBasicObjects::CommBaseState> & server,
		const CHS::QueryId id, const CommBasicObjects::CommVoid & request) throw ()
{
    std::cout << "Query for basestate \n";

    COMP->PlayerClientMutex.acquire();

    COMP->robot->ReadIfWaiting();

    base_position.set_x(COMP->position_2d_proxy->GetXPos(), 1.0);
    base_position.set_y(COMP->position_2d_proxy->GetYPos(), 1.0);
    base_position.set_z(0);
    base_position.set_base_alpha(COMP->position_2d_proxy->GetYaw());

    double xSpeed;
    double ySpeed;
    xSpeed = COMP->position_2d_proxy->GetXSpeed();
    ySpeed = COMP->position_2d_proxy->GetYSpeed();
    base_velocity.set_v(((xSpeed + ySpeed) / 2.0) * 1000.0);

    double yawSpeed;
    yawSpeed = COMP->position_2d_proxy->GetYawSpeed();
    base_velocity.set_omega_base(yawSpeed);

    COMP->PlayerClientMutex.release();



    base_state.set_base_position(base_position);
    base_state.set_base_velocity(base_velocity);

    base_state.set_base_raw_position(COMP->rawPos); // TODO: check!


    server.answer(id, base_state);
}
