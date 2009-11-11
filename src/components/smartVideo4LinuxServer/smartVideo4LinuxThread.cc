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

#include <memory>

#include <linux/videodev.h>

#include "smartVideo4LinuxThread.hh"

#include "commVideoImage.hh"
#include "commVoid.hh"

using namespace Smart;

Video4LinuxThread::Video4LinuxThread
(
    CHS::SmartComponent &component,
    CHS::SmartParameter &parameters
)
: _component(component),
  _verbose(false),

  _push_verbose(false),
  _push_active(false),
  _push_name("image"),
  _push_num_buffers(25),
  _push_stride(1),

  _query_verbose(false),
  _query_queue_vector(2),
  _query_queue_index(0),

  _hardware_verbose(false),
  _use_hardware(true),
  _fake_hardware_delay(30),
  _device("/dev/video"),
  _channel("camera"),
  _capture_flags(0),
  _brightness(0.5),
  _hue(0.5),
  _colour(0.5),
  _contrast(0.5),
  _whiteness(0.5),
  _format(CommVideoImage::RGB24),
  _width(640),
  _height(480)
{
  parameters.getTruthValue("component","verbose",_verbose);

  parameters.getTruthValue("push","verbose",_push_verbose);
  parameters.getTruthValue("push","active",_push_active);
  parameters.getString("push","name",_push_name);
  parameters.getInt("push","buffers",_push_num_buffers);
  parameters.getInt("push","stride",_push_stride);

  parameters.getTruthValue("query","verbose",_query_verbose);

  parameters.getTruthValue("hardware","verbose",_hardware_verbose);
  parameters.getTruthValue("hardware","present",_use_hardware);
  parameters.getInt("hardware","delay",_fake_hardware_delay);
  parameters.getString("hardware","device",_device);
  parameters.getString("hardware","channel",_channel);
  parameters.getInt("hardware","capture_flags",_capture_flags);
  parameters.getInt("image","width",_width);
  parameters.getInt("image","height",_height);
  parameters.getDouble("image","brightness",_brightness);
  parameters.getDouble("image","hue",_hue);
  parameters.getDouble("image","colour",_colour);
  parameters.getDouble("image","contrast",_contrast);
  parameters.getDouble("image","whiteness",_whiteness);

  std::string format = "rgb24";
  parameters.getString("image","format",format);
  if(!CommVideoImage::string2format(format, _format))
  {
    std::cerr << "WARNING: unknown image format \"" << format << "\", keeping default \"rgb24\"." << std::endl;
  }
}

