//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------


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
// --------------------------------------------------------------------------


#include "GMappingTask.hh"
#include "gen/SmartGMapping.hh"
#include <CommBasicObjects/commMobileLaserScan.hh>
#include <CommBasicObjects/commBasePosition.hh>
#include <CommBasicObjects/commBasePositionUpdate.hh>

#include <iostream>

GMappingTask::GMappingTask()
{
	 COMP->processor=new GMapping::GridSlamProcessor;
	 COMP->mymap = NULL;
}

int GMappingTask::svc()
{

	CommBasicObjects::CommMobileLaserScan laserscan;
     CHS::StatusCode status;

	 //get first laser scan
	 COMP->laserClient->getUpdateWait(laserscan);

	  //////////////////////////////////////////////////////////////////////////////////////////
	   //SENSOR MAP
	   //loads from the carmen wrapper the laser and robot settings
	  GMapping::SensorMap sensorMap;
	  GMapping::RangeSensor* m_frontLaser=0;

	  //TODO remove used ini instead
	   double maximum_range = 8;

	   std::string sensorName="FLASER";
	   GMapping::OrientedPoint rpose(0, 0, 0);
	   GMapping::OrientedPoint lpose(0, 0, 0);
	   GMapping::OrientedPoint dp=GMapping::absoluteDifference(lpose, rpose);

	   if(COMP->ini.gfs.numScanBeams == 361)
	   {
	 	//TODO ändern Robot offset
	      m_frontLaser=new GMapping::RangeSensor(sensorName, COMP->ini.gfs.numScanBeams , laserscan.get_scan_resolution(), GMapping::OrientedPoint(laserscan.get_scanner_x(1),laserscan.get_scanner_y(1),laserscan.get_scanner_azimuth()), 0, maximum_range);
	   }
	   else if(COMP->ini.gfs.numScanBeams == 181)
	   {
	      //0.017453 is the intended scan resolution (1 degree in rad)
	      m_frontLaser=new GMapping::RangeSensor(sensorName, COMP->ini.gfs.numScanBeams ,0.017453 , GMapping::OrientedPoint(laserscan.get_scanner_x(1),laserscan.get_scanner_y(1),laserscan.get_scanner_azimuth()), 0, maximum_range);
	   }
	   else
	   {
	      std::cerr<<"Invalid number of scan beams configured! Change value in ini file!"<<std::endl;
	      return -1;
	   }
	   m_frontLaser->updateBeamsLookup();
	   sensorMap.insert(make_pair(sensorName, m_frontLaser));

	   COMP->processor->setSensorMap(sensorMap);
	   /////////////////////////////////////////////////////////////////////////////////////////


	   //set the command line parameters
	   COMP->processor->setMatchingParameters(COMP->ini.gfs.maxUrange, COMP->ini.gfs.maxrange, COMP->ini.gfs.sigma, COMP->ini.gfs.kernelSize, COMP->ini.gfs.lstep, COMP->ini.gfs.astep, COMP->ini.gfs.iterations, COMP->ini.gfs.lsigma, COMP->ini.gfs.ogain, COMP->ini.gfs.lskip);
	   COMP->processor->setMotionModelParameters(COMP->ini.gfs.srr, COMP->ini.gfs.srt, COMP->ini.gfs.str, COMP->ini.gfs.stt);
	   COMP->processor->setUpdateDistances(COMP->ini.gfs.linearUpdate, COMP->ini.gfs.angularUpdate, COMP->ini.gfs.resampleThreshold);
	   COMP->processor->setgenerateMap(COMP->ini.gfs.generateMap);
	   //OrientedPoint initialPose(xmin+xmax/2, ymin+ymax/2, 0);
	   GMapping::OrientedPoint initialPose(0,0,0);


	   //INITIALIZATION
	   //TODO RICHTIGTE INIT POSE eintragen
	   COMP->processor->init(COMP->ini.gfs.particles, COMP->ini.gfs.xmin, COMP->ini.gfs.ymin, COMP->ini.gfs.xmax, COMP->ini.gfs.ymax, COMP->ini.gfs.delta, initialPose);

	   bool running=true;

	   GMapping::GridSlamProcessor* ap, *copy=COMP->processor->clone();
	   ap=COMP->processor; COMP->processor=copy; copy=ap;


	   GMapping::RangeReading rr(0,0);
	   double intpart,fractpart;


	 	while (running){
	 	   // ----------------------------------------------------------
	    	   // wait for activation
	    	   // ("active")
	   	   // ----------------------------------------------------------
	 	   status = COMP->stateServer->acquire("active");
	    	   if (status == CHS::SMART_OK) {
	 	        if(CHS::SMART_OK == COMP->laserClient->getUpdateWait(laserscan)){

	 		const GMapping::RangeSensor * rs= m_frontLaser;
	         	GMapping::RangeReading reading(rs, 0);
	 	        reading.resize(COMP->ini.gfs.numScanBeams);

	 		if(COMP->ini.gfs.numScanBeams==361){
	 		   for (unsigned int i=0; i< (unsigned int)laserscan.get_scan_size(); i++){
	 		   	   double angle=COMP->pi_to_pi((double)laserscan.get_scan_angle(i));
	 			   int index =(int) -1.0*((-1.5708-angle)/laserscan.get_scan_resolution());
	                            //cout<<"index: "<<i<<" "<<index<<endl;
	                            reading[index]=(double)laserscan.get_scan_distance(i,1);
	                    }
	                 }
	                 else if(COMP->ini.gfs.numScanBeams==181){
	  		   for (unsigned int i=0; i< (unsigned int)laserscan.get_scan_size(); i++){
	 		   	   double angle=COMP->pi_to_pi((double)laserscan.get_scan_angle(i));
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
	         	reading.setPose(GMapping::OrientedPoint(laserscan.get_base_state().get_base_raw_position().get_x(1),
	 			laserscan.get_base_state().get_base_raw_position().get_y(1),
	 			COMP->pi_to_pi(laserscan.get_base_state().get_base_raw_position().get_base_alpha())));


	 		bool processed=COMP->processor->processScan(reading);

	 			//this returns true when the algorithm effectively processes (the traveled path since the last processing is over a given threshold)
	 			if (processed){


	 				cout<<"SLAM odom pose: "<<laserscan.get_base_state().get_base_raw_position().get_x(1)<<" "<<laserscan.get_base_state().get_base_raw_position().get_y(1)<<" "<<COMP->pi_to_pi(laserscan.get_base_state().get_base_raw_position().get_base_alpha())<<endl;
	 				cerr << "PROCESSED" << endl;
	 				//for searching for the BEST PARTICLE INDEX
	 				unsigned int best_idx=COMP->processor->getBestParticleIndex();

	 				const GMapping::GridSlamProcessor::ParticleVector& particles = COMP->processor->getParticles();

	 				//this is for recovering the tree of PARTICLE TRAJECTORIES (obtaining the ancestor of each particle)
	 				cerr << "Particle reproduction story begin" << endl;
	 				for (unsigned int i=0; i<particles.size(); i++){
	 					cerr << particles[i].previousIndex << "->"  << i << " ";
	 				}
	 				cerr << "Particle reproduction story end" << endl;



	 				GMapping::OrientedPoint pose = COMP->processor->getParticles()[best_idx].pose;
	 				cerr<<"Best Pose is pose x:"<<pose.x<<" y:"<<pose.y<<" alpha:"<<pose.theta<<std::endl;

	 				double best_weight=particles[best_idx].weightSum;
	 				cerr << "Best Particle is " << best_idx << " with weight " << best_weight << endl;

	 				CommBasicObjects::CommBasePositionUpdate upd;
	 				CommBasicObjects::CommBasePosition old_pos;
	 				CommBasicObjects::CommBasePosition upd_pos;

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
	 			        COMP->basePositionUpdateClient->send(upd);
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


	 				//new Map data avialable Lock map
	 				std::cout<<"main mapLock.acquire();"<<std::endl;
	 				COMP->mapLock.acquire();
	 				std::cout<<"PAST main mapLock.acquire();"<<std::endl;

	 				if(COMP->mymap!=NULL)
	 				{
	 					delete COMP->mymap;
	 				}
	 				//then if you want to access the BEST MAP,
	 				//of course by copying it in a plain structure
	 				//Map<double, DoubleArray2D, false>* mymap = processor->getParticles()[best_idx].map.toDoubleMap();
	 				COMP->mymap = COMP->processor->getParticles()[best_idx].map.toDoubleMap();
	 				//at this point mymap is yours. Can do what you want.

	 				//exporting the map finshed data accessable
	 				COMP->mapLock.release();




	 				cerr << __PRETTY_FUNCTION__  << "CLONING... " << endl;
	 				GMapping::GridSlamProcessor* newProcessor=COMP->processor->clone();
	 				cerr << "DONE" << endl;
	 				cerr << __PRETTY_FUNCTION__  << "DELETING... " << endl;
	 				delete COMP->processor;
	 				cerr << "DONE" << endl;
	 				COMP->processor=newProcessor;

	 			} //if processed
	 		} //if getUpdateWait(laserscan)
	 	   }//if
	 	   // -----------------------------------------------------
	 	   // unlock state at end of while loop
	 	   // -----------------------------------------------------
	 	   status = COMP->stateServer->release("active");
	 	} //while running




	return 0;
}
