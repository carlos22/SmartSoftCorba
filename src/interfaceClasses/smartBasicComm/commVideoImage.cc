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
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------

#include "commVideoImage.hh"
#include "smartKnuthMorrisPratt.hh"

using namespace Smart;

bool CommVideoImage::has_parameters(unsigned int width, unsigned int height, Format format) const
{
  if(shm)
  {
    return (width==get_width()) && (height==get_height()) && (format==get_format());
  }
  return false;
}

unsigned int CommVideoImage::get_size_as_rgb24() const
{
  if(shm)
  {
    return 3 * get_width() * get_height();
  }
  return 0;
}

void CommVideoImage::get_as_rgb24(unsigned char *target) const
{
  _get_as_rgb(target, 0);
}

unsigned int CommVideoImage::get_size_as_rgb32() const
{
  if(shm)
  {
    return 4 * get_width() * get_height();
  }
  return 0;
}

void CommVideoImage::get_as_rgb32(unsigned char *target) const
{
  _get_as_rgb(target, 2);
}

void CommVideoImage::get_as_bgr24(unsigned char *target) const
{
  _get_as_bgr(target, 0);
}

void CommVideoImage::get_as_bgr32(unsigned char *target) const
{
  _get_as_bgr(target, 2);
}

void CommVideoImage::_get_as_rgb(unsigned char *target, int mode) const
{
  if(shm)
  {
    const unsigned int w = get_width();
    const unsigned int h = get_height();
    const unsigned int num_pixels = w*h; // NB: num_pixels != size
    const Format format = get_format();

    const unsigned char *source = get_data();

    const unsigned char *source_u = 0;
    const unsigned char *source_v = 0;
    const unsigned char *u_line = 0;
    const unsigned char *v_line = 0;

    // initialize
    switch(format)
    {
      case GREY:
      {
        break;
      }
      case RGB565:
      {
        break;
      }
      case RGB555:
      {
        break;
      }
      case RGB24:
      {
        break;
      }
      case RGB32:
      {
        break;
      }
      case YUV422:
      {
        break;
      }
      case YUYV:
      {
        break;
      }
      case UYVY:
      {
        break;
      }
      case YUV420P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/4);
        u_line = source_u;
        v_line = source_v;
        break;
      }
      case YUV422P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/2);
        u_line = source_u;
        v_line = source_v;
        break;
      }
      case YUV411P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/4);
        u_line = source_u;
        v_line = source_v;
        break;
      }
    }

    unsigned int x = 0;
    unsigned int y = 0;
    while(y<h)
    {
      if(mode==1) *target++ = 0;
      switch(format)
      {
        case GREY:
        {
          target[0] = target[1] = target[2] = *source;
          target += 3;
          ++source;
          ++x;
          break;
        }
        case RGB565:
        {
          const unsigned int w = source[0] + (((unsigned int)source[1]) << 8);
          target[0] = (w & 0x001F) << 3; // >> 0,  * 8
          target[1] = (w & 0x07E0) >> 3; // >> 5,  * 4
          target[2] = (w & 0xF800) >> 8; // >> 11, * 8
          target += 3;
          source += 2;
          ++x;
          break;
        }
        case RGB555:
        {
          const unsigned int w = source[0] + (((unsigned int)source[1]) << 8);
          target[0] = (w & 0x001F) << 3; // >> 0,  * 8
          target[1] = (w & 0x03E0) >> 2; // >> 5,  * 8
          target[2] = (w & 0x7C00) >> 7; // >> 10, * 8
          target += 3;
          source += 2;
          ++x;
          break;
        }
        case RGB24:
        {
          *target++ = *source++;
          *target++ = *source++;
          *target++ = *source++;
          ++x;
          break;
        }
        case RGB32:
        {
          ++source;
          *target++ = *source++;
          *target++ = *source++;
          *target++ = *source++;
          ++x;
          break;
        }
        case YUV422:
        case YUYV:
        {
          yuv2rgb(source[0],source[1],source[3], *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[2],source[1],source[3], *target++,*target++,*target++);
          source += 4;
          x += 2;
          break;
        }
        case UYVY:
        {
          yuv2rgb(source[1],source[0],source[2], *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[3],source[0],source[2], *target++,*target++,*target++);
          source += 4;
          x += 2;
          break;
        }
        case YUV420P:
        {
          yuv2rgb(source[0],*u_line,*v_line, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[1],*u_line,*v_line, *target++,*target++,*target++);

          ++u_line;
          ++v_line;

          source += 2;
          x += 2;

          if(x==w)
          {
            if(y%2==0)
            {
              source_u += w/2;
            }
            else
            {
              source_v += w/2;
            }
            u_line = source_u;
            v_line = source_v;
          }
          break;
        }
        case YUV422P:
        {
          yuv2rgb(source[0],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[1],*source_u,*source_v, *target++,*target++,*target++);
          ++source_u;
          ++source_v;
          source += 2;
          x += 2;
          break;
        }
        case YUV411P:
        {
          yuv2rgb(source[0],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[1],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[2],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2rgb(source[3],*source_u,*source_v, *target++,*target++,*target++);
          ++source_u;
          ++source_v;
          source += 4;
          x += 4;
          break;
        }
        default:
        {
          target[0] = target[1] = target[2] = 0;
          target += 3;
          x += 1;
        }
      }
      if(mode==2) *target++ = 0;
      if(x==w)
      {
        x = 0;
        ++y;
      }
    }
  }
}

