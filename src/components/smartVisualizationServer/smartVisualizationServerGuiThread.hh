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

#ifndef SMART_VISUALIZATION_SERVER_GUI_THREAD_HH
#define SMART_VISUALIZATION_SERVER_GUI_THREAD_HH

#include <map>
#include <string>

#include "smartSoft.hh"

#include <qobject.h>

#include "smartQtSendServerHandler.hh"

#include "fawPainter.hh"

#include "commPropertySet.hh"
#include "fawTransmittablePainter.hh"

class GuiThread: public QObject, public CHS::SmartTask
{
  Q_OBJECT
public:
  GuiThread
  (
    int argc, char **argv,
    const CHS::SmartParameter &parameters,
    Smart::QtSendServerHandler<Smart::CommPropertySet> &registration_handler,
    Smart::QtSendServerHandler<Faw::TransmittablePainter> &drawing_handler
  );
  
  int svc();

public slots:
  void redraw(Faw::Painter&);
  void freeze(bool toggle);
  void save();
  void view_all();

signals:
  void model_changed();
  void fit_view_port(double left, double right, double bottom, double top);

private slots:
  void handle_registration();
  void handle_drawing();
  
private:

  int _argc;
  char **_argv;

  double _view_min_x, _view_max_x, _view_min_y, _view_max_y;
  int _win_width;
  int _win_height;
  int _update_interval;
  
  Smart::QtSendServerHandler<Smart::CommPropertySet> &_registration_handler;
  Smart::QtSendServerHandler<Faw::TransmittablePainter> &_drawing_handler;

  std::map<std::string, Faw::DrawingStoreReplayPainter> _data;
};

#endif
