//------------------------------------------------------------------------
//
//  Copyright (C) 2011 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of "CommKinectServer".
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
//--------------------------------------------------------------------------

#include "KinectWrapperMRPT.hh"
#include "gen/SmartKinectServer.hh"

KinectWrapper::KinectWrapper()
{

	cout << "Calling CKinect::initialize()...";
//	kinect.enablePreviewRGB(true);
	kinect.initialize();
	cout << "OK\n";

	imageCounter = 0;

	// create matrixes
	colorIntrinsic.set_size(4, 4);
	colorIntrinsic.zeros();

	distIntrinsic.set_size(4, 4);
	distIntrinsic.zeros();

	distIntrinsic(0, 0) = 5.9421434211923247e+02;
	distIntrinsic(1, 1) = 5.9104053696870778e+02;
	distIntrinsic(0, 2) = 3.3930780975300314e+02;
	distIntrinsic(1, 2) = 2.4273913761751615e+02;
	distIntrinsic(2, 2) = 1;
	distIntrinsic(3, 3) = 1;

	extrinsicMat.set_size(4, 4);
	extrinsicMat.zeros();
	//	extrinsicMat << 5.34866271e+02 << 3.89654806e+00 << 0.00000000e+00 << 1.74704200e-02 << arma::endr;
	//	extrinsicMat << -4.70724694e+00 << -5.28843603e+02 << 0.00000000e+00 << -1.22753400e-02 << arma::endr;
	//	extrinsicMat << -3.19670762e+02 << -2.60999685e+02 << 0.00000000e+00 << -9.99772000e-01 << arma::endr;
	//	extrinsicMat << -6.98445586e+00 << 3.31139785e+00 << 0.00000000e+00 << 1.09167360e-02 << arma::endr;
}

KinectWrapper::~KinectWrapper()
{
}

void KinectWrapper::getImage(CommVisionObjects::CommKinectImage& image)
{

	bool there_is_obs = true, hard_error = false;
	// Grab new observation from the camera:
	CObservation3DRangeScanPtr obs = CObservation3DRangeScan::Create(); // Smart pointers to observations
	do
	{

		kinect.getNextObservation(*obs, there_is_obs, hard_error);
		if (!there_is_obs)
		{
			std::cout << "There is no new observation!" << endl;
		}

	} while (!there_is_obs);

	// fill ranges as 2D:
	if (obs->hasRangeImage)
	{
		// Normalize the image
		//static CMatrixFloat  range2D;   // Static to save time allocating the matrix in every iteration
		CMatrixFloat range2D; // Static to save time allocating the matrix in every iteration
		//range2D = obs->rangeImage * (1.0/ 20.0); //kinect.getMaxRange());
		range2D = obs->rangeImage;

		std::cout << "[KinectWrapperMRPT]:range2D size: col: " << range2D.getColCount() << " row: " << range2D.getRowCount() << std::endl;
		std::vector<float> depth(range2D.getColCount() * range2D.getRowCount());

		for (unsigned int x = 0; x < range2D.getColCount(); x++)
		{
			for (unsigned int y = 0; y < range2D.getRowCount(); y++)
			{
				//std::cout<<__LINE__<<"x: "<<x<<"y: "<<y<<std::endl;
				depth[y * range2D.getColCount() + x] = range2D(y, x); //access to matrix in y, x direction!
				//cout<<"range2D(y,x) "<<range2D(y,x)<<endl;
				//std::cout<<__LINE__<<std::endl;
			}
		}
		image.set_distances(depth, range2D.getColCount(), range2D.getRowCount());
	}
	else
	{
		std::cout<<"The obs has no RangeImage!"<<std::endl;
	}
	// Show intensity image:

	if (obs->hasIntensityImage)
	{

		CImage img = obs->intensityImage;
		img.swapRB();

		// undistort image
		// todo move this somewhere global as undistort_map already is.
		// setup undistortion:
		TCamera cam_params = kinect.getCameraParamsIntensity();
		cam_params.cx(COMP->ini.hardware_properties.colorIntrinsicCx);
		cam_params.cy(COMP->ini.hardware_properties.colorIntrinsicCy);
		cam_params.fx(COMP->ini.hardware_properties.colorIntrinsicFx);
		cam_params.fy(COMP->ini.hardware_properties.colorIntrinsicFy);

		// distortion = [K1 K2 T1 T2 K3]
		cam_params.k1(COMP->ini.hardware_properties.colorDistortionK1);
		cam_params.k2(COMP->ini.hardware_properties.colorDistortionK2);
		cam_params.p1(COMP->ini.hardware_properties.colorDistortionP1);
		cam_params.p2(COMP->ini.hardware_properties.colorDistortionP2);
		cam_params.k3(COMP->ini.hardware_properties.colorDistortionK3);

		undistort_map.setFromCamParams(cam_params);
		undistort_map.undistort(img);  // output in place


		std::cout << "colorImg size: " << img.getWidth() << " " << img.getHeight() << std::endl;
		std::vector<uint8_t> colorImg(img.getWidth() * img.getHeight() * 3);

		//copy rgb color image 1 to 1
		for (unsigned int x = 0; x < img.getWidth(); x++)
		{
			for (unsigned int y = 0; y < img.getHeight(); y++)
			{
				//std::cout<<__LINE__<<"x: "<<x<<"y: "<<y<<std::endl;

				colorImg[3* y * img.getWidth() + 3* x ] = *img(x, y, 0);//access to matrix in y, x direction!
				colorImg[3* y * img.getWidth() + 3* x + 1] = *img(x, y, 1);
				colorImg[3* y * img.getWidth() + 3* x + 2] = *img(x, y, 2);
				//cout<<"range2D(y,x) "<<range2D(y,x)<<endl;
				//std::cout<<__LINE__<<std::endl;
			}
		}

		image.set_rgb_image(colorImg, img.getWidth(), img.getHeight());

		//TCamera cameraParamsIntensity =	kinect.getCameraParamsIntensity();
		TCamera cameraParamsDepth = kinect.getCameraParamsDepth();

		arma::mat colorIntrinsic;
		arma::mat distIntrinsic;
		arma::mat extrinsicMat;

		// create matrixes
		colorIntrinsic.set_size(4, 4);
		colorIntrinsic.zeros();
		colorIntrinsic(0, 0) = COMP->ini.hardware_properties.colorIntrinsicFx;
		colorIntrinsic(1, 1) = COMP->ini.hardware_properties.colorIntrinsicFy;
		colorIntrinsic(0, 2) = COMP->ini.hardware_properties.colorIntrinsicCx;
		colorIntrinsic(1, 2) = COMP->ini.hardware_properties.colorIntrinsicCy;
		colorIntrinsic(2, 2) = 1;
		colorIntrinsic(3, 3) = 1;

		distIntrinsic.set_size(4, 4);
		distIntrinsic.zeros();

		distIntrinsic(0, 0) = cameraParamsDepth.fx();
		distIntrinsic(1, 1) = cameraParamsDepth.fy();
		distIntrinsic(0, 2) = cameraParamsDepth.cx();
		distIntrinsic(1, 2) = cameraParamsDepth.cy();
		distIntrinsic(2, 2) = 1;
		distIntrinsic(3, 3) = 1;

		extrinsicMat.set_size(4, 4);
		extrinsicMat.zeros();

		extrinsicMat(0, 3) = 0.025;
		extrinsicMat(0, 0) = 1;
		extrinsicMat(1, 1) = 1;
		extrinsicMat(2, 2) = 1;
		extrinsicMat(3, 3) = 1;

		//		CMatrixDouble44 exMat = kinect.getRelativePoseIntensityWrtDepth().getHomogeneousMatrixVal();
		//		extrinsicMat.set_size(4, 4);
		//		for (int i = 0; i < 4; i++)
		//		{
		//			for (int j = 0; j < 4; j++)
		//			{
		//				extrinsicMat(i, j) = exMat(i, j);
		//			}
		//		}

		image.set_color_intrinsic(colorIntrinsic);
		image.set_distance_intrinsic(distIntrinsic);
		image.set_camera_extrinsic(extrinsicMat);
		image.set_sequence_counter(imageCounter++);
		image.set_data_valid(true);

	}
	else
	{
		std::cout<<"The obs has no IntensityImage!"<<std::endl;
	}

	if (obs->hasIntensityImage && obs->hasRangeImage)
	{
		cout << "is_data_valid true " << endl;
		image.set_sequence_counter(imageCounter++);
		image.set_data_valid(true);
	} else
	{
		cout << "is_data_valid false" << endl;
		image.set_sequence_counter(imageCounter++);
		image.set_data_valid(false);
	}

}

