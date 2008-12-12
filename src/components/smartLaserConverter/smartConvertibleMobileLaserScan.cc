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


#include "smartConvertibleMobileLaserScan.hh"

////////////////////////////////////////////////////////////////////
// save_formatSteffen
int Smart::SmartConvertibleMobileLaserScan::save_formatSteffen( char *filename, int flag ) const
{
  int i;
  FILE *file;

  file = fopen(filename,"w");
  if (file==NULL) {
    printf("Unable to open data file %s for scan.\n",filename);
    return 1;
  }

  if (flag==0) 
  {
    fprintf(file,"# Created by CHS/SmartSoft Ulm\n");
  }
  else 
  {
    fprintf(file,"# Created by Steffen's Scan Studio, University of Freiburg\n");
  }

  fprintf(file,"2D-Scan\n");
  fprintf(file,"RobotPos: %lf %lf %lf\n",
         this->get_base_state().get_base_position().get_x(),
         this->get_base_state().get_base_position().get_y(),
         this->get_base_state().get_base_position().get_base_alpha() );

  fprintf(file,"RobotSigma: %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
         this->get_base_state().get_base_position().get_cov(0,0),
         this->get_base_state().get_base_position().get_cov(0,1),
         this->get_base_state().get_base_position().get_cov(0,2),
         this->get_base_state().get_base_position().get_cov(1,0),
         this->get_base_state().get_base_position().get_cov(1,1),
         this->get_base_state().get_base_position().get_cov(1,2),
         this->get_base_state().get_base_position().get_cov(2,0),
         this->get_base_state().get_base_position().get_cov(2,1),
         this->get_base_state().get_base_position().get_cov(2,2) );

  fprintf(file,"ScannerRelPos: %lf %lf %lf\n", 
         this->get_scanner_x() - this->get_base_state().get_base_position().get_x(),
         this->get_scanner_y() - this->get_base_state().get_base_position().get_y(),
         this->get_scanner_azimuth() - this->get_base_state().get_base_position().get_base_alpha() );

  fprintf(file,"NumPoints: %d\n", this->get_scan_size() );
  fprintf(file,"DATA\n");

  for (i=0;i < (int)this->get_scan_size(); i++) {
    fprintf(file,"%lf %lf\n",
        this->get_scan_angle(i),
        this->get_scan_distance(i) );
  }
  
  fclose(file);
  return 0;
}


