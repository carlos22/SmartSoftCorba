//--------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Xfig Tools".
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// CREDITS:
//
// FIG : Facility for Interactive Generation of figures
// Copyright (c) 1985-1988 by Supoj Sutanthavibul
// Parts Copyright (c) 1989-2000 by Brian V. Smith
// Parts Copyright (c) 1991 by Paul King
//
// Any party obtaining a copy of these files is granted, free of charge, a
// full and unrestricted irrevocable, world-wide, paid up, royalty-free,
// nonexclusive right and license to deal in this software and
// documentation files (the "Software"), including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons who receive
// copies from any such party to do so, with the only requirement being
// that this copyright notice remain intact.
//
//--------------------------------------------------------------------------

#ifndef SMARTLASERFIGOBJECT_H
#define SMARTLASERFIGOBJECT_H

// internally used own defines
#define  FAW_XFIG_POINT          1
#define  FAW_XFIG_CROSS          2

// values to signify color used for transparent GIF color

#define  TRANSP_BACKGROUND      -3    // use background of figure as transp color
#define  TRANSP_NONE            -2    // no transp color
#define  COLOR_NONE             -2    // no background color (exporting)

// DEFAULT is used for many things - font, color etc

#define  DEFAULT               (-1)

#define  SOLID_LINE              0
#define  DASH_LINE               1
#define  DOTTED_LINE             2
#define  DASH_DOT_LINE           3
#define  DASH_2_DOTS_LINE        4
#define  DASH_3_DOTS_LINE        5

#define  RUBBER_LINE             11
#define  PANEL_LINE              12

#define  BLACK                   0
#define  BLUE                    1
#define  GREEN                   2
#define  CYAN                    3
#define  RED                     4
#define  MAGENTA                 5
#define  YELLOW                  6
#define  WHITE                   7
#define  GREEN4                  12

#define  DEF_ARROW_WID           4
#define  DEF_ARROW_HT            8

// ellipse object
#define  T_ELLIPSE_BY_RAD        1
#define  T_ELLIPSE_BY_DIA        2
#define  T_CIRCLE_BY_RAD         3
#define  T_CIRCLE_BY_DIA         4
#define  UNFILLED               -1

// arc object
#define  T_OPEN_ARC              0
#define  T_PIE_WEDGE_ARC         1
#define  CAP_BUTT                0
#define  CAP_ROUND               1
#define  CAP_PROJECT             2

#define  CLOSED_PATH             0
#define  OPEN_PATH               1
#define  DEF_BOXRADIUS           7
#define  DEF_DASHLENGTH          4
#define  DEF_DOTGAP              3

// line object
#define  T_POLYLINE              1
#define  T_BOX                   2
#define  T_POLYGON               3
#define  T_ARC_BOX               4
#define  T_PICTURE               5
#define  JOIN_MITER              0
#define  JOIN_ROUND              1
#define  JOIN_BEVEL              2

// text object
#define  T_LEFT_JUSTIFIED        0
#define  T_CENTER_JUSTIFIED      1
#define  T_RIGHT_JUSTIFIED       2
#define  RIGID_TEXT              1
#define  SPECIAL_TEXT            2
#define  PSFONT_TEXT             4
#define  HIDDEN_TEXT             8


// object codes 
#define  XFIG_O_COLOR_DEF        0
#define  XFIG_O_ELLIPSE          1
#define  XFIG_O_POLYLINE         2
#define  XFIG_O_SPLINE           3
#define  XFIG_O_TEXT             4
#define  XFIG_O_ARC              5
#define  XFIG_O_COMPOUND         6
#define  XFIG_O_END_COMPOUND     -XFIG_O_COMPOUND
// pseudo-object XFIG_O_FIGURE only for edit panel
#define  XFIG_O_FIGURE           17777
#define  XFIG_O_ALL_OBJECT       99

#endif

