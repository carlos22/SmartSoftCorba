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
//
// 	1394-Based Digital Camera Control Library
//
// 	Color conversion functions, including Bayer pattern decoding
//
//	 Written by Damien Douxchamps and Frederic Devernay
//
//	 This library is free software; you can redistribute it and/or
// 	modify it under the terms of the GNU Lesser General Public
// 	License as published by the Free Software Foundation; either
// 	version 2.1 of the License, or (at your option) any later version.
//
//	 This library is distributed in the hope that it will be useful,
// 	but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// 	Lesser General Public License for more details.
//
// 	You should have received a copy of the GNU Lesser General Public
// 	License along with this library; if not, write to the Free Software
// 	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

#ifndef SMART_COMM_VIDEO_IMAGE_HH
#define SMART_COMM_VIDEO_IMAGE_HH

#include <cmath>

#include "commSharedMemory.hh"

namespace Smart {

/**
  A constant video image.
  Constant means that pixel values and the image size can be read but cannot be changed.
  Clients may not lock the image (the image is constant, so there is no locking needed).
 */
class CommVideoImage: protected CommSharedMemoryBase
{
public:

  /**
    Description of the image parameters.
   */
  struct ImageParameters
  {
    //
    // The following fiels may be written from CommResizableVideoImage instances
    //

    /**
      The width of the image in pixels.
     */
    unsigned int width;

    /**
      The height of the image in pixels.
     */
    unsigned int height;

    /**
      The image format.
     */
    unsigned int format;

    /**
      Average number of bits per pixel.
     */
    unsigned int depth;

    /**
      Size of the image in bytes.
     */
    unsigned int size;

    //
    // The following fiels may be written from CommMutableVideoImage instances
    //

    /**
      Boolean flag if image data is valid.
     */
    bool data_valid;

    /**
      Sequence counter
     */
    unsigned long seq_count;
  };

  /**
    Supported image formats.
   */
  enum Format
  {
    GREY,
    RGB565,
    RGB555,
    RGB24,
    RGB32,
    YUV422,
    YUYV,
    UYVY,
    YUV420P,
    YUV422P,
    YUV411P
  };

  /**
   */
  inline CommVideoImage()
  {
  }

  /**
   */
  inline CommVideoImage(const CommVideoImage &source)
  : CommSharedMemoryBase(source)
  {
  }

  /**
   */
  inline CommVideoImage &operator=(const CommVideoImage &source)
  {
    CommSharedMemoryBase::operator=(source);
    return *this;
  }

  /**
    Required by SmartSoft.
   */
  inline void get(CORBA::Any &a) const { CommSharedMemoryBase::get(a); }

  /**
    Required by SmartSoft.
   */
  inline void set(const CORBA::Any &a) { CommSharedMemoryBase::set(a); }

  /**
    Required by SmartSoft.
   */
  static inline std::string identifier()
  {
    return std::string("CommVideoImage");
  }

  /**
    Get the width of the image in pixels.
   */
  inline unsigned int get_width() const
  {
    return reinterpret_cast<const ImageParameters*>(shm)->width;
  }

  /**
    Get the height of the image in pixels.
   */
  inline unsigned int get_height() const
  {
    return reinterpret_cast<const ImageParameters*>(shm)->height;
  }

  /**
    Get the data format of the image.
    The <a href="http://www.thedirks.org/v4l2/">Video4Linux Two documentation</a>
    contains
    <a href="http://www.thedirks.org/v4l2/v4l2fmt.htm">a nice description of these formats</a>.
    Note that the integer values of the enum members of this class do not necessarily
    correspond to the macro values of video4linux for the same format.
   */
  inline Format get_format() const
  {
    return (Format)(reinterpret_cast<const ImageParameters*>(shm)->format);
  }

  /**
    Return the size of the image in memory in bytes.
   */
  inline unsigned int get_size() const
  {
    return reinterpret_cast<const ImageParameters*>(shm)->size;
  }

