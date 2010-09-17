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
//  This file is part of the "SmartSoft Mapper/GridMapper component".
//  It provides mapping services based on grid maps. The current map
//  represents the latest snapshot of the local surrounding based on
//  laserscans. The current map can be preoccupied by the longterm map.
//  The longterm map is a simple occupancy grid map.
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

#include "smartLtmGridMap.hh"

using namespace Smart;


SmartLtmGridMap::SmartLtmGridMap() : CommGridMap()
{
  // constructor
}


SmartLtmGridMap::SmartLtmGridMap(int x,int y,int size)
{
  // constructor
  _gridmap.id = 0;
  _gridmap.is_valid = false;
  _gridmap.time.sec = 0;
  _gridmap.time.usec = 0;
  _gridmap.xOffsetMM = 0;
  _gridmap.yOffsetMM = 0;
  _gridmap.xOffsetCells = 0;
  _gridmap.yOffsetCells = 0;
  _gridmap.cellSizeMM = size;
  _gridmap.xSizeMM = x;
  _gridmap.ySizeMM = y;
  _gridmap.xSizeCells = (int)ceil((double)x/(double)size);
  _gridmap.ySizeCells = (int)ceil((double)y/(double)size);
  _gridmap.size = _gridmap.xSizeCells*_gridmap.ySizeCells;
  
  _gridmap.cell.length(_gridmap.size); 
}


SmartLtmGridMap::SmartLtmGridMap(int x,int y,int px,int py,int size,int id)
{
  // constructor
  _gridmap.id = id;
  _gridmap.is_valid = false;
  _gridmap.time.sec = 0;
  _gridmap.time.usec = 0;
  _gridmap.xOffsetMM = px;
  _gridmap.yOffsetMM = py;
  _gridmap.xOffsetCells = (int)floor((double)px/(double)size);
  _gridmap.yOffsetCells = (int)floor((double)py/(double)size);
  _gridmap.cellSizeMM = size;
  _gridmap.xSizeMM = x;
  _gridmap.ySizeMM = y;
  _gridmap.xSizeCells = (int)ceil((double)x/(double)size);
  _gridmap.ySizeCells = (int)ceil((double)y/(double)size);
  _gridmap.size = _gridmap.xSizeCells*_gridmap.ySizeCells;

  _gridmap.cell.length(_gridmap.size);
}


/* use auto generated - for more information check the header file

SmartLtmGridMap const &SmartLtmGridMap::operator=(SmartLtmGridMap const &other)
{
  // assignment operator
  if (this != &other)
  {
    _gridmap = other._gridmap;
    _gridmap.cell.length(_gridmap.size);

    for ( unsigned int i=0; i < _gridmap.size; i++)
    {
      _gridmap.cell[i] = other._gridmap.cell[i];
    }

  }
  return (*this);
}

  
SmartLtmGridMap::SmartLtmGridMap(SmartLtmGridMap const &other)
{
  // copy constructor
  _gridmap = other._gridmap;
  _gridmap.cell.length(_gridmap.size);

  for ( unsigned int i=0; i < _gridmap.size; i++)
  {
    _gridmap.cell[i] = other._gridmap.cell[i];
  }
}
*/
 
SmartLtmGridMap::~SmartLtmGridMap()
{
  // destructor
}


