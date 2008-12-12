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
// --------------------------------------------------------------------------

#ifndef SMART_EXAMPLE_NOMAD200_CLIENT_GUI_THREAD_HH
#define SMART_EXAMPLE_NOMAD200_CLIENT_GUI_THREAD_HH

#include <list>

#include <qobject.h>

#include "smartSoft.hh"
#include "smartQtPushTimedClient.hh"

#include "fawPainter.hh"

#include "commBaseState.hh"

class GuiThread: public QObject, public CHS::SmartTask
{
  Q_OBJECT
public:
  GuiThread
  (
    int argc, char **argv,
    Smart::QtPushTimedClient<Smart::CommBaseState> &basestate_push_timed
  )
  : _argc(argc), _argv(argv), _basestate_push_timed(basestate_push_timed)
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

  Smart::QtPushTimedClient<Smart::CommBaseState> &_basestate_push_timed;
  Smart::CommBaseState _basestate;
};

#endif
