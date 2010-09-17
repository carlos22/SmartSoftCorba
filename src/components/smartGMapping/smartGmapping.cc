// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Matthias Lutz
//
//        schlegel@hs-ulm.de
//        lutz@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
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



#include "smartSoft.hh"
#include "values.h"
#include <utils/gvalues.h>
#include <gridfastslam/gridslamprocessor.h>
#include <utils/orientedboundingbox.h>
#include <utils/commandline.h>
#include <sstream>
#include <string>
#include <fstream>	// for ofstream
#include "commMobileLaserScan.hh"
#include "commBasePositionUpdate.hh"


#define INI_PARAMETERS_FILE "smartGmapping.ini"


class PrintStateChangeHandler;

CHS::SmartComponent *component;

PrintStateChangeHandler *stateHandler;
CHS::SmartStateServer *state;

// -------------------------------------------------------------
//
// Parameters
//
// -------------------------------------------------------------

//particle parameters
int 	particles=30;

//number of scan beams used
int	numScanBeams=361;

//gfs parameters
double 	angularUpdate=0.5;
double 	linearUpdate=1;


//scan matching parameters
double  maxrange=8.;//changend from 80.
double 	maxUrange=8.;//changed from 80.
double	sigma=0.05;
double 	regscore=1e4;
int 	iterations=5;
double 	critscore=0.;
double 	maxMove=1.;


// default settings for a 0.1 m map cell
double 	lstep=.05;
double 	astep=.05;
double 	lsigma=.075;
int 	lskip=0;

int 	kernelSize=1;
double 	ogain=3;
double 	resampleThreshold=0.5;

//motion model parameters
double 	srr=0.1, srt=0.1, str=0.1, stt=0.1;

// inital map params
double	xmin=-100.;
double 	ymin=-100.;
double	xmax=100.;
double	ymax=100.;
double	delta=0.05;


bool 	generateMap=true;
std::string outfilename="GmappingOutFile";




/**
 * Read the parameters.
 * The parameters are read from an ini-file.
 */
void readParameters(int argc, char **argv)
{
  CHS::SmartParameter parameter;

  try {
    parameter.addFile(argc, argv, "ini");
  } catch(CHS::ParameterError e) {
    try {
      parameter.addFile(INI_PARAMETERS_FILE);
    } catch(CHS::ParameterError e) {
      std::cerr << "No parameter file found." << std::endl;
      return;
    }
  }

  ////////////////////
  // read parameters
  //
  parameter.getInt("gfs", "particles", particles );
  std::cout << "particles = " << particles << std::endl;
  
  parameter.getInt("gfs", "numScanBeams", numScanBeams );
  std::cout << "numScanBeams = " << numScanBeams << std::endl;
 
  parameter.getDouble("gfs","angularUpdate", angularUpdate);
  std::cout << "angularUpdate = " << angularUpdate << std::endl;

  parameter.getDouble("gfs","linearUpdate", linearUpdate);
  std::cout << "linearUpdate = " << linearUpdate << std::endl;

 
  parameter.getDouble("gfs","maxrange", maxrange);
  std::cout << "maxrange = " << maxrange << std::endl;

  parameter.getDouble("gfs","maxUrange", maxUrange);
  std::cout << "maxUrange = " << maxUrange << std::endl;

  parameter.getDouble("gfs","sigma", sigma);
  std::cout << "sigma = " << sigma << std::endl;

  parameter.getDouble("gfs","regscore", regscore);
  std::cout << "regscore = " << regscore << std::endl;

  parameter.getInt("gfs", "iterations", iterations );
  std::cout << "iterations = " << iterations << std::endl;

  parameter.getDouble("gfs","critscore", critscore);
  std::cout << "regscore = " << regscore << std::endl;

  parameter.getDouble("gfs","maxMove", maxMove);
  std::cout << "maxMove = " << maxMove << std::endl;


  parameter.getDouble("gfs","lstep", lstep);
  std::cout << "lstep = " << lstep << std::endl;

  parameter.getDouble("gfs","astep", astep);
  std::cout << "astep = " << astep << std::endl;

  parameter.getDouble("gfs","lsigma", lsigma);
  std::cout << "lsigma = " << lsigma << std::endl;

  parameter.getDouble("gfs","lskip", lskip);
  std::cout << "lskip = " << lskip << std::endl;


  parameter.getInt("gfs", "kernelSize", kernelSize );
  std::cout << "kernelSize = " << kernelSize << std::endl;

  parameter.getInt("gfs", "ogain", ogain );
  std::cout << "ogain = " << ogain << std::endl;

  parameter.getDouble("gfs","resampleThreshold", resampleThreshold);
  std::cout << "resampleThreshold = " << resampleThreshold << std::endl;


  parameter.getDouble("gfs","srr", srr);
  std::cout << "srr = " << srr << std::endl;

  parameter.getDouble("gfs","srt", srt);
  std::cout << "srt = " << srt << std::endl;

  parameter.getDouble("gfs","str", str);
  std::cout << "str = " << str << std::endl;

  parameter.getDouble("gfs","stt", stt);
  std::cout << "stt = " << stt << std::endl;


  parameter.getDouble("gfs","xmin", xmin);
  std::cout << "xmin = " << xmin << std::endl;

  parameter.getDouble("gfs","ymin", ymin);
  std::cout << "ymin = " << ymin << std::endl;

  parameter.getDouble("gfs","xmax", xmax);
  std::cout << "xmax = " << xmax << std::endl;

  parameter.getDouble("gfs","ymax", ymax);
  std::cout << "ymax = " << ymax << std::endl;


  parameter.getString("gfs", "outfilename", outfilename);
  std::cout << "outfilename = " << outfilename << std::endl;
  
  parameter.getTruthValue("gfs", "generateMap", generateMap);
  std::cout << "generateMap = " << generateMap << std::endl;

}