///////////////////////////////////////////////////
// Reprojection of depth points to color image --> for rgb color image correction
///////////////////////////////////////////////////
/*
 const int sizeRangeScan = obs->points3D_x.size();
 const float cx = obs->cameraParamsIntensity.cx();
 const float cy = obs->cameraParamsIntensity.cy();
 const float fx = obs->cameraParamsIntensity.fx();
 const float fy = obs->cameraParamsIntensity.fy();
 unsigned int img_idx_x = 0, img_idx_y = 0;
 size_t imgW=0, imgH=0;
 bool hasValidIntensityImage = false;

 if (obs->hasIntensityImage)
 {
 // assure the size matches?
 if (sizeRangeScan == obs->intensityImage.getWidth() * obs->intensityImage.getHeight() )
 {
 hasValidIntensityImage = true;
 imgW = obs->intensityImage.getWidth();
 imgH = obs->intensityImage.getHeight();
 }
 }



 const bool hasColorIntensityImg = hasValidIntensityImage && obs->intensityImage.isColor();


 std::cout<<"RangeScanSize: "<<sizeRangeScan<<endl;

 for (size_t i=0;i<sizeRangeScan;i++)
 {
 // Valid point?
 if ( obs->points3D_x[i]!=0 && obs->points3D_y[i]!=0 )
 {

 const float scan_x = obs->points3D_x[i];
 const float scan_y = obs->points3D_y[i];
 const float scan_z = obs->points3D_z[i];
 imgW = obs->intensityImage.getWidth();
 imgH = obs->intensityImage.getHeight();
 float	pR=1,pG=1,pB=1;
 const float K_8u = 1.0f/255;

 // Do we have to project the 3D point into the image plane??
 bool hasValidColor = false;


 TPoint3D  pt; // pt_wrt_colorcam;
 obs->relativePoseIntensityWRTDepth.inverseComposePoint(
 scan_x,scan_y,scan_z,
 pt.x,pt.y,pt.z);

 // Project to image plane:
 if (pt.z)
 {
 img_idx_x = cx + fx * pt.x/pt.z;
 img_idx_y = cy + fy * pt.y/pt.z;

 hasValidColor=
 img_idx_x>=0 && img_idx_x<imgW &&
 img_idx_y>=0 && img_idx_y<imgH;
 }


 if (hasValidColor && hasColorIntensityImg)
 {
 const uint8_t *c= obs->intensityImage.get_unsafe(img_idx_x, img_idx_y, 0);
 pR= c[2] * K_8u;
 pG= c[1] * K_8u;
 pB= c[0] * K_8u;

 colorImg[i*3+0]= pR*255;//access to matrix in y, x direction!
 colorImg[i*3+1]= pG*255;
 colorImg[i*3+2]= pB*255;
 }
 }
 }
 image->set_rgb_image(colorImg);
 */
