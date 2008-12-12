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

#include <qtimer.h>

#include "smartQtTemporalSignalMerger.hh"

using namespace Smart;

QtTemporalSignalMerger::QtTemporalSignalMerger(int interval, QObject *parent)
: QObject(parent),
  _interval(interval),
  _blocking(false),
  _signal_blocked(false)
{
}
  
void QtTemporalSignalMerger::single_activate()
{
  if(_blocking)
  {
    _signal_blocked = true;
  }
  else
  {
    emit merged_activate();
    block_signals();
  }
}

void QtTemporalSignalMerger::block_signals()
{
  _blocking = true;
  _signal_blocked = false;
  QTimer::singleShot(_interval,this,SLOT(unblock_signals()));
}
  
void QtTemporalSignalMerger::unblock_signals()
{
  if(_blocking && _signal_blocked)
  {
    emit merged_activate();
    block_signals();
  }
  else
  {
    _blocking = false;
  }
}
