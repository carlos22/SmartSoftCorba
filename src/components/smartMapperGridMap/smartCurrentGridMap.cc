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

#include "smartCurrentGridMap.hh"

using namespace Smart;


SmartCurrentGridMap::SmartCurrentGridMap() : CommGridMap()
{
  // constructor
  initializeGrowing();
}


SmartCurrentGridMap::SmartCurrentGridMap(int x,int y,int size,int growing)
{
  // constructor
  growingType = growing;
  growingSize = 0;

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
  initializeGrowing();
}


SmartCurrentGridMap::SmartCurrentGridMap(int x,int y,int px,int py,int size,int growing,int id)
{
  // constructor
  growingType = growing;
  growingSize = 0;

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
  initializeGrowing();

}

/* use auto generated - for more information check the header file

SmartCurrentGridMap const &SmartCurrentGridMap::operator=(SmartCurrentGridMap const &other)
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


SmartCurrentGridMap::SmartCurrentGridMap(SmartCurrentGridMap const &other)
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

SmartCurrentGridMap::~SmartCurrentGridMap()
{
  // destructor
}


int SmartCurrentGridMap::clearMap()
{
  unsigned int x,y;

  for (x=0; x < _gridmap.xSizeCells; x++) 
  {
    for (y=0; y < _gridmap.ySizeCells; y++) 
    {
      _gridmap.cell[x+y * _gridmap.xSizeCells] = MAPPER_FREE;
    }
  }

  return drawBorder();
}


int SmartCurrentGridMap::update( Smart::CommMobileLaserScan const &scan ) 
{
  int xa,ya,xs,ys;
  double a, x, y, x1, y1;
  int status;
  unsigned int i;
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

    status = bresenham(xa,ya,xs,ys);  // draw line
    status = obstacleGrowing(xs,ys);  // do obstacle growing
  }
  return status;
}


int SmartCurrentGridMap::setLtmOccupation( int threshold, Smart::SmartLtmGridMap &ltm )
{
  //std::cout << "SmartCurrentGridMap::setLtmOccupation - not implemented yet !!\n";
  int status;
 
  status = ltm.getPartialMap(_gridmap.xOffsetMM,
                             _gridmap.yOffsetMM,
                             _gridmap.xSizeMM,
                             _gridmap.ySizeMM,
                             threshold,
                             _gridmap.cell);
  return status;
}




int SmartCurrentGridMap::convertObstacleGrowing()
{
  unsigned int counter;
  int status;

  for (counter=0; counter < (_gridmap.xSizeCells*_gridmap.ySizeCells); counter++) 
  {
    if (_gridmap.cell[counter] == MAPPER_GROWING) 
    {
      _gridmap.cell[counter] = MAPPER_OBSTACLE;
    }
  }
  status = drawBorder();

  return status;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//PRIVATE MEMBERS

int SmartCurrentGridMap::drawBorder(void)
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


int SmartCurrentGridMap::initializeGrowing(void)
{
  int status;

  switch (growingType) {
    case MAPPER_GROWING_NO:
      growingSize   = 0;
      status        = 0;
      break;
    case MAPPER_GROWING_CIRCLE_16:
      growingSize   = 16;
      growing_x[0]  =  3;
      growing_x[1]  =  3;
      growing_x[2]  =  2;
      growing_x[3]  =  1;
      growing_x[4]  =  0;
      growing_x[5]  = -1;
      growing_x[6]  = -2;
      growing_x[7]  = -3;
      growing_x[8]  = -3;
      growing_x[9]  = -3;
      growing_x[10] = -2;
      growing_x[11] = -1;
      growing_x[12] =  0;
      growing_x[13] =  1;
      growing_x[14] =  2;
      growing_x[15] =  3;
      growing_y[0]  =  0;
      growing_y[1]  = -1;
      growing_y[2]  = -2;
      growing_y[3]  = -3;
      growing_y[4]  = -3;
      growing_y[5]  = -3;
      growing_y[6]  = -2;
      growing_y[7]  = -1;
      growing_y[8]  =  0;
      growing_y[9]  =  1;
      growing_y[10] =  2;
      growing_y[11] =  3;
      growing_y[12] =  3;
      growing_y[13] =  3;
      growing_y[14] =  2;
      growing_y[15] =  1;
      status        =  0;
      break;
    case MAPPER_GROWING_STAR_16:
      growingSize   = 16;
      growing_x[0]  =  4;
      growing_x[1]  = -4;
      growing_x[2]  = -4;
      growing_x[3]  =  4;
      growing_x[4]  =  3;
      growing_x[5]  = -3;
      growing_x[6]  = -3;
      growing_x[7]  =  3;
      growing_x[8]  =  2;
      growing_x[9]  = -2;
      growing_x[10] = -2;
      growing_x[11] =  2;
      growing_x[12] =  1;
      growing_x[13] = -1;
      growing_x[14] = -1;
      growing_x[15] =  1;
      growing_y[0]  =  4;
      growing_y[1]  =  4;
      growing_y[2]  = -4;
      growing_y[3]  = -4;
      growing_y[4]  =  3;
      growing_y[5]  =  3;
      growing_y[6]  = -3;
      growing_y[7]  = -3;
      growing_y[8]  =  2;
      growing_y[9]  =  2;
      growing_y[10] = -2;
      growing_y[11] = -2;
      growing_y[12] =  1;
      growing_y[13] =  1;
      growing_y[14] = -1;
      growing_y[15] = -1;
      status        =  0;
      break;
    case MAPPER_GROWING_STAR_12:
      growingSize   = 12;
      growing_x[0]  =  3;
      growing_x[1]  = -3;
      growing_x[2]  = -3;
      growing_x[3]  =  3;
      growing_x[4]  =  2;
      growing_x[5]  = -2;
      growing_x[6]  = -2;
      growing_x[7]  =  2;
      growing_x[8]  =  1;
      growing_x[9]  = -1;
      growing_x[10] = -1;
      growing_x[11] =  1;
      growing_y[0]  =  3;
      growing_y[1]  =  3;
      growing_y[2]  = -3;
      growing_y[3]  = -3;
      growing_y[4]  =  2;
      growing_y[5]  =  2;
      growing_y[6]  = -2;
      growing_y[7]  = -2;
      growing_y[8]  =  1;
      growing_y[9]  =  1;
      growing_y[10] = -1;
      growing_y[11] = -1;
      status        =  0;
      break;
    case MAPPER_GROWING_CIRCLE_8:
      growingSize   =  8;
      growing_x[0]  =  2;
      growing_x[1]  =  1;
      growing_x[2]  =  0;
      growing_x[3]  = -1;
      growing_x[4]  = -2;
      growing_x[5]  = -1;
      growing_x[6]  =  0;
      growing_x[7]  =  1;
      growing_y[0]  =  0;
      growing_y[1]  =  1;
      growing_y[2]  =  2;
      growing_y[3]  =  1;
      growing_y[4]  =  0;
      growing_y[5]  = -1;
      growing_y[6]  = -2;
      growing_y[7]  = -1;
      status        =  0;
      break;
    default:
      status = 1;
      break;
  }
  return status;
}


int SmartCurrentGridMap::bresenham(int xa,int ya,int xs,int ys)
{
  int x,y,z,dx,dy,dz,i1,i2,index;
  unsigned char update1,update2;
    
  dx = abs(xa-xs);
  dy = abs(ya-ys);

  if (xa<xs) 
  {
    x = xa;
    y = ya;
    update1 = MAPPER_FREE;
    update2 = MAPPER_OBSTACLE;
    if (ya>ys) z=-1; else z=1;
  } 
  else 
  {
    x = xs;
    y = ys;
    update1 = MAPPER_OBSTACLE;
    update2 = MAPPER_FREE;
    if (ys>ya) z=-1; else z=1;
  }

  if (dx>dy) i2=dx; else i2=dy;
  dz = i2/2;

  // the map border must not be destroyed during update
  if ((0<x) && (x < (int)_gridmap.xSizeCells-1) && (0<y) && (y < (int)_gridmap.ySizeCells-1)) {
    _gridmap.cell[x+y * _gridmap.xSizeCells] = update1;
  }

  for (i1=1; i1 <=i2; i1++) 
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

    index = x+y*_gridmap.xSizeCells;
    // the map border must not be destroyed during update
    if ((0<x) && (x < (int)_gridmap.xSizeCells-1) && (0<y) && (y < (int)_gridmap.ySizeCells-1)) 
    {
      if (i1!=i2) 
      {
        if ((_gridmap.cell[index] != MAPPER_GROWING) && (_gridmap.cell[index] != MAPPER_UNDELETABLE)) 
        {
          _gridmap.cell[index] = MAPPER_FREE;
        }
      }
      if (i1==i2) _gridmap.cell[index] = update2;
    }
  }
  return 0;
}


int SmartCurrentGridMap::obstacleGrowing(int x,int y)
{
  int x1,y1;

  for (int i=0; i < growingSize; i++) {
    x1 = x + growing_x[i];
    y1 = y + growing_y[i];

    if ((0<=x1) && (x1 < (int)_gridmap.xSizeCells) && (0<=y) && (y1 < (int)_gridmap.ySizeCells)) 
    {
      if (_gridmap.cell[x1+y1*_gridmap.xSizeCells] != MAPPER_UNDELETABLE) 
      {
        _gridmap.cell[x1+y1*_gridmap.xSizeCells] = MAPPER_GROWING;
      }
    }
  }
  return 0;
}