  /**
    Check if the image data is valid.
    Image data may be invalid for example if the client requested a format which the
    server isn't able to create.
    At the moment, the server is configured by its ini file to one static format and size.
   */
  inline bool is_data_valid() const
  {
    return (shm) && reinterpret_cast<const ImageParameters*>(shm)->data_valid;
  }

  /**
    Returns the server sided sequence counter of this image.
    Use it to see if your client drops frames.
   */
  inline unsigned long get_sequence_counter() const
  {
    return reinterpret_cast<const ImageParameters*>(shm)->seq_count;
  }

  /**
    Return a pointer to the image data.
   */
  inline const unsigned char *get_data() const
  {
    return (const unsigned char*)shm + sizeof(ImageParameters);
  }

  unsigned int get_size_as_rgb24() const;
  void get_as_rgb24(unsigned char *target) const;

  unsigned int get_size_as_rgb32() const;
  void get_as_rgb32(unsigned char *target) const;

  inline unsigned int get_size_as_bgr24() const { return get_size_as_rgb24(); }
  void get_as_bgr24(unsigned char *target) const;

  inline unsigned int get_size_as_bgr32() const { return get_size_as_rgb32(); }
  void get_as_bgr32(unsigned char *target) const;

  /**
    Check if this image has a certain format.
   */
  bool has_parameters(unsigned int width, unsigned int height, Format format) const;

  //
  // helper functions for format handling
  //

  /**
    Try to convert a string description of a format into the
    corresponding enumeration member.
    Returns \c false on failure.
   */
  static bool string2format(const std::string &name, Format &format);

  /**
    Return a string describing the image format.
    This string can be converted back to an enum member using string2format.
   */
  static std::string format2string(Format format);

  /**
    Return the average number of bits per pixel for a given image format.
   */
  static unsigned int depth(Format format);

  /**
    Adjust the image size to values permitted by the indicated format.
    Certain formats require the image width to be an even number of pixels etc.
   */
  static void adjust_size(unsigned int &width, unsigned int &height, Format format);

  static inline void yuv2rgb(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char &r, unsigned char &g, unsigned char &b);
  static inline void yuv2bgr(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char &b, unsigned char &g, unsigned char &r) { yuv2rgb(y,u,v,r,g,b); }
  
  /**
    Save an XML like representation of this image to the given output stream.
   */
   void save_xml(std::ostream &os, const std::string &indent = "") const;

private:
  void _get_as_rgb(unsigned char *target, int mode) const;
  void _get_as_bgr(unsigned char *target, int mode) const;
};


/**
  A mutable video image.
  Pixel values may be changed, but size and format of the image are constant.
  Clients may lock the image.
 */
class CommMutableVideoImage: public CommVideoImage
{
public:
  inline CommMutableVideoImage() {}
  inline CommMutableVideoImage(const CommMutableVideoImage &source) : CommVideoImage(source) {}
  inline CommMutableVideoImage &operator=(const CommMutableVideoImage &source)
  {
    if(&source!=this) CommVideoImage::operator=(source);
    return *this;
  }

  /**
    Try to lock the shared memory segment and get the reference counter.
    This method can be used to find out if some other process or thread is accessing
    this image, which is the case if you fail to get the reference counter or if the
    counter is greater than one.

    We won't allow locking of images for an extended period of time,
    since we have no way of deblocking the shared memory usage for example for
    component configuration changes or component shutdown. Use the default
    SmartSoft communication to synchronize access to an image instance.
   */
  inline bool try_get_ref_count(unsigned int &n)
  {
    if(CommSharedMemoryBase::trylock()==0)
    {
      n = CommSharedMemoryBase::get_ref_count();
      CommSharedMemoryBase::unlock();
      return true;
    }
    return false;
  }