void CommVideoImage::_get_as_bgr(unsigned char *target, int mode) const
{
  if(shm)
  {
    const unsigned int w = get_width();
    const unsigned int h = get_height();
    const unsigned int num_pixels = w*h; // NB: num_pixels != size
    const Format format = get_format();

    const unsigned char *source = get_data();

    const unsigned char *source_u = 0;
    const unsigned char *source_v = 0;
    const unsigned char *u_line = 0;
    const unsigned char *v_line = 0;

    // initialize
    switch(format)
    {
      case GREY:
      {
        break;
      }
      case RGB565:
      {
        break;
      }
      case RGB555:
      {
        break;
      }
      case RGB24:
      {
        break;
      }
      case RGB32:
      {
        break;
      }
      case YUV422:
      {
        break;
      }
      case YUYV:
      {
        break;
      }
      case UYVY:
      {
        break;
      }
      case YUV420P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/4);
        u_line = source_u;
        v_line = source_v;
        break;
      }
      case YUV422P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/2);
        u_line = source_u;
        v_line = source_v;
        break;
      }
      case YUV411P:
      {
        source_u = source + num_pixels;
        source_v = source + (num_pixels + num_pixels/4);
        u_line = source_u;
        v_line = source_v;
        break;
      }
    }

    unsigned int x = 0;
    unsigned int y = 0;
    while(y<h)
    {
      if(mode==1) *target++ = 0;
      switch(format)
      {
        case GREY:
        {
          target[0] = target[1] = target[2] = *source;
          target += 3;
          ++source;
          ++x;
          break;
        }
        case RGB565:
        {
          const unsigned int w = source[0] + (((unsigned int)source[1]) << 8);
          target[2] = (w & 0x001F) << 3; // >> 0,  * 8
          target[1] = (w & 0x07E0) >> 3; // >> 5,  * 4
          target[0] = (w & 0xF800) >> 8; // >> 11, * 8
          target += 3;
          source += 2;
          ++x;
          break;
        }
        case RGB555:
        {
          const unsigned int w = source[0] + (((unsigned int)source[1]) << 8);
          target[2] = (w & 0x001F) << 3; // >> 0,  * 8
          target[1] = (w & 0x03E0) >> 2; // >> 5,  * 8
          target[0] = (w & 0x7C00) >> 7; // >> 10, * 8
          target += 3;
          source += 2;
          ++x;
          break;
        }
        case RGB24:
        {
          target[2] = *source++;
          target[1] = *source++;
          target[0] = *source++;
          target += 3;
          ++x;
          break;
        }
        case RGB32:
        {
          target[2] = *source++;
          target[1] = *source++;
          target[0] = *source++;
          ++source;
          ++x;
          break;
        }
        case YUV422:
        case YUYV:
        {
          yuv2bgr(source[0],source[1],source[3], *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[2],source[1],source[3], *target++,*target++,*target++);
          source += 4;
          x += 2;
          break;
        }
        case UYVY:
        {
          yuv2bgr(source[1],source[0],source[2], *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[3],source[0],source[2], *target++,*target++,*target++);
          source += 4;
          x += 2;
          break;
        }
        case YUV420P:
        {
          yuv2bgr(source[0],*u_line,*v_line, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[1],*u_line,*v_line, *target++,*target++,*target++);

          ++u_line;
          ++v_line;

          source += 2;
          x += 2;

          if(x==w)
          {
            if(y%2==0)
            {
              source_u += w/2;
            }
            else
            {
              source_v += w/2;
            }
            u_line = source_u;
            v_line = source_v;
          }
          break;
        }
        case YUV422P:
        {
          yuv2bgr(source[0],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[1],*source_u,*source_v, *target++,*target++,*target++);
          ++source_u;
          ++source_v;
          source += 2;
          x += 2;
          break;
        }
        case YUV411P:
        {
          yuv2bgr(source[0],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[1],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[2],*source_u,*source_v, *target++,*target++,*target++);
          if((mode==1) || (mode==2)) *target++ = 0;
          yuv2bgr(source[3],*source_u,*source_v, *target++,*target++,*target++);
          ++source_u;
          ++source_v;
          source += 4;
          x += 4;
          break;
        }
        default:
        {
          target[0] = target[1] = target[2] = 0;
          target += 3;
          x += 1;
        }
      }
      if(mode==2) *target++ = 0;
      if(x==w)
      {
        x = 0;
        ++y;
      }
    }
  }
}

