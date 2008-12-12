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

#ifndef SMART_EXAMPLE_LASER_CLIENT_GUI_THREAD_HH
#define SMART_EXAMPLE_LASER_CLIENT_GUI_THREAD_HH

#include <list>

#include <qobject.h>

#include "smartSoft.hh"
#include "smartQtPushNewestClient.hh"

#include "fawPainter.hh"

#include "commMobileLaserScan.hh"

class GuiThread: public QObject, public CHS::SmartTask
{
  Q_OBJECT
public:
  GuiThread
  (
    int argc, char **argv,
    Smart::QtPushNewestClient<Smart::CommMobileLaserScan> &scan_push_newest
  )
  : _argc(argc), _argv(argv), _scan_push_newest(scan_push_newest)
  {
  }
  
  int svc();

public slots:
  void redraw(Faw::Painter&);

signals:
  void model_changed();

private slots:
  void new_data();

private:

  int _argc;
  char **_argv;

  Smart::QtPushNewestClient<Smart::CommMobileLaserScan> &_scan_push_newest;
  Smart::CommMobileLaserScan _scan;
};

#endif