  /**
    Set the valid flag of the data to false.
    The flag is automatically set to true if data is written to the image.
    Thus the server either writes data to the image or sets the valid flag to false.
   */
  void set_data_invalid();

  /**
    Set the sequence counter of the image.
   */
  void set_sequence_counter(unsigned long int n);

  /**
    Copy image data from the memory chunk at \a data to the internal shared memory.
    The number of bytes to copy is derived from the internal format and size descriptions.
   */
  void set_data(const unsigned char *data);

  /** Required by SmartSoft. */
  inline void get(CORBA::Any &a) const { CommSharedMemoryBase::get(a); }

  /** Required by SmartSoft. */
  inline void set(const CORBA::Any &a) { CommSharedMemoryBase::set(a); }

  /** Required by SmartSoft. */
  static inline std::string identifier() { return "CommMutableVideoImage"; }
};


/**
  A resizable image.
  Use this class to create new images.
  Image format, size, and pixel values may be changed.
 */
class CommResizableVideoImage: public CommMutableVideoImage
{
public:
  inline CommResizableVideoImage() {}
  inline CommResizableVideoImage(const CommResizableVideoImage &source) : CommMutableVideoImage(source) {}
  inline CommResizableVideoImage &operator=(const CommResizableVideoImage &source)
  {
    if(&source!=this) CommMutableVideoImage::operator=(source);
    return *this;
  }
  CommResizableVideoImage(unsigned int width, unsigned int height, Format format, const unsigned char *data = 0);

  /**
    Set the parameters of an image.
    This causes currently attached shared memory to be detached, and a new shared memory segment
    to be allocated.
   */
  void set_parameters(unsigned int width, unsigned int height, Format format);
  
  	/**
	 Load from XML represenation on an istream.
	 Be warned, no error checking is done.
	 For details, see \ref smart_basic_com-xml.
	 */
	void load_xml(std::istream &is);
};

inline void CommVideoImage::yuv2rgb(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char &r, unsigned char &g, unsigned char &b)
{
/* Old Version which has not been tested
  const float fy = 1.1643836 * (y -  16);
  const float fu = 1.1339286 * (u - 128);
  const float fv = 1.1339286 * (v - 128);
  const float fr = fy + 1.402 * fv;
  const float fg = fy - 0.344 * fu - 0.714 * fv;
  const float fb = fy + 1.772 * fu;
*/

/* Old Version which function properly and has been used before the libdc1394 version
  const float fr = y + 1.402 * (v - 128);
  const float fg = y - 0.344 * (u - 128) - 0.714 * (v - 128);
  const float fb = y + 1.772 * (u - 128);
*/

/* This block is needed to convert the float back to unsigned char
	r = (fr<0)?0:(fr>255)?255:((unsigned char)::rint(fr));
	g = (fg<0)?0:(fg>255)?255:((unsigned char)::rint(fg));
	b = (fb<0)?0:(fb>255)?255:((unsigned char)::rint(fb));
*/

	/**
	 * This block is taken from the libdc1394 library.
	 *
	 * The only modification is the subtraction of 128 from u and v because we use unsigned and have to shift it.
	 *
	 * The speed up of this version against the old version is about 3 to 6 times.
	 * Old Version conversion has been done in 60 to 70 ms on a 1024x768pixel (YUV422-UYVY) image.
	 * This image has then been converted into RGB32 with the same size.
	 * This Version converts in 10 to 20 ms on the same image.
	 */
	int fr = y + (((v-128)*1436) >> 10);
	int fg = y - (((u-128)*352 + (v-128)*731) >> 10);
	int fb = y + (((u-128)*1814) >> 10);
	fr = fr < 0 ? 0 : fr;
	fg = fg < 0 ? 0 : fg;
	fb = fb < 0 ? 0 : fb;
	r = fr > 255 ? 255 : fr;
	g = fg > 255 ? 255 : fg;
	b = fb > 255 ? 255 : fb;

}


} // namespace Smart

#endif