int SmartLtmGridMap::occupyLine(int x1,int y1,int x2,int y2,int n)
{
  int     x,y,z,dx,dy,dz,i1,i2;
  int     xa,ya,xs,ys;
  int     index;

  xa = (int)(floor(x1/(double)_gridmap.cellSizeMM)-(double)_gridmap.xOffsetCells);
  ya = (int)(floor(y1/(double)_gridmap.cellSizeMM)-(double)_gridmap.yOffsetCells);
  xs = (int)(floor(x2/(double)_gridmap.cellSizeMM)-(double)_gridmap.xOffsetCells);
  ys = (int)(floor(y2/(double)_gridmap.cellSizeMM)-(double)_gridmap.yOffsetCells);

  dx = abs(xa-xs);
  dy = abs(ya-ys);

  if (xa<xs) 
  {
    x = xa;
    y = ya;
    if (ya>ys) z=-1; else z=1;
  } 
  else 
  {
    x = xs;
    y = ys;
    if (ys>ya) z=-1; else z=1;
  }

  if (dx>dy) i2=dx; else i2=dy;
  dz=i2/2;

  for (i1=0;i1<=i2;i1++) 
  {
    if (dz<dx) 
    {
      dz = dz+dy;
      x = x+1;
    }
    if (dz>=dx) 
    {
      dz = dz-dx;
      y = y+z;
    }

    if ((0<=x) && (x < (int)_gridmap.xSizeCells) && (0<=y) && (y < (int)_gridmap.ySizeCells)) 
    {
      index      = x + y * _gridmap.xSizeCells;
      _gridmap.cell[index] = (unsigned char)n;
    }
  }
  return 0;
}


int SmartLtmGridMap::update( Smart::CommMobileLaserScan const &scan, double n )
{
  int xa,ya,xs,ys;
  double a, x, y, x1, y1;
  unsigned int i;
  int status = -1;
  double laserPolarAngleRad; // = laserPolarAngle*M_PI/180.0;
  double laserPolarDistance; // = 1000.0*lp[j];

  for (i=0; i < scan.get_scan_size(); i++)
  {
    laserPolarAngleRad = i/2.0 * M_PI / 180.0;
     a =  scan.get_base_state().get_base_position().get_base_alpha(); // angle of robot in real world
    laserPolarDistance = scan.get_scan_distance(i);

    // draw line from scanner pos to measured laser point
    xa = (int)(floor(scan.get_scanner_x() /(double)_gridmap.cellSizeMM)-(double)_gridmap.xOffsetCells);
    ya = (int)(floor(scan.get_scanner_y() /(double)_gridmap.cellSizeMM)-(double)_gridmap.yOffsetCells);

    scan.get_scan_cartesian_point_world( (unsigned int) i, x, y );
    scan.get_scan_cartesian_point_scanner( (unsigned int) i, x1, y1 );

    xs = (int)(floor(x /(double)_gridmap.cellSizeMM)-(double)_gridmap.xOffsetCells);
    ys = (int)(floor(y /(double)_gridmap.cellSizeMM)-(double)_gridmap.yOffsetCells);

    status = bresenham( xa, ya, xs, ys, n);
  }
  return status;
}


int SmartLtmGridMap::clearMap(int value)
{
  unsigned int x,y;

  for (x=0; x < _gridmap.xSizeCells; x++)
  {
    for (y=0; y < _gridmap.ySizeCells; y++)
    {
      _gridmap.cell[x+y * _gridmap.xSizeCells] = value;
    }
  }

  return drawBorder();
}