bool CommVideoImage::string2format(const std::string &name, Format &format)
{
  if(name=="grey")    { format = GREY;    return true; }
  if(name=="rgb565")  { format = RGB565;  return true; }
  if(name=="rgb555")  { format = RGB555;  return true; }
  if(name=="rgb24")   { format = RGB24;   return true; }
  if(name=="rgb32")   { format = RGB32;   return true; }
  if(name=="yuv422")  { format = YUV422;  return true; }
  if(name=="yuyv")    { format = YUYV;    return true; }
  if(name=="uyvy")    { format = UYVY;    return true; }
  if(name=="yuv420p") { format = YUV420P;  return true; }
  if(name=="yuv422p") { format = YUV422P; return true; }
  if(name=="yuv411p") { format = YUV411P; return true; }
  return false;
}

std::string CommVideoImage::format2string(Format format)
{
  switch(format)
  {
    case GREY: return "grey";
    case RGB565: return "rgb565";
    case RGB555: return "rgb555";
    case RGB24: return "rgb24";
    case RGB32: return "rgb32";
    case YUV422: return "yuv422";
    case YUYV: return "yuyv";
    case UYVY: return "uyvy";
    case YUV420P: return "yuv420p";
    case YUV422P: return "yuv422p";
    case YUV411P: return "yuv411p";
    default: return "<unknown>";
  }
  return "<unknown>";
}

unsigned int CommVideoImage::depth(Format format)
{
  switch(format)
  {
    case GREY: return 8;
    case RGB565: return 16;
    case RGB555: return 16;
    case RGB24: return 24;
    case RGB32: return 32;
    case YUV422: return 16;
    case YUYV: return 16;
    case UYVY: return 16;
    case YUV420P: return 12;
    case YUV422P: return 16;
    case YUV411P: return 12;
    default: return 0;
  }
  return 0;
}

void CommVideoImage::adjust_size(unsigned int &width, unsigned int &height, Format format)
{
  if((format==YUV422) || (format==YUYV)  || (format==UYVY) || (format==YUV422P))
  {
    // subsampled pixel size: w=2 h=1
    if(width%2 != 0)
    {
      std::cerr << "CommVideoImage: requested image format (YUV422,YUYV,UYVY,YUV422P) requires even width." << std::endl;
      ++width;
    }
  }
  else if(format==YUV420P)
  {
    // subsampled pixel size: w=2 h=2
    if(width%2 != 0)
    {
      std::cerr << "CommVideoImage: requested image format (YUV420P) requires even width." << std::endl;
      ++width;
    }
    if(height%2 != 0)
    {
      std::cerr << "CommVideoImage: requested image format (YUV420P) requires even height." << std::endl;
      ++height;
    }
  }
  else if(format==YUV411P)
  {
    // subsampled pixel size: w=4 h=1
    if(width%4 != 0)
    {
      std::cerr << "CommVideoImage: requested image format (YUV411P) requires width to be a multiple of 4." << std::endl;
      width = width + 4 - width%4;
    }
  }
}


