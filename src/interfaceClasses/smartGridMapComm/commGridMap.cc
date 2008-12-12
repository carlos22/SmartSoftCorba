// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Christian Schlegel, Andreas Steck
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de 
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Classes".
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
// --------------------------------------------------------------------------

#include "smartKnuthMorrisPratt.hh"
#include "commGridMap.hh"

using namespace Smart;

CommGridMap::CommGridMap()
{
  _gridmap.id = 0;
  _gridmap.is_valid = false;
  _gridmap.time.sec = 0;
  _gridmap.time.usec = 0;
  _gridmap.xOffsetMM = 0;
  _gridmap.yOffsetMM = 0;
  _gridmap.xOffsetCells = 0;
  _gridmap.yOffsetCells = 0;
  _gridmap.cellSizeMM = 0;
  _gridmap.xSizeMM = 0;
  _gridmap.ySizeMM = 0;
  _gridmap.xSizeCells = 0;
  _gridmap.ySizeCells = 0;
  _gridmap.size = 0;
}


CommGridMap::CommGridMap(const SmartIDL::GridMap &gridmap)
: _gridmap(gridmap)
{
}


void CommGridMap::get(CORBA::Any &a) const
{
  a <<= _gridmap;
}


CommGridMap::~CommGridMap()
{
}


void CommGridMap::set(const CORBA::Any &a)
{
  SmartIDL::GridMap *tmp_gridmap = 0;
  if(a >>= tmp_gridmap)
  {
    _gridmap = *tmp_gridmap;
  }
}


int CommGridMap::get_parameter( unsigned int &id, 
                                bool &is_valid, 
                                struct timeval &time, 
                                int &xOffsetMM, 
                                int &yOffsetMM,
                                int &xOffsetCells, 
                                int &yOffsetCells, 
                                unsigned int &cellSizeMM, 
                                unsigned int &xSizeMM, 
                                unsigned int &ySizeMM, 
                                unsigned int &xSizeCells, 
                                unsigned int &ySizeCells ) const
{
  id           = _gridmap.id;
  is_valid     = _gridmap.is_valid;
  time.tv_sec  = _gridmap.time.sec; 
  time.tv_usec = _gridmap.time.usec;
  xOffsetMM    = _gridmap.xOffsetMM;
  yOffsetMM    = _gridmap.yOffsetMM;
  xOffsetCells = _gridmap.xOffsetCells;
  yOffsetCells = _gridmap.yOffsetCells;
  cellSizeMM   = _gridmap.cellSizeMM;
  xSizeMM      = _gridmap.xSizeMM;
  ySizeMM      = _gridmap.ySizeMM;
  xSizeCells   = _gridmap.xSizeCells;
  ySizeCells   = _gridmap.ySizeCells;

  return 0;
}


void CommGridMap::save_xpm( std::ostream &os ) const
{
  int a;
  os << "/* XPM */\n";
  os << "static char *noname[] = {\n";
  os << "/* width height ncolors chars_per_pixel */\n";
  os << "\"" << _gridmap.xSizeCells << " " << _gridmap.ySizeCells << " 131 2\",\n";
  os << "/* colors */\n";

  // set grey values for free cells of actual map
  // 0    empty cell     => color value 255/255/255 (white)
  // ...
  // 127  occupied cell  => color value 1/1/1       (black)

  // set the fill character to '0' -> together with std::hex and std::setw(2)
  //                                  the value will be printed with length 2: 00, 01, 02, ..., 0f, 10, ...
  os << std::setfill('0'); 

  for (int h=0;h<128;h++) 
  {
    //                                
    os << "\"" << std::hex << std::setw(2) << h << " c " 
       << "#" << std::setw(2) << (255-2*h) 
              << std::setw(2) << (255-2*h) 
              << std::setw(2) << (255-2*h) << "\",\n"; 
  }

  // special value: obstacle (yellow) (0,255,255)
  os << "\"" << MAPPER_OBSTACLE << " c "
     << "#" << std::setw(2) << 0 
            << std::setw(2) << 255 
            << std::setw(2) << 255 << "\",\n"; 
      
  // special value: growing (green) (0,255,0)
  os << "\"" << MAPPER_GROWING << " c " 
     << "#" << std::setw(2) << 0 
            << std::setw(2) << 255 
            << std::setw(2) << 0 << "\",\n"; 

  // special value: undeletable (red) (255,0,0)
  os << "\"" << MAPPER_UNDELETABLE << " c " 
     << "#" << std::setw(2) << 255 
            << std::setw(2) << 0 
            << std::setw(2) << 0 << "\",\n"; 

  os << "/* pixels */\n";

  for (int y=(_gridmap.ySizeCells-1); y >= 0; y--) 
  {
    os << "\"";
    for (unsigned int x=0; x < _gridmap.xSizeCells; x++) 
    {
      a = (int)(_gridmap.cell[x+y*_gridmap.xSizeCells]);
      os << std::setw(2) <<  a;
    }
    if (y==0) {
      os << "\"\n";
    } else {
      os << "\",\n";
    }
  }

  os << "};\n";
}


