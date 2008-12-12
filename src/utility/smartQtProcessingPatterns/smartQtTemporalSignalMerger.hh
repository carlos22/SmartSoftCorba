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
// --------------------------------------------------------------------------

#ifndef SMART_QT_TEMPORAL_SIGNAL_MERGER_HH
#define SMART_QT_TEMPORAL_SIGNAL_MERGER_HH

#include <qobject.h>

namespace Smart {

class QtTemporalSignalMerger: public QObject
{
  Q_OBJECT

public:
  /**
    For a sequence of signals received on slot \c single_activate(), 
    emit the \c merged_activate() signal at most every \a interval milliseconds.
   */
  QtTemporalSignalMerger(int interval = 100, QObject *parent = 0);
  
public slots:
  void single_activate();
  
signals:
  void merged_activate();

private slots:
  void block_signals();
  void unblock_signals();

private:
  int _interval; // [msec]
  bool _blocking;
  bool _signal_blocked;
};

} // namespace Smart

#endif