void CommVideoImage::save_xml(std::ostream &os, const std::string &indent) const {
	os << indent << "<video_image>" << std::endl;
	os << indent << "  <width>" << get_width() << "</width>" << std::endl;
	os << indent << "  <height>" << get_height() << "</height>" << std::endl;
	os << indent << "  <format>" << format2string(get_format()) << "</format>" << std::endl;
	os << indent << "  <depth>" << depth(get_format()) << "</depth>" << std::endl;
	os << indent << "  <size>" << get_size() << "</size>" << std::endl;

	os << indent << "  <valid>" << (int) is_data_valid() << "</valid>" << std::endl;
	os << indent << "  <sequence_count>" << get_sequence_counter() << "</sequence_count>" << std::endl;

	os << indent << "  <image>";
	os.write(reinterpret_cast<const char *> (get_data()), get_size());
	os << "</image>" << std::endl;

	os << indent << "</video_image>" << std::endl;
}


void CommMutableVideoImage::set_data_invalid()
{
  if(shm)
  {
    reinterpret_cast<ImageParameters*>(shm)->data_valid = false;
  }
}

void CommMutableVideoImage::set_sequence_counter(unsigned long int n)
{
  if(shm)
  {
    reinterpret_cast<ImageParameters*>(shm)->seq_count = n;
  }
}

void CommMutableVideoImage::set_data(const unsigned char *data)
{
  if(shm)
  {
    memcpy((unsigned char*)shm + sizeof(ImageParameters), data, get_size());
    reinterpret_cast<ImageParameters*>(shm)->data_valid = true;
  }
}

CommResizableVideoImage::CommResizableVideoImage(unsigned int width, unsigned int height, Format format, const unsigned char *data)
: CommMutableVideoImage()
{
  set_parameters(width,height,format);
  if(data) set_data(data);
}
  
void CommResizableVideoImage::set_parameters(unsigned int width, unsigned int height, Format format)
{
  adjust_size(width, height, format);
  const unsigned int d = depth(format); // d = bits per pixel
  const unsigned int s = (width * height * d) / 8; // size in bytes
  if(CommSharedMemoryBase::alloc(sizeof(ImageParameters) + s)==0)
  {
    ImageParameters *p = reinterpret_cast<ImageParameters*>(shm);
    p->width = width;
    p->height = height;
    p->format = format;
    p->depth = d;
    p->size = s;
    p->data_valid = false;
    p->seq_count = 0;
  }
}

void CommResizableVideoImage::load_xml(std::istream &is) {
	static const KnuthMorrisPratt kmp_begin("<video_image>");

	static const KnuthMorrisPratt kmp_width("<width>");
	static const KnuthMorrisPratt kmp_height("<height>");
	static const KnuthMorrisPratt kmp_format("<format>");

	static const KnuthMorrisPratt kmp_valid("<valid>");
	static const KnuthMorrisPratt kmp_sequence_count("<sequence_count>");

	static const KnuthMorrisPratt kmp_image("<image>");

	static const KnuthMorrisPratt kmp_end("</video_image>");

	std::string format;
	uint32_t width, height;

	kmp_begin.search(is);

	kmp_width.search(is);
	is >> width;
	kmp_height.search(is);
	is >> height;
	kmp_format.search(is);
	
	char c = 0;
	while (c != '<') {
		is.read(&c, 1);
		
		if (c != '<') {
			format += c;
		}
	}	
	
	Format form;
	string2format(format, form);
	set_parameters(width, height, form);

	// size and depth must not be set because this is
	// done in set_parameters

	int b;
	kmp_valid.search(is);
	is >> b;

	ulong l;
	kmp_sequence_count.search(is);
	is >> l;
	set_sequence_counter(l);

	kmp_image.search(is);
	unsigned char* data = new unsigned char[get_size()];
	is.read(reinterpret_cast<char *> (data), get_size());
	set_data(data);
	delete data;

	// set the valid flag
  	if(shm)
  	{
    	reinterpret_cast<ImageParameters*>(shm)->data_valid = b;
  	}

	kmp_end.search(is);
}