double pi_to_pi(double angle){
  
   angle+=M_PI;
   double ret_angle = fmod(angle,2*M_PI);

   if(angle<0)
     ret_angle+=2*M_PI;

   ret_angle-=M_PI;

   return ret_angle;

}



// -------------------------------------------------------------
// derive the state class and provide handler functions
// -------------------------------------------------------------
class PrintStateChangeHandler : public CHS::StateChangeHandler
{
public:
  void handleEnterState(const std::string & s) throw()
    {
      std::cout << "    enterHandler  activate   " << s << std::endl;
    };
  void handleQuitState(const std::string & s) throw()
    {
      std::cout << "    quitHandler   deactivate " << s << std::endl;
    };
};


using namespace GMapping;

class GmappingThread : public CHS::SmartTask
{
public:
  GmappingThread() {};
  ~GmappingThread() {};

  int svc(void);

private:

};

int GmappingThread::svc(void)
{

  GridSlamProcessor* processor=new GridSlamProcessor;
  
  CHS::PushNewestClient<Smart::CommMobileLaserScan> *laserClient;
  laserClient = new CHS::PushNewestClient<Smart::CommMobileLaserScan>(component);

  CHS::SendClient<Smart::CommBasePositionUpdate> smartBasePositionUpdateSendClient(component,"smartPioneerBaseServer","positionUpdate");


  Smart::CommMobileLaserScan laserscan;
  CHS::StatusCode status;

  // connect laserClient
  status = laserClient->connect("smartLaserServer","scan");
  laserClient->subscribe();

  if(status!=CHS::SMART_OK)
  {
     std::cerr << "ERROR: failed to connect to laser scan service "<< std::endl;
     std::cerr << "ERROR: (" << CHS::StatusCodeConversion(status) << ")" << std::endl;
     return -1;
  }

  //get first laser scan
  laserClient->getUpdateWait(laserscan);


  //////////////////////////////////////////////////////////////////////////////////////////
  //SENSOR MAP
  //loads from the carmen wrapper the laser and robot settings
  SensorMap sensorMap;
  RangeSensor* m_frontLaser=0;

  double maximum_range = 8;

  std::string sensorName="FLASER";
  OrientedPoint rpose(0, 0, 0);
  OrientedPoint lpose(0, 0, 0);
  OrientedPoint dp=absoluteDifference(lpose, rpose);

  if(numScanBeams == 361)
  {
     m_frontLaser=new RangeSensor(sensorName, numScanBeams , laserscan.get_scan_resolution(), OrientedPoint(laserscan.get_scanner_x(1),laserscan.get_scanner_y(1),laserscan.get_scanner_azimuth()), 0, maximum_range);
  }
  else if(numScanBeams == 181)
  {
     //0.017453 is the intended scan resolution (1 degree in rad)
     m_frontLaser=new RangeSensor(sensorName, numScanBeams ,0.017453 , OrientedPoint(laserscan.get_scanner_x(1),laserscan.get_scanner_y(1),laserscan.get_scanner_azimuth()), 0, maximum_range);
  }
  else
  {
     std::cerr<<"Invalid number of scan beams configured! Change value in ini file!"<<std::endl;
     return -1;
  }
  m_frontLaser->updateBeamsLookup();
  sensorMap.insert(make_pair(sensorName, m_frontLaser));

  processor->setSensorMap(sensorMap);
  /////////////////////////////////////////////////////////////////////////////////////////


  //set the command line parameters
  processor->setMatchingParameters(maxUrange, maxrange, sigma, kernelSize, lstep, astep, iterations, lsigma, ogain, lskip);
  processor->setMotionModelParameters(srr, srt, str, stt);
  processor->setUpdateDistances(linearUpdate, angularUpdate, resampleThreshold);
  processor->setgenerateMap(generateMap);
  //OrientedPoint initialPose(xmin+xmax/2, ymin+ymax/2, 0);
  OrientedPoint initialPose(0,0,0);


  //INITIALIZATION
  processor->init(particles, xmin, ymin, xmax, ymax, delta, initialPose);
  if (outfilename.length()>0)
  	processor->outputStream().open(outfilename.c_str());

  bool running=true;

  GridSlamProcessor* ap, *copy=processor->clone();
  ap=processor; processor=copy; copy=ap;


  RangeReading rr(0,0);
  double intpart,fractpart;


	while (running){
	   // ----------------------------------------------------------
   	   // wait for activation
   	   // ("active")
  	   // ----------------------------------------------------------
	   status = state->acquire("active");
   	   if (status == CHS::SMART_OK) {
	      while (CHS::SMART_OK == laserClient->getUpdateWait(laserscan)){

		const RangeSensor * rs= m_frontLaser;
        	RangeReading reading(rs, 0);
	        reading.resize(numScanBeams);

		if(numScanBeams==361){
		   for (unsigned int i=0; i< (unsigned int)laserscan.get_scan_size(); i++){
		   	   double angle=pi_to_pi((double)laserscan.get_scan_angle(i));
			   //0.00872664
			   int index =(int) -1.0*((-1.5708-angle)/laserscan.get_scan_resolution());
                           //cout<<"index: "<<i<<" "<<index<<endl;
                           reading[index]=(double)laserscan.get_scan_distance(i,1);
                   }
                }
                else if(numScanBeams==181){
 		   for (unsigned int i=0; i< (unsigned int)laserscan.get_scan_size(); i++){
		   	   double angle=pi_to_pi((double)laserscan.get_scan_angle(i));
			   fractpart = modf((angle/M_PI*180),&intpart);
                           if((fractpart>0.4 && fractpart<0.6) || (fractpart>-0.6 && fractpart<-0.4))
                           {
                              continue;
                           }
			   //0.017453
			   int index =(int) -1.0*((-1.5708-angle)/0.017453);
                           //cout<<"index: "<<i<<" "<<index<<endl;
                           reading[index]=(double)laserscan.get_scan_distance(i,1);
                   }
                }

		//insert odometry data in the reading
		//raw odometry data from within the laser scan com object is used 
        	reading.setPose(OrientedPoint(laserscan.get_base_state().get_base_raw_position().get_x(1), 
			laserscan.get_base_state().get_base_raw_position().get_y(1), 
			pi_to_pi(laserscan.get_base_state().get_base_raw_position().get_base_alpha())));
        	

		bool processed=processor->processScan(reading);

			//this returns true when the algorithm effectively processes (the traveled path since the last processing is over a given threshold)
			if (processed){
		cout<<"SLAM odom pose: "<<laserscan.get_base_state().get_base_raw_position().get_x(1)<<" "<<laserscan.get_base_state().get_base_raw_position().get_y(1)<<" "<<pi_to_pi(laserscan.get_base_state().get_base_raw_position().get_base_alpha())<<endl;
				cerr << "PROCESSED" << endl;
				//for searching for the BEST PARTICLE INDEX
				unsigned int best_idx=processor->getBestParticleIndex();

				const GridSlamProcessor::ParticleVector& particles = processor->getParticles();

				//this is for recovering the tree of PARTICLE TRAJECTORIES (obtaining the ancestor of each particle)
				cerr << "Particle reproduction story begin" << endl;
				for (unsigned int i=0; i<particles.size(); i++){
					cerr << particles[i].previousIndex << "->"  << i << " ";
				}
				cerr << "Particle reproduction story end" << endl;



				OrientedPoint pose = processor->getParticles()[best_idx].pose;
				cerr<<"Best Pose is pose x:"<<pose.x<<" y:"<<pose.y<<" alpha:"<<pose.theta<<std::endl;

				double best_weight=particles[best_idx].weightSum;
				cerr << "Best Particle is " << best_idx << " with weight " << best_weight << endl;

  				Smart::CommBasePositionUpdate upd;
				Smart::CommBasePosition old_pos;
				Smart::CommBasePosition upd_pos;

				//No Cov to set!
				old_pos.set_cov(0, 0, laserscan.get_base_state().get_base_position().get_cov(0,0));
			        old_pos.set_cov(1, 1, laserscan.get_base_state().get_base_position().get_cov(1,1));
          			old_pos.set_cov(2, 2, laserscan.get_base_state().get_base_position().get_cov(2,2));
				upd_pos.set_cov(0, 0, laserscan.get_base_state().get_base_position().get_cov(0,0));
			        upd_pos.set_cov(1, 1, laserscan.get_base_state().get_base_position().get_cov(1,1));
          			upd_pos.set_cov(2, 2, laserscan.get_base_state().get_base_position().get_cov(2,2));
				
				cout<<"SLAM: pos update: old pos: "<<laserscan.get_base_state().get_base_position().get_x(1)<<" "<<laserscan.get_base_state().get_base_position().get_y(1)<<" "<<laserscan.get_base_state().get_base_position().get_base_alpha()<<endl;
				old_pos.set_x (laserscan.get_base_state().get_base_position().get_x(1), 1);
			        old_pos.set_y (laserscan.get_base_state().get_base_position().get_y(1), 1);
			        old_pos.set_z (laserscan.get_base_state().get_base_position().get_z(1), 1);
          			old_pos.set_base_alpha (laserscan.get_base_state().get_base_position().get_base_alpha());

          			upd_pos.set_x (pose.x, 1);
          			upd_pos.set_y (pose.y, 1);
			        upd_pos.set_z (laserscan.get_base_state().get_base_position().get_z(1), 1);
			        upd_pos.set_base_alpha (pose.theta);
                                upd_pos.set_cov_invalid(true);

			        upd.set_corrected_position (upd_pos);
			        upd.set_old_position (old_pos);
			        smartBasePositionUpdateSendClient.send(upd);
 				std::cout<<"Postion update send!"<<std::endl;


/* 				{
				cout<<"m_count: "<<processor->m_count<<endl;
				std::string sFileExtension = ".mat";
				std::stringstream sFileName;
				sFileName << "/home/zafh/SOFTWARE/smartsoft/src/components/smartGMapping/experimentData/" << "estimatedVehiclePos";
				sFileName << setfill('0') << setw(4) << processor->m_count << sFileExtension;

				ofstream fileStream((sFileName.str()).c_str(), ios::trunc);	// open file for output
				if(fileStream) // file could be opened
				{
					cout<<"File stream opendend!"<<endl;
					fileStream<<std::scientific<<std::setprecision(7);
					fileStream <<setw(16)<< pose.x<<endl;
					fileStream <<setw(16)<< pose.y<<endl;
					fileStream <<setw(16)<< pose.theta<<endl;
				}
				else{
				cout<<"WARNING: could not open Debug filestream!"<<endl;
				}
				fileStream.close();	// close the File
				}
*/				
				cerr << __PRETTY_FUNCTION__  << "CLONING... " << endl;
				GridSlamProcessor* newProcessor=processor->clone();
				cerr << "DONE" << endl;
				cerr << __PRETTY_FUNCTION__  << "DELETING... " << endl;
				delete processor;
				cerr << "DONE" << endl;
				processor=newProcessor;
			} //if processed
		} //while getUpdateWait(laserscan)
	   }//if
	   // -----------------------------------------------------
	   // unlock state at end of while loop
	   // -----------------------------------------------------
	   status = state->release("active");
	} //while running

  return 0;
}


int main (int argc, char *argv[])
{
  try
  {
    CHS::SmartThreadManager *threadManager = CHS::SmartThreadManager::instance();

    component = new CHS::SmartComponent("smartGmapping",argc,argv);

    readParameters(argc,argv);


    // state StateServer
    stateHandler = new PrintStateChangeHandler();
    state = new CHS::SmartStateServer(component,*stateHandler);

    if (state->defineStates("active" ,"active") != CHS::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->activate() != CHS::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;


    GmappingThread gmappingThread;
    gmappingThread.open();

    component->run();
    threadManager->wait();
  }
  catch  (const CORBA::Exception &)
  {
    std::cerr << "Uncaught CORBA exception" << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

