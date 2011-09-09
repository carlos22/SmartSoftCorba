//------------------------------------------------------------------------
//
//  Copyright (C) 2010 Siegfried Hochdorfer, Manuel Wopfner
//
//        hochdorfer@hs-ulm.de
//        wopfner@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file contains functions which are usefull for working with OpenCV
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


#ifndef OPENCV_HELPERS
#define OPENCV_HELPERS

#include <cv.h>
#include <iostream>

class OpenCVHelpers {
public:

	/**
	 * create grayscale image from float.
	 * returns IplImage that needs to be cvReleaseImage'ed by the caller.
	 */
	static IplImage* copyFloatToIplImage( const float* const floatImage, const int height, const int width) {
	    CvMat dataMatrix;

	    // const cast used here because we only use it to create the ipl image and do not make further use of the matrix
	    cvInitMatHeader(&dataMatrix, height, width, CV_32FC1, const_cast<float *>(floatImage));

	    IplImage* ipl_image = cvCreateImage(cvSize(height, width), IPL_DEPTH_32F, 1);
	    cvCopy(&dataMatrix, ipl_image);
	    return ipl_image;
    }



/**
convert a rgb image to an ipl image. use for example with kinect.get_rgb_image()
returns IplImage that needs to be cvReleaseImage'ed by the caller.
*/
static IplImage* copyRGBToIplImage( unsigned char* arr_image, const int height, const int width) {

		CvMat dataMatrix;
		IplImage *returnImage;
		cvInitMatHeader(&dataMatrix, height, width, CV_8UC3, arr_image);
		returnImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3); //IPL_DEPTH_32F

		// copy matrix data into image
		cvCopy(&dataMatrix, returnImage);

		// swap RGB/BGR as opencv internally stores as BGR
		cvCvtColor(returnImage, returnImage, CV_RGB2BGR);

		return returnImage;
}



	static IplImage* stretchContrast32f(IplImage * src, const double quantile = 0.01, const int histBins = 2048) {
        double min_val = 0.0;
        double max_val = 0.0;
        CvPoint* min_loc = NULL;
        CvPoint* max_loc = NULL;
        cvMinMaxLoc(src, &min_val, &max_val, min_loc, max_loc);

        // histogram variables
        CvHistogram *hist;
        int hist_size = histBins;
        float range_0[] = { min_val, max_val };
        float* ranges[] = { range_0 };

        int index;
        float hist_value;
        const int channels = 1;
        IplImage * GRAY = cvCreateImage(cvGetSize(src), 8, channels);

        hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
        cvCalcHist(&src, hist, 0, NULL);

        // remove upper and lower n-percent quantile for noise reduction purposes
        double sum = 0.0;
        int lowerIndex = 0;
        const int maxSumHistValues = src->width * src->height;

        // remove lower n-percent quantil
        for (index = 0; index < hist_size; index++) {
	        hist_value = cvQueryHistValue_1D(hist, index);
	        sum += hist_value;
	        if (sum >= maxSumHistValues * quantile) {
		        lowerIndex = index;
		        break;
	        }
	        float* histEntry = cvGetHistValue_1D(hist, index);
	        *histEntry = 0.0; // set histogram entry to 0.0
        }

        // remove upper n-percent quantile
        int upperIndex = hist_size - 1; // initialize with highest index
        sum = 0;
        for (index = hist_size - 1; index >= 0; index--) {
	        hist_value = cvQueryHistValue_1D(hist, index);
	        sum += hist_value;
	        if (sum >= maxSumHistValues * quantile) {
		        upperIndex = index;
		        break;
	        }
	        float* histEntry = cvGetHistValue_1D(hist, index);
	        *histEntry = 0.0; // set histogram entry to 0.0
        }

        cvNormalizeHist(hist, 1.0); // normalize to 1.0 to get the probability density function

        // create new histogram to store the cumulative probability density	function of the histogram hist
        CvHistogram* cumulativeHist = NULL;
        cvCopyHist(hist, &cumulativeHist);

        // calculate the cumulative probability density function of the	histogram hist
        float cumulativeSum = 0.0;
        for (index = 0; index < hist_size; index++) {
	        hist_value = cvQueryHistValue_1D(hist, index);
	        float* histEntry = cvGetHistValue_1D(cumulativeHist, index);
	        cumulativeSum += hist_value; // cumulative sum of all histogram	entries
            *histEntry += cumulativeSum; // store cumulative sum in the histogram cumulativeHist
        }

        // transform the src image to GRAY image using the cdf. Before adding the new value to the GRAY image, the values are normalize to 255 (UINT_8)
        double h = 0.0;
        float pixelValue = 0.0;
        int cdfIndex = 0;
        for (int y = 0; y < src->height; y++) {
	        float* srcPtr = (float*) (src->imageData + y * src->widthStep);
	        uchar* dstPtr = (uchar*) (GRAY->imageData + y * GRAY->widthStep);
	        for (int x = 0; x < src->width; x++) {
		        pixelValue = srcPtr[x];
		        // ((src pixel value - min value in src image) / (max_value in src image - min value in src image)) * number of histogram bins)
		        cdfIndex = cvRound(((pixelValue - min_val) / (max_val - min_val)) * (histBins - 1));
		        hist_value = cvQueryHistValue_1D(cumulativeHist, cdfIndex);
		        h = cvRound(hist_value * (256 - 1));
		        dstPtr[x] = (uchar) h;
	        }
        }

        cvReleaseHist(&cumulativeHist);
        cvReleaseHist(&hist);

        return GRAY;

    }
};

#endif