////////////////////////////////////////////////////////////////////
// load_formatSteffen
bool Smart::SmartConvertibleMobileLaserScan::load_formatSteffen( char *filename, int flag )
{
  // <info: this information is not contained in the scan%04d.2d files !>
  const double startAngle = 270.0;
  const double resolution = 0.5;
  // </info>

  FILE *file;
  int    i,num,numberScanPoints;
  double posx,posy;
  double posa;
  double scanrelx,scanrely,scanrela;
  double angle,distance;
  double covM[3][3];

  Smart::CommBaseState    state;
  Smart::CommBasePosition pos;
  Smart::CommBaseVelocity vel;

  file = fopen(filename,"r");
  if (file==NULL) {
    fprintf(stderr,"Unable to open data file %s for reading\n",filename);
    this->set_scan_valid( false );
    return false;
  } 
  else 
  {
    // read first line
    if (flag==0) 
    {
      fscanf( file,"# Created by CHS/SmartSoft Ulm\n");
    } 
    else 
    {
      fscanf( file,"# Created by Steffen's Scan Studio, University of Freiburg\n");
    }

    // read second line
    fscanf( file,"2D-Scan\n");

    // read third line
    num = fscanf( file,"RobotPos: %lf %lf %lf\n",&posx,&posy,&posa);
    if (num!=3) 
    {
      fprintf(stderr,"Error in line 3: wrong position format\n");
      fclose( file);
      this->set_scan_valid( false );
      return false;
    } 
    else 
    {
      pos.set_x( posx );
      pos.set_y( posy );
      pos.set_z( 0.0 );
      pos.set_base_alpha( posa );
    }

    // read fourth line
    num = fscanf( file,"RobotSigma: %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                  &covM[0][0], &covM[0][1], &covM[0][2], 
                  &covM[1][0], &covM[1][1], &covM[1][2], 
                  &covM[2][0], &covM[2][1], &covM[2][2] ); 

    if (num!=9) {
      fprintf(stderr,"Error in line 4: wrong error covariance matrix format\n");
      fclose( file);
      this->set_scan_valid( false );
      return false;
    }
    else
    {
      pos.set_cov(0,0, covM[0][0] );
      pos.set_cov(0,1, covM[0][1] );
      pos.set_cov(0,2, covM[0][2] );
      pos.set_cov(1,0, covM[1][0] );
      pos.set_cov(1,1, covM[1][1] );
      pos.set_cov(1,2, covM[1][2] );
      pos.set_cov(2,0, covM[2][0] );
      pos.set_cov(2,1, covM[2][1] );
      pos.set_cov(2,2, covM[2][2] );
    }

    // read fifth line
    num = fscanf( file,"ScannerRelPos: %lf %lf %lf\n",
      &scanrelx,&scanrely,&scanrela);
    if (num!=3) {
      fprintf(stderr,"Error in line 5: wrong scanner position information\n");
      fclose( file);
      this->set_scan_valid( false );
      return false;
    } 
    else 
    {
      this->set_scanner_x( scanrelx + posx );
      this->set_scanner_y( scanrely + posy );
      this->set_scanner_z( 0.0 );
      this->set_scanner_azimuth( NormAngle(scanrela + posa ) );
    }

    // read sixth line
    num = fscanf( file,"NumPoints: %d\n",&(numberScanPoints));
    if (num!=1) {
      fprintf(stderr,"Error in line 6: wrong format of number of points\n");
      fclose( file);
      this->set_scan_valid( false );
      return false;
    }
    this->set_scan_size( numberScanPoints );

    // read seventh line
    num = fscanf( file,"DATA\n");

    // laserscan starts at angle 270° with a resolution of 0.5°
    this->set_scan_integer_field_of_view( int(startAngle*100), int(resolution*100) );

     // read data part of laser scan
    for (i=0;i<numberScanPoints;i++) {
      num = fscanf( file,"%lf %lf\n", &angle, &distance );
      if (num!=2) {
        fprintf(stderr,"Error in data area: wrong data format\n");
        fclose( file);
        this->set_scan_valid( false );
        return false;
      } 
      else 
      {
        // the new laserscan doesn't store the angle directly -> thus convert angle to index
        // angle in degree
        double a = angle/M_PI*180.0; 
        // convert angle in range 0 to PI (0°-180°)
        a -= startAngle;
        if( a < 0 ) a += 360.0;
        // set index by angle / resolution
        this->set_scan_index( i, rint( a / resolution) );
        // set distance
        this->set_scan_distance (i, distance);
      }
    }

    state.set_base_position( pos );
    state.set_base_velocity( vel );
    this->set_base_state( state );

    this->set_scan_valid( true );
    fclose( file);
  } // else

  return true; 
}

////////////////////////////////////////////////////////////////////
// save_xFig (char *filename)
void Smart::SmartConvertibleMobileLaserScan::save_xFig( char *filename ) const
{
  SmartLaserXfigClass drawing;
  std::string         name(filename);

  if (this->is_scan_valid()) {
    drawing.open(name);
    save_xFig( &drawing );
    drawing.close();
  }
}


////////////////////////////////////////////////////////////////////
// save_xFig (SmartLaserXfigClass drawing)
void Smart::SmartConvertibleMobileLaserScan::save_xFig( SmartLaserXfigClass *drawing ) const
{
  int                 x,y;
  double              angle;
  int                 status;
  int                 i;

  status = 0;

  // draw current scanner position
  if (status==0) status = drawing->drawPoint(this->get_scanner_x(), this->get_scanner_y(), RED,FAW_XFIG_CROSS);

  // draw current robot position
  if (status==0) status = drawing->drawPoint(this->get_base_state().get_base_position().get_x(),
                                            this->get_base_state().get_base_position().get_y(),
                                            GREEN,FAW_XFIG_CROSS);

  for ( i = 0; i < (int)(this->get_scan_size()); i++) {
    angle = this->get_scanner_azimuth() + this->get_scan_angle(i);  //data.scanPosA+data.scan.point[i].angle;
    x     = (int)(this->get_scanner_x() + this->get_scan_distance(i) *cos(angle));
    y     = (int)(this->get_scanner_y() + this->get_scan_distance(i) *sin(angle));
    if (status==0) status = drawing->drawPoint(x,y,BLACK,FAW_XFIG_POINT);
  }

}

/////////////////////////////////////////////
// PRIVATE

double Smart::SmartConvertibleMobileLaserScan::NormAngle(double a)
{
    if(a < -M_PI || a >= M_PI)
    {
        a = a - 2*M_PI * floor(a / (2*M_PI));
        while(a < -M_PI)
            a += 2*M_PI;
        while(a >= M_PI)
            a -= 2*M_PI;
    }
    return(a);
}