void CommGridMap:: save_xml( std::ostream &os, const std::string &indent ) const
{
  os << indent << "<grid_map>" << std::endl;

  os << indent << "  <id>" << _gridmap.id << "</id>" << std::endl;
  os << indent << "  <valid>" << int(_gridmap.is_valid) << "</valid>" << std::endl;

  // timestamp
  get_time_stamp().save_xml(os, indent + "  ");

  os << indent << "  <xOffsetMM>" << int(_gridmap.xOffsetMM) << "</xOffsetMM>" << std::endl;
  os << indent << "  <yOffsetMM>" << int(_gridmap.yOffsetMM) << "</yOffsetMM>" << std::endl;
  os << indent << "  <xOffsetCells>" << int(_gridmap.xOffsetCells) << "</xOffsetCells>" << std::endl;
  os << indent << "  <yOffsetCells>" << int(_gridmap.yOffsetCells) << "</yOffsetCells>" << std::endl;
  os << indent << "  <cellSizeMM>" << int(_gridmap.cellSizeMM) << "</cellSizeMM>" << std::endl;
  os << indent << "  <xSizeMM>" << int(_gridmap.xSizeMM) << "</xSizeMM>" << std::endl;
  os << indent << "  <ySizeMM>" << int(_gridmap.ySizeMM) << "</ySizeMM>" << std::endl;
  os << indent << "  <xSizeCells>" << int(_gridmap.xSizeCells) << "</xSizeCells>" << std::endl;
  os << indent << "  <ySizeCells>" << int(_gridmap.ySizeCells) << "</ySizeCells>" << std::endl;

  // map cells
  os << indent << "  <map n=\"" << _gridmap.size << "\">" << std::endl;
  os << std::hex << std::setfill('0');
  for(unsigned int i=0; i < _gridmap.size; ++i)
  {
    os << indent << "    <cell value=\"" << std::setw(2) << (int)_gridmap.cell[i] << "\"/>" << std::endl;
  }
  os << indent << "  </map>" << std::endl;

  os << indent << "</grid_map>" << std::endl;

}


void CommGridMap:: load_xml( std::istream &is )
{
  int b;
  
  static const KnuthMorrisPratt kmp_begin("<grid_map>");
  static const KnuthMorrisPratt kmp_id("<id>");
  static const KnuthMorrisPratt kmp_valid("<valid>");

  static const KnuthMorrisPratt kmp_xOffsetMM("<xOffsetMM>");
  static const KnuthMorrisPratt kmp_yOffsetMM("<yOffsetMM>");
  static const KnuthMorrisPratt kmp_xOffsetCells("<xOffsetCells>");
  static const KnuthMorrisPratt kmp_yOffsetCells("<yOffsetCells>");
  static const KnuthMorrisPratt kmp_cellSizeMM("<cellSizeMM>");
  static const KnuthMorrisPratt kmp_xSizeMM("<xSizeMM>");
  static const KnuthMorrisPratt kmp_ySizeMM("<ySizeMM>");
  static const KnuthMorrisPratt kmp_xSizeCells("<xSizeCells>");
  static const KnuthMorrisPratt kmp_ySizeCells("<ySizeCells>");

  static const KnuthMorrisPratt kmp_map_cells("<map n=\"");
  static const KnuthMorrisPratt kmp_cell_value("<cell value=\"");

  static const KnuthMorrisPratt kmp_end("</grid_map>");

  kmp_begin.search(is);

  // <id>
  kmp_id.search(is);
  is >> b;
  _gridmap.id = b;

  // <valid>
  kmp_valid.search(is);
  is >> b;
  _gridmap.is_valid = (bool)b;

  // timestamp
  CommTimeStamp ts;
  ts.load_xml(is);
  set_time_stamp(ts);

  // <xOffsetMM>
  kmp_xOffsetMM.search(is);
  is >> b;
  _gridmap.xOffsetMM = b;
  
  // <yOffsetMM>
  kmp_yOffsetMM.search(is);
  is >> b;
  _gridmap.yOffsetMM = b;

  // <xOffsetCells>
  kmp_xOffsetCells.search(is);
  is >> b;
  _gridmap.xOffsetCells = b;

  // <yOffsetCells>
  kmp_yOffsetCells.search(is);
  is >> b;
  _gridmap.yOffsetCells = b;

  // <cellSizeMM>
  kmp_cellSizeMM.search(is);
  is >> b;
  _gridmap.cellSizeMM = b;

  // <xSizeMM>
  kmp_xSizeMM.search(is);
  is >> b;
  _gridmap.xSizeMM = b;

  // <ySizeMM>
  kmp_ySizeMM.search(is);
  is >> b;
  _gridmap.ySizeMM = b;

  // <xSizeCells>
  kmp_xSizeCells.search(is);
  is >> b;
  _gridmap.xSizeCells = b;

  // <ySizeCells>
  kmp_ySizeCells.search(is);
  is >> b;
  _gridmap.ySizeCells = b;

  unsigned int n;
  kmp_map_cells.search(is);
  is >> n;
  _gridmap.size = n;
  _gridmap.cell.length(n);


  for(unsigned int i=0; i<n; ++i)
  {
    kmp_cell_value.search(is);
    is >> std::hex >> b;
    _gridmap.cell[i] = b;
  }

  kmp_end.search(is);

}

