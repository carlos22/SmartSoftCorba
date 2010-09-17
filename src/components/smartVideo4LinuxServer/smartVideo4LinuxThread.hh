// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        kluge@faw.uni-ulm.de
//
//        FAW Ulm
//        Research Institute for Applied Knowledge Processing
//        Helmholtzstr. 16
//        D-89081 Ulm
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
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

#ifndef SMART_VIDEO4LINUX_THREAD_HH
#define SMART_VIDEO4LINUX_THREAD_HH

#include <cmath>

#include "smartSoft.hh"

#include "commVoid.hh"
#include "commVideoImage.hh"

namespace Smart {

class Video4LinuxThread: 
  public CHS::SmartTask,
  public CHS::QueryServerHandler<Smart::CommMutableVideoImage,Smart::CommVoid>
{
public:

  Video4LinuxThread(CHS::SmartComponent &component, CHS::SmartParameter &parameters);

  int svc();

  void handleQuery(CHS::QueryServer<Smart::CommMutableVideoImage, Smart::CommVoid> &server, const CHS::QueryId id, const Smart::CommMutableVideoImage &image) throw();

  class PushBufferCleanupThread: public CHS::SmartTask
  {
  public:
    PushBufferCleanupThread();
    int svc();
    inline void remove_reference_to(Smart::CommVideoImage *i) { if(i) _enqueue(i); }
    inline void stop() { _enqueue(0); }
  private:
    void _enqueue(Smart::CommVideoImage*);
    std::list<Smart::CommVideoImage*> _queue;
    CHS::SmartMutex _queue_mutex;
    CHS::SmartSemaphore _queue_sem;
  };

private:

  struct QueryQueueEntry
  {
    CHS::QueryId id;
    Smart::CommMutableVideoImage image;
    CHS::QueryServer<Smart::CommMutableVideoImage, Smart::CommVoid> *server;
  };

  CHS::SmartComponent &_component;
  bool _verbose;

  bool _push_verbose;
  bool _push_active;
  std::string _push_name;
  unsigned int _push_num_buffers;
  unsigned int _push_stride;
  
  bool _query_verbose;
  std::vector<std::list<QueryQueueEntry> > _query_queue_vector;
  unsigned int _query_queue_index;
  CHS::SmartMutex _query_queue_mutex;

  bool _hardware_verbose;
  bool _use_hardware;
  unsigned int _fake_hardware_delay;
  std::string _device;
  std::string _channel;
  unsigned int _capture_flags;
  
  double _brightness;
  double _hue;
  double _colour;
  double _contrast;
  double _whiteness;
  Smart::CommVideoImage::Format _format;
  int _width;
  int _height;

  inline int _scale_double2int(double d) const
  {
    if(d<0) return 0;
    if(d>1) return 65535;
    return int(::rint(d*65535));
  }

  void _statistics(int where);

  static int _convert_smart_to_v4l_format(CommVideoImage::Format format);
  static CommVideoImage::Format _convert_v4l_to_smart_format(int format);
};

} // namespace Smart

#endif
