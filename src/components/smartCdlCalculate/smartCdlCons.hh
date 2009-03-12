//--------------------------------------------------------------------------
//
//  Copyright (C) 1998 Christian Schlegel
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotic Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
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

#ifndef _SMARTCDLCONS_H
#define _SMARTCDLCONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define CDL_OK 		0
#define CDL_INF         1
#define CDL_NO          2
#define CDL_NOK		1000
#define CDL_ACCURACY    1.0e-05


// -----------------------------------------------------------------
// v/w-discretisation, configuration space of the robot, synchro drive
// -----------------------------------------------------------------
#define CDL_V_TRA_MAX   1000.0	// mm/s
#define CDL_V_TRA_MIN  -1000.0	// mm/s
#define CDL_V_ROT_MAX   70.0  	// deg/s
#define CDL_V_ROT_MIN  -70.0	// deg/s

#define CDL_V_TRA_STEP  10.0  	// mm/s
#define CDL_V_ROT_STEP  1.0	// deg/s

// some robot specific parameters
#define CDL_A_ROT_MAX   200.0    // deg/s2 //(200)
#define CDL_A_TRA_MAX   500.0   // mm/s2 //(500)

#define CDL_TRA_CELLS ((CDL_V_TRA_MAX-CDL_V_TRA_MIN)/CDL_V_TRA_STEP+1)
#define CDL_ROT_CELLS ((CDL_V_ROT_MAX-CDL_V_ROT_MIN)/CDL_V_ROT_STEP+1)

// -----------------------------------------------------------------
// x/y-discretisation, local robot environment, cartesian space
// -----------------------------------------------------------------
#define CDL_C_X_MIN   -500.0	         // mm
#define CDL_C_X_MAX    2500.0	         // mm
#define CDL_C_Y_MIN   -2500.0	         // mm
#define CDL_C_Y_MAX    2500.0	         // mm
#define CDL_C_RES       50.0            // mm

#define CDL_MAX_DISTANCE	5000.0   // mm
#define CDL_CAPTURE_DISTANCE    50.0    // mm

#define CDL_C_X_CELLS ((CDL_C_X_MAX-CDL_C_X_MIN)/CDL_C_RES+1)
#define CDL_C_Y_CELLS ((CDL_C_Y_MAX-CDL_C_Y_MIN)/CDL_C_RES+1)

// ------------------------------------------------------------------
// Curvature-discretisation
// ------------------------------------------------------------------
// maximum reasonable resolution
#define CDL_CURVATURE_INDICES	(CDL_TRA_CELLS*2+CDL_ROT_CELLS*2-4)
#define CDL_ANGLE_STEP		(2*PI/CDL_CURVATURE_INDICES)

// ------------------------------------------------------------------
// Maximum array sizes etc.
// ------------------------------------------------------------------
#define CDL_MAX_LINES           100
#define CDL_MAX_TRA_CELLS	201
#define CDL_MAX_ROT_CELLS	141
#define CDL_MAX_X_CELLS		61
#define CDL_MAX_Y_CELLS		101
#define CDL_MAX_CURVATURE	680



#ifdef __cplusplus
}
#endif

#endif

