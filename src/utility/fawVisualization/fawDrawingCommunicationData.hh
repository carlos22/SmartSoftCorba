//--------------------------------------------------------------------------
//
//  Copyright (C) 2002 Boris Kluge
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
 
#ifndef FAW_DRAWING_COMM_DATA_HH
#define FAW_DRAWING_COMM_DATA_HH

#include "fawSimpleArray.hh"
#include "fawDeflatingOutputStream.hh"
#include "fawBufferOutputStream.hh"
#include "fawBufferInputStream.hh"
#include "fawInflatingInputStream.hh"

#include "fawDrawingStoreReplayPainter.hh"

class FawDrawingCommunicationData
{
  FawSimpleArray<char> _data;

public:
  static const char *const NAME = "FawDrwCommData";
  static const char *const FMT  = "int,{int,<char:1>}";

  inline FawDrawingCommunicationData()  {}

  inline FawDrawingCommunicationData(const FawDrawingStoreReplayPainter &srp)
  { 
    set(srp); 
  }

  inline void print(std::ostream &os = cout) const 
  { 
    os << "FawDrawingCommunicationData(" /* << get()*/ << ")" << std::endl; 
  }

  inline void clear()
  {
    _data.clear();
  }

  inline void set(const FawDrawingStoreReplayPainter &painter) 
  {
    FawBufferOutputStream bos;
    FawDeflatingOutputStream dos(bos);
    painter.save(dos);
    dos.close();
    _data.set(bos.begin(), bos.end());
//cerr << "*** " << dos.uncompressed_bytes_count() << "\t" << dos.compressed_bytes_count() << std::endl;
  }

  inline void get(FawDrawingStoreReplayPainter &painter) const 
  { 
    FawBufferInputStream<const char*> bis(_data.begin(), _data.end());
    FawInflatingInputStream iis(bis);
    painter.load(iis);
  }

/*
  inline string get() const 
  { 
    char inflated_data[_uncompressed_size + 1];
    inflated_data[_uncompressed_size] = '\0';
    FawBufferInputStream<const char*> bis(_data.begin(), _data.end());
    FawInflatingInputStream iis(bis);
    for(int i=0; (i<_uncompressed_size) && (iis.good()); ++i) iis.get(inflated_data[i]);
    return inflated_data; 
  }
*/
  inline void save(std::ostream &os) const
  {
    char ch;
    FawBufferInputStream<const char*> bis(_data.begin(), _data.end());
    FawInflatingInputStream iis(bis);
    while(!iis.get(ch).eof()) os.put(ch);
  }

  inline void replay(FawPainter &painter) const
  {
    FawBufferInputStream<const char*> bis(_data.begin(), _data.end());
    FawInflatingInputStream iis(bis);
    FawDrawingStoreReplayPainter::replay(iis, painter);
  }
};

inline std::ostream &operator<<(std::ostream &os, const FawDrawingCommunicationData &u)
{
  u.print(os); return os;
}

#endif