int SmartLtmGridMap::getPartialMap( int partXoffset,int partYoffset,
                                    int partXsize,int partYsize,
                                    int threshold, 
                                    SmartIDL::GridMap::_tao_seq_CORBA_Octet_ &destCell)
{
//  std::cout << "SmartLtmGridMap::getPartialMap - not implemented yet !!\n";

  int xCells,yCells;           // number of cells to be copied
  int xbCells,ybCells;         // begin of cells to be copied in LTM
  int sourceIndex,destIndex;
  int sourceValue,destValue;

  xCells  = (int)ceil(    (double)partXsize  / (double)_gridmap.cellSizeMM);
  yCells  = (int)ceil(    (double)partYsize  / (double)_gridmap.cellSizeMM);
  xbCells = (int)floor( ( (double)partXoffset/ (double)_gridmap.cellSizeMM) - (double)_gridmap.xOffsetCells);
  ybCells = (int)floor( ( (double)partYoffset/ (double)_gridmap.cellSizeMM) - (double)_gridmap.yOffsetCells);

  for (int y=0; y < yCells; y++) 
  {
    for (int x=0; x < xCells; x++) 
    {
      sourceIndex = (x+xbCells) + (y+ybCells) * _gridmap.xSizeCells;
      destIndex   = (x+y*xCells);
      sourceValue = (int)_gridmap.cell[sourceIndex];
      destValue   = (int)destCell[destIndex];

      if (destValue == MAPPER_GROWING) 
      {
        // destination cell will not be overwritten !
      } 
      else 
      {
        if (sourceValue < threshold) 
        {
          // mark destination cell as free
          destCell[destIndex] = MAPPER_FREE;
        } 
        else 
        {
          // mark destination cell as occupied
          destCell[destIndex] = MAPPER_OBSTACLE;
        }
      }
    }
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//PRIVATE MEMBERS

int SmartLtmGridMap::drawBorder(void)
{
  unsigned int x,y; 

  for ( x=0; x < _gridmap.xSizeCells; x++)
  {
    _gridmap.cell[x + 0* _gridmap.xSizeCells] = MAPPER_UNDELETABLE;
    _gridmap.cell[x + (_gridmap.ySizeCells-1) * _gridmap.xSizeCells] = MAPPER_UNDELETABLE;
  }

  for (y=0; y < _gridmap.ySizeCells; y++)
  {
    _gridmap.cell[ 0 + y * _gridmap.xSizeCells] = MAPPER_UNDELETABLE;
    _gridmap.cell[ (_gridmap.xSizeCells - 1) + y * _gridmap.xSizeCells] = MAPPER_UNDELETABLE;
  }

  return 0;
}

int SmartLtmGridMap::bresenham(int xa,int ya,int xs,int ys,double n)
{
  int     x,y,z,dx,dy,dz,i1,i2;
  int     index;
  double  oldvalue;
  double  update1,update2;
  double  free,occupied;

  free     = 0.0;
  occupied = 255.0;

  dx = abs(xa-xs);
  dy = abs(ya-ys);

  if (xa < xs) 
  {
    x = xa;
    y = ya;
    update1 = free;
    update2 = occupied;
    if (ya>ys) z=-1; else z=1;
  } 
  else 
  {
    x = xs;
    y = ys;
    update1 = occupied;
    update2 = free;
    if (ys>ya) z=-1; else z=1;
  }

  if (dx > dy) i2=dx; 
  else i2=dy;

  dz = i2/2;

  if ( (0<=x) && (x < (int)_gridmap.xSizeCells) && (0<=y) && (y < (int)_gridmap.ySizeCells) ) 
  {
    index      = x + y * _gridmap.xSizeCells;
    oldvalue   = (double)_gridmap.cell[index];
    _gridmap.cell[index] = (unsigned char)(oldvalue + floor(1/n*(update1-oldvalue)));
  }

  for ( i1=1; i1 <= i2; i1++) 
  {
    if (dz < dx) 
    {
      dz = dz+dy;
      x = x+1;
    }

    if (dz >= dx) 
    {
      dz = dz-dx;
      y = y+z;
    }

    if ( (0<=x) && (x < (int)_gridmap.xSizeCells) && (0<=y) && (y < (int)_gridmap.ySizeCells) ) 
    {
      if (i1!=i2) 
      {
        index      = x + y * _gridmap.xSizeCells;
        oldvalue   = (double)_gridmap.cell[index];
        _gridmap.cell[index] = (unsigned char)(oldvalue+floor(1/n*(free-oldvalue)));
      }
      if (i1==i2) 
      {
        index      = x + y * _gridmap.xSizeCells;
        oldvalue   = (double)_gridmap.cell[index];
        _gridmap.cell[index] = (unsigned char)(oldvalue+floor(1/n*(update2-oldvalue)));
      }
    }
  } // for

  return 0;
}