int Video4LinuxThread::svc()
{
  int fd = 0;
  video_mbuf vid_mbuf;
  video_mmap vid_mmap;
  unsigned char *vid_mem = 0;
  if(_use_hardware)
  {
    fd = ::open(_device.c_str(), O_RDWR);
    if(fd<0)
    {
      std::cerr << "ERROR: failed to open \"" << _device <<  "\"." << std::endl;
      return -1;
    }

      video_capability vid_cap;
      if(ioctl(fd, VIDIOCGCAP, &vid_cap)<0)
      {
        std::cerr << "ERROR: failed to get video capabilities." << std::endl;
        perror("ioctl(fd, VIDIOCGCAP, &vid_cap)");
        close(fd);
        return -1;
      }

    if(_hardware_verbose)
    {
      std::cout << "Connected to \"" << vid_cap.name << " Channels: "<< vid_cap.channels << "\" video device." << std::endl;
    }

    if((vid_cap.type & VID_TYPE_CAPTURE)==0)
    {
      std::cerr << "ERROR: device does not support capturing." << std::endl;
      close(fd);
      return -1;
    }

    if(_hardware_verbose)
    {
      std::cout << "Scanning input channels." << std::endl;
    }

    int channel = -1;
    for(int i=0; i<vid_cap.channels; ++i)
    {
      video_channel vid_chan;
      vid_chan.channel = i;
      if(ioctl(fd, VIDIOCGCHAN, &vid_chan)<0)
      {
        std::cerr << "ERROR: failed to get channel description." << std::endl;
        perror("ioctl(fd, VIDIOCGCHAN, &vid_chan)");
        close(fd);
        return -1;
      }

      if(_hardware_verbose)
      {
        std::cout << "Channel " << vid_chan.channel << " is \"" << vid_chan.name << "\" (";
        if(vid_chan.type==VIDEO_TYPE_TV)
        {
          std::cout << "TV";
        }
        else if(vid_chan.type==VIDEO_TYPE_CAMERA)
        {
          std::cout << "camera";
        }
        else
        {
          std::cout << "unknown";
        }
        std::cout << " type)" << std::endl;
      }

      if((channel<0) && (_channel == std::string(vid_chan.name)))
      {
        channel = vid_chan.channel;
      }
    }

    if(channel<0)
    {
      std::cerr << "WARNING: channel \"" << _channel << "\" not found, using channel 0" << std::endl;
      channel = 0;
    }
    else if(_hardware_verbose)
    {
      std::cout << "Using channel " << channel << " (" << _channel << ")." << std::endl;
    }

    if(ioctl(fd, VIDIOCSCHAN, &channel)<0)
    {
      std::cerr << "ERROR: failed to switch to channel " << channel << std::endl;
      perror("ioctl(fd, VIDIOCSCHAN, &channel)");
      close(fd);
      return -1;
    }

    video_picture vid_pict;
    int vid_pict_orig_palette = 0;
    if(ioctl(fd, VIDIOCGPICT, &vid_pict)<0)
    {
      std::cerr << "ERROR: failed to get image parameters" << std::endl;
      perror("ioctl(fd, VIDIOCGPICT, &vid_pict)");
      close(fd);
      return -1;
    }
    vid_pict_orig_palette = vid_pict.palette;

    vid_pict.brightness = _scale_double2int(_brightness);
    vid_pict.hue        = _scale_double2int(_hue);
    vid_pict.colour     = _scale_double2int(_colour);
    vid_pict.contrast   = _scale_double2int(_contrast);
    vid_pict.whiteness  = _scale_double2int(_whiteness);
    vid_pict.palette    = _convert_smart_to_v4l_format(_format);

    if(_hardware_verbose)
    {
      std::cout << "Selecting " << CommVideoImage::format2string(_format) << " image format." << std::endl;
    }

    if(ioctl(fd, VIDIOCSPICT, &vid_pict)<0)
    {
      std::cerr << "WARNING: failed to set image parameters, format may be unsupported." << std::endl;

      _format = _convert_v4l_to_smart_format(vid_pict_orig_palette);
      vid_pict.palette = _convert_smart_to_v4l_format(_format);
      if(_hardware_verbose)
      {
        std::cout << "Leaving format unchanged (\"" << CommVideoImage::format2string(_format) << "\")" << std::endl;
      }

      if(ioctl(fd, VIDIOCSPICT, &vid_pict)<0)
      {
        std::cerr << "ERROR: failed to set image parameters." << std::endl;
        perror("ioctl(fd, VIDIOCSPICT, &vid_pict)");
        close(fd);
        return -1;
      }
    }


    if(ioctl(fd, VIDIOCGMBUF, &vid_mbuf)<0)
    {
      std::cerr << "ERROR: failed to get video buffer information." << std::endl;
      perror("ioctl(fd, VIDIOCGMBUF, &vid_mbuf)");
      close(fd);
      return -1;
    }

    if(_hardware_verbose)
    {
      std::cout << "Device provides buffer(s) for " << vid_mbuf.frames << " frame(s)." << std::endl;
    }

    vid_mem = (unsigned char*)mmap(0, vid_mbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(vid_mem==(unsigned char*)-1)
    {
      std::cerr << "ERROR: failed to map memory." << std::endl;
      perror("mmap(0, vid_mbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)");
      close(fd);
      return -1;
    }
  }
  else // !_use_hardware
  {
    // simulate a set of buffers filled with (random) image data
    vid_mbuf.frames = 8;
    unsigned int w = _width;
    unsigned int h = _height;
    Smart::CommVideoImage::adjust_size(w,h,_format);
    const unsigned int size = (w * h * Smart::CommVideoImage::depth(_format)) / 8;
    vid_mem = new unsigned char[vid_mbuf.frames * size];
    for(unsigned int i=0; i<vid_mbuf.frames * size; ++i)
    {
      // create a random data block to copy in order to properly simulate computational load
      vid_mem[i] = rand();
    }
    for(int i=0; i<vid_mbuf.frames; ++i)
    {
      vid_mbuf.offsets[i] = i * size;
    }
  }


  // initialize push service, if acitvated
  std::auto_ptr<CHS::PushNewestServer<Smart::CommVideoImage> > push_server;
  std::vector<Smart::CommMutableVideoImage*> push_ring_buffer;
  unsigned int push_ring_buffer_index = 0;
  PushBufferCleanupThread *push_buffer_cleanup_thread = 0;
  if(_push_active)
  {
    if(_push_verbose) std::cout << "Creating push servant." << std::endl;
    push_server.reset(new CHS::PushNewestServer<Smart::CommVideoImage>(&_component,_push_name));

    if(_push_verbose) std::cout << "Creating ring buffer for push service." << std::endl;
    push_ring_buffer.resize(_push_num_buffers);
    for(unsigned int i=0; i<_push_num_buffers; ++i)
    {
      push_ring_buffer[i] = new CommResizableVideoImage(_width,_height,_format);
    }

    if(_push_verbose) std::cout << "Creating ring buffer cleanup thread." << std::endl;
    push_buffer_cleanup_thread = new PushBufferCleanupThread;
    push_buffer_cleanup_thread->open();
  }

  if(_use_hardware)
  {
    video_window vid_win;
    unsigned int vid_win_orig_width;
    unsigned int vid_win_orig_height;
    unsigned int vid_win_orig_flags;
    if(ioctl(fd, VIDIOCGWIN, &vid_win)<0)
    {
      std::cerr << "ERROR: failed to get capture window." << std::endl;
      perror("ioctl(fd, VIDIOCGWIN, &vid_win)");
      munmap(vid_mem, vid_mbuf.size);
      close(fd);
      return -1;
    }
    vid_win_orig_width  = vid_win.width;
    vid_win_orig_height = vid_win.height;
    vid_win_orig_flags  = vid_win.flags;
    if(_hardware_verbose)
    {
      std::cout << "Current capture window size is " << vid_win.width << "x" << vid_win.height << std::endl;
    }

    vid_win.width  = _width;
    vid_win.height = _height;
    vid_win.flags  = _capture_flags;
    if(_hardware_verbose)
    {
      std::cout << "Setting capture window size to " << _width << "x" << _height << ", flags=0x"
                << std::hex << std::setw(8) << std::setfill('0') << _capture_flags << std::dec << std::endl;
    }
    if(ioctl(fd, VIDIOCSWIN, &vid_win)<0)
    {
      std::cerr << "WARNING: failed to set user defined capture window size, using current size." << std::endl;
      vid_win.width  = vid_win_orig_width;
      vid_win.height = vid_win_orig_height;
      vid_win.flags  = vid_win_orig_flags;
      if(ioctl(fd, VIDIOCSWIN, &vid_win)<0)
      {
        std::cerr << "ERROR: failed to set capture window size." << std::endl;
        perror("ioctl(fd, VIDIOCSWIN, &vid_win)");
        munmap(vid_mem, vid_mbuf.size);
        close(fd);
        return -1;
      }
    }
    // read the size which actually will be used:
    if(ioctl(fd, VIDIOCGWIN, &vid_win)<0)
    {
      std::cerr << "ERROR: failed to get capture window size." << std::endl;
      perror("ioctl(fd, VIDIOCGWIN, &vid_win)");
      munmap(vid_mem, vid_mbuf.size);
      close(fd);
      return -1;
    }
    _width = vid_win.width;
    _height = vid_win.height;
    if(_hardware_verbose)
    {
      std::cout << "Actual size is " << _width << "x" << _height << std::endl;
      std::cout << "Actual flags=0x" << std::hex << std::setw(8) << std::setfill('0') << vid_win.flags << std::dec << std::endl;
    }

    vid_mmap.height = _height;
    vid_mmap.width = _width;
    vid_mmap.format = _convert_smart_to_v4l_format(_format);
    for(int i=0; i<vid_mbuf.frames; ++i)
    {
      vid_mmap.frame = i;
      if(ioctl(fd, VIDIOCMCAPTURE, &vid_mmap)<0)
      {
        std::cerr << "ERROR: failed to start capturing to buffer " << i << "." << std::endl;
        perror("ioctl(fd, VIDIOCMCAPTURE, &vid_mmap)");
        munmap(vid_mem, vid_mbuf.size);
        close(fd);
        return -1;
      }
    }
  }

  unsigned char *image_data = 0;
  int ret = 0;
  int buffer = 0;
  unsigned int push_stride_countdown = 0;
  unsigned long seq_count = 0;
  while (! CHS::SmartThreadManager::instance()->testcancel(ACE_Thread::self()))
  {
//    if(_hardware_verbose) std::cout << "Sync on next frame" << std::endl;
    if(_use_hardware)
    {
      if(ioctl(fd, VIDIOCSYNC, &buffer)<0)
      {
        std::cerr << "ERROR: failed to sync to buffer " << buffer << "." << std::endl;
        perror("ioctl(fd, VIDIOCSYNC, &buffer)");
        ret = -1;
        break;
      }
    }
    else // !_use_hardware
    {
      usleep(_fake_hardware_delay * 1000);
    }

    if(_verbose) _statistics(0);

    // access buffer...
    image_data = vid_mem + vid_mbuf.offsets[buffer];

    if(_push_active)
    {
      if(push_stride_countdown==0)
      {
        push_stride_countdown = _push_stride;

        std::cout << "Debug: --0--" << std::endl;
        CommMutableVideoImage *image = push_ring_buffer[push_ring_buffer_index];
        std::cout << "Debug: --1--" << std::endl;
        unsigned int ref_count = 1;
        if((!image->try_get_ref_count(ref_count)) || (ref_count>1))
        {
          // image is still accessed by some client
          push_buffer_cleanup_thread->remove_reference_to(image); // directly removing could block!
          image = new CommResizableVideoImage(_width,_height,_format);
          push_ring_buffer[push_ring_buffer_index] = image;
        std::cout << "Debug: --2--" << std::endl;
        }

        // copy image data...
        image->set_data(image_data);
        image->set_sequence_counter(seq_count);
        std::cout << "Debug: --3--" << std::endl;

        // ...and push
        push_server->put(*image);
        if(_push_verbose) std::cout << "Pushed data" << std::endl;

        ++push_ring_buffer_index;
        if(push_ring_buffer_index==_push_num_buffers) push_ring_buffer_index = 0;
      }
      --push_stride_countdown;
    }

    // answer queries:
    // first, swap the two query queues...
    unsigned int index;
    _query_queue_mutex.acquire();
    index = _query_queue_index;
    _query_queue_index = (_query_queue_index + 1) % 2;
    _query_queue_mutex.release();

    // ...then, drain the possibly filled queue
    while(!_query_queue_vector[index].empty())
    {
      QueryQueueEntry entry = _query_queue_vector[index].front();
      _query_queue_vector[index].pop_front();
      if(entry.image.has_parameters(_width,_height,_format))
      {
        entry.image.set_data(image_data);
        entry.image.set_sequence_counter(seq_count);
      }
      else
      {
        entry.image.set_data_invalid();
      }
      entry.server->answer(entry.id,Smart::CommVoid());
      if(_query_verbose) std::cout << "Answered query request (id=" << entry.id << ")" << std::endl;
    }

    if(_use_hardware)
    {
      vid_mmap.frame = buffer;
      if(ioctl(fd, VIDIOCMCAPTURE, &vid_mmap)<0)
      {
        std::cerr << "ERROR: failed to restart capturing to buffer " << buffer << "." << std::endl;
        perror("ioctl(fd, VIDIOCMCAPTURE, &vid_mmap)");
        ret = -1;
        break;
      }
    }

    if(_verbose) _statistics(1);

    ++buffer;
    if(buffer==vid_mbuf.frames) buffer = 0;

    ++seq_count;
  }


  if(_push_active)
  {
    if(_push_verbose) std::cout << "Freeing ring buffer for push service." << std::endl;
    for(unsigned int i=0; i<_push_num_buffers; ++i)
    {
      delete push_ring_buffer[i];
    }

    if(_push_verbose) std::cout << "Stopping ring buffer cleanup thread." << std::endl;
    push_buffer_cleanup_thread->stop();
    delete push_buffer_cleanup_thread;
  }

  if(_use_hardware)
  {
    munmap(vid_mem, vid_mbuf.size);
    close(fd);
  }
  else
  {
    delete[] vid_mem;
  }

  return ret;
}



void Video4LinuxThread::handleQuery(CHS::QueryServer<Smart::CommMutableVideoImage,Smart::CommVoid> &server, const CHS::QueryId id, const Smart::CommMutableVideoImage &image) throw()
{
  if(_query_verbose) std::cout << "Received query request (id=" << id << ")" << std::endl;
  QueryQueueEntry entry;
  entry.id = id;
  entry.image = image;
  entry.server = &server;
  _query_queue_mutex.acquire();
  _query_queue_vector[_query_queue_index].push_back(entry);
  _query_queue_mutex.release();
}


void Video4LinuxThread::_statistics(int where)
{
  static unsigned int frame_counter = 0;
  static timeval integrate_start_time;
  static timeval frame_start_time;
  static bool started = false;
  static unsigned int integrated_time_ms = 0;

  if(where==0)
  {
    gettimeofday(&frame_start_time,0);
  }
  else
  {
    timeval current_time;
    gettimeofday(&current_time,0);

    if(!started)
    {
      integrate_start_time = current_time;
      frame_counter = 0;
      integrated_time_ms = 0;
      started = true;
    }
    else
    {
      ++frame_counter;

      const unsigned int frame_time_msec = (2000 + (current_time.tv_usec - frame_start_time.tv_usec)/1000) % 1000;
      integrated_time_ms += frame_time_msec;

      const double dt = (current_time.tv_sec - integrate_start_time.tv_sec) + 1e-6*(current_time.tv_usec - integrate_start_time.tv_usec);
      if(dt>5)
      {
        const double fps = frame_counter / dt;
        std::cout << fps << " fps, "
                  << 0.1 * integrated_time_ms * fps / frame_counter << " % cpu time" << std::endl;

        integrate_start_time = current_time;
        frame_counter = 0;
        integrated_time_ms = 0;
      }
    }
  }
}

int Video4LinuxThread::_convert_smart_to_v4l_format(CommVideoImage::Format format)
{
  switch(format)
  {
    case CommVideoImage::GREY: return VIDEO_PALETTE_GREY;
    case CommVideoImage::RGB565: return VIDEO_PALETTE_RGB565;
    case CommVideoImage::RGB555: return VIDEO_PALETTE_RGB555;
    case CommVideoImage::RGB24: return VIDEO_PALETTE_RGB24;
    case CommVideoImage::RGB32: return VIDEO_PALETTE_RGB32;
    case CommVideoImage::YUV422: return VIDEO_PALETTE_YUV422;
    case CommVideoImage::YUYV: return VIDEO_PALETTE_YUYV;
    case CommVideoImage::UYVY: return VIDEO_PALETTE_UYVY;
    case CommVideoImage::YUV420P: return VIDEO_PALETTE_YUV420P;
    case CommVideoImage::YUV422P: return VIDEO_PALETTE_YUV422P;
    case CommVideoImage::YUV411P: return VIDEO_PALETTE_YUV411P;
    default: return 0; // won't happen, list above is exhaustive
  }
  return 0;
}

CommVideoImage::Format Video4LinuxThread::_convert_v4l_to_smart_format(int format)
{
  switch(format)
  {
    case VIDEO_PALETTE_GREY: return CommVideoImage::GREY;
    case VIDEO_PALETTE_RGB565: return CommVideoImage::RGB565;
    case VIDEO_PALETTE_RGB555: return CommVideoImage::RGB555;
    case VIDEO_PALETTE_RGB24: return CommVideoImage::RGB24;
    case VIDEO_PALETTE_RGB32: return CommVideoImage::RGB32;
    case VIDEO_PALETTE_YUV422: return CommVideoImage::YUV422;
    case VIDEO_PALETTE_YUYV: return CommVideoImage::YUYV;
    case VIDEO_PALETTE_UYVY: return CommVideoImage::UYVY;
    case VIDEO_PALETTE_YUV420P: return CommVideoImage::YUV420P;
    case VIDEO_PALETTE_YUV422P: return CommVideoImage::YUV422P;
    case VIDEO_PALETTE_YUV411P: return CommVideoImage::YUV411P;
    default:
    {
      std::cerr << "WARNING: unknown V4L format \"" << format << "\"" << std::endl;
      return CommVideoImage::RGB24;
    }
  }
  return CommVideoImage::RGB24;
}

Video4LinuxThread::PushBufferCleanupThread::PushBufferCleanupThread()
: _queue_sem(0)
{
}

int Video4LinuxThread::PushBufferCleanupThread::svc()
{
  Smart::CommVideoImage *img = 0;
  while(true)
  {
    _queue_sem.acquire();
    _queue_mutex.acquire();
    img = _queue.front();
    _queue.pop_front();
    _queue_mutex.release();
    if(img==0) break; // "0" means stop
    delete img; // may block on semaphore protecting shm
  }
  return 0;
}


void Video4LinuxThread::PushBufferCleanupThread::_enqueue(Smart::CommVideoImage *img)
{
  _queue_mutex.acquire();
  _queue.push_back(img);
  _queue_mutex.release();
  _queue_sem.release();
}



