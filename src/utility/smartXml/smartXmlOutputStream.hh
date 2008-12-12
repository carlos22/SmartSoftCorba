//--------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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

#ifndef SMART_XML_OUTPUT_STREAM_HH
#define SMART_XML_OUTPUT_STREAM_HH

#include <iostream>

namespace Smart {

/**
  An output stream which delegates output to another output stream,
  and prepends and appends administrative data (XML header) on open and close.
 */ 
class XmlOutputStream: public std::ostream
{
public:
  /**
    Default constructor. 
    Stream is unconnected.
   */
  inline XmlOutputStream() : std::ostream(0), _sbuf(0) {}

  /**
    Create a stream, connected to the given streambuf.
    This ctor calls the \c open() method, which already writes some initialization
    data to the stream.
   */
  inline XmlOutputStream(std::streambuf *sbuf)
  : std::ostream(0), _sbuf(0)
  {
    this->open(sbuf);
  }

  /**
    Create a stream, connected to the given output stream.
    This ctor calls the \c open() method, which already writes some initialization
    data to the stream.
   */
  inline XmlOutputStream(std::ostream &os)
  : std::ostream(0), _sbuf(0)
  {
    this->open(os);
  }

  /**
    Destroy the stream.
    Calls the \c close() method internally, which write some finalizing data to the stream.
   */
  ~XmlOutputStream()
  {
    this->close();
  }

  /**
    Connect this stream with a streambuf to which output will be delegated.
    The following initialization data is written to the stream:
    \code
      <?xml version="1.0"?>
      <smart_xml>
    \endcode
   */
  inline void open(std::streambuf *sbuf)
  {
    this->close();
    _sbuf = sbuf;
    this->init(_sbuf);
    (*this) << "<?xml version=\"1.0\"?>" << std::endl;
    (*this) << "<smart_xml>" << std::endl;
  }
  
  /**
    Connect this stream to another output stream.
    See the other \c open() method for details.
   */
  inline void open(std::ostream &os)
  {
    this->open(os.rdbuf());
  }

  /**
    Close this stream.
    Before this stream is disconnected from its delegation target,
    it writes the following finalizing data
    \code
      </smart_xml>
    \endcode
    which is complementary to the data written by \c open().
   */
  inline void close()
  {
    if(_sbuf)
    {
      (*this) << "</smart_xml>" << std::endl;
      (*this) << std::flush;
      this->init(0);
    }
    _sbuf = 0;
  }
  
private:
  std::streambuf *_sbuf;
};

} // namespace Smart

#endif
