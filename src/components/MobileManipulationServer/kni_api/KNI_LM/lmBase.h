/*
 *   Katana Native Interface - A C++ interface to the robot arm Katana.
 *   Copyright (C) 2005 Neuronics AG
 *   Check out the AUTHORS file for detailed contact information.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/********************************************************************************/
#ifndef _LMBASE_H_
#define _LMBASE_H_
/********************************************************************************/

#include "KNI_InvKin/ikBase.h"
#include "common/exception.h"
#include <vector>
/********************************************************************************/


//------------------------------------------------------------------------------//

/*!	\brief	[LM] linear movement: points to be interpolated
 */
struct TLM_points {
    double pos;  //!< position of one point to be interpolated (% refer to the total trajectory)
    double time; //!< time that it takes to reach the point (from starting position)
};

/*!	\brief	[LM] linear movement: parameters
 */
struct TLMtrajectory {
    double*		arr_actpos;			//!< current position in cartesian units
    double*		arr_tarpos;			//!< target position in cartesian units
    int			distance;			//!< distance between target and current position
    double		time;				//!< time that it takes from current position to target position
    double		dt;					//!< time elapsed between one step and the next one
    short		number_of_points;	//!< number of points to interpolate
    TLM_points*	points;				//!< points to be interpolated
    short**		motors;				//!< motor position in each point to be interpolated
    double**	derivatives;		//!< second order derivatives of the polinomes that join the points, in the points
    double***	coefficients;		//!< coefficients of the polinomes that join the points
    short***	parameters;			//!< parameters to be sent in the command 'L' packet
};

/*!	\brief	[LM] Store intermediate targets for multiple linear movements
 */
struct TMLMIP {
    short	mlm_intermediate_pos[5];	  //!< current position in cartesian units
};


/*!	\brief	[LMBLEND] Standard coordinates for a point in space
 */

struct TPoint6D {
	double X;
	double Y;
	double Z;
	double Al;
	double Be;
	double Ga;
};

struct TPoint3D {
	double X;
	double Y;
	double Z;
};


struct TBlendtrace {
	TPoint3D	p1p2n;
	TPoint3D	p2p3n;
	TPoint3D	V1A;
	TPoint3D	V1B;
	TPoint3D	P1A;
	TPoint3D	P1B;
	TPoint3D	b1;
	TPoint3D	b2;
	TPoint3D	m1;
	TPoint3D	m2;
	double		tA;
	double		tB;
	double		distBA;
};

struct TSplinepoint {
	TPoint6D	point;
	double		time;
};

/*!	\brief	[LMBLEND] Trajectory points
 */

struct TBLENDtrajectory {
	TPoint6D*		referencepoints;
	short			number_of_referencepoints;
	TBlendtrace*	blendtrace; 
	short			number_of_blends;
	TSplinepoint*	splinepoints;
	short			number_of_splines;
	short			number_of_splinepoints;
	double			tEnd;

};

//---------------------------------------------------------------------------//

///
/// @addtogroup exceptions
/// @{
///


/// Joint speed too high.
/// \note error_number = -70
class JointSpeedException : public Exception {
public:
    JointSpeedException() throw(): 
	Exception("Joint speed too high", -70) {}
};

/// Wait parameter set to false.
/// \note error_number = -71
class WaitParameterException : public Exception {
public:
    WaitParameterException() throw(): 
	Exception("Wait parameter set to false", -71) {}
};

///
/// @}
///

//---------------------------------------------------------------------------//

/*!	\brief	Linear movement Class
 *
 *	This class allows to do linear movements with the Katana robot.
 */
class DLLDIR_LM CLMBase : public CikBase {

 private:
	double _maximumVelocity;
	bool   _activatePositionController;
        bool   _isInitialized;


    TLMtrajectory trajectory;
    TBLENDtrajectory blendtrajectory;



    void fillPoints(double vmax);
    void polDeviratives();
    void polCoefficients();

    void calcParameters(double* arr_actpos, double* arr_tarpos, double vmax);


/**
 * Calculates time needed for movement over a distance.
 *
 * @author Jonas Haller
 * @param distance	distance of the movement in mm
 * @param acc	acceleration at the beginning in mm/s^2
 * @param dec	deceleration at the end in mm/s^2
 * @param vmax	maximum velocity of the movement in mm/s
 * @return time needed for the movement in s
 */
	double totalTime(double distance, double acc, double dec, double vmax);

/**
 * Calculates the relative position reached after the relative time given.
 *
 * @author Jonas Haller
 * @param reltime	relative time (fraction of totaltime)
 * @param distance	distance of the movement in mm
 * @param acc	acceleration at the beginning in mm/s^2
 * @param dec	deceleration at the end in mm/s^2
 * @param vmax	maximum velocity of the movement in mm/s
 * @return relative distance (fraction of distance)
 */
	double relPosition(double reltime, double distance, double acc, double dec,
		double vmax);
	
/**
 * Calculates the spline coefficient and stores them in arr_p1 - arr_p4.
 *
 * Boundary conditions are that f_1'=0 and f_n'=0 (zero velocity at beginning
 * and end of the movement) and f_i''=P_(i+1)''.
 *
 * @author Jonas Haller
 * @param steps	number of splines to calculate
 * @param timearray	times of the points (length = steps + 1)
 * @param encoderarray	encoder values of the points (length = steps + 1)
 * @param arr_p1	to return parameters 1 (length = steps)
 * @param arr_p2	to return parameters 2 (length = steps)
 * @param arr_p3	to return parameters 3 (length = steps)
 * @param arr_p4	to return parameters 4 (length = steps)
 * @return void
 */
	void splineCoefficients(int steps, double *timearray, double *encoderarray,
		double *arr_p1, double *arr_p2, double *arr_p3, double *arr_p4);
	
/**
 * Checks if the joint speeds are below speed limit.
 *
 * Maximum joint speed is 180enc / 10ms.
 *
 * @author Jonas Haller
 * @param lastsolution	encoder values of last point
 * @param solution	encoder values of current point
 * @param time	time difference between the points in s
 * @return true if joint speeds ok, false if joint speed too high
 */
	bool checkJointSpeed(std::vector<int> lastsolution,
		std::vector<int> solution, double time);

//     TRetBLEND blend_calculate_blendtraces(double amax, double vmax, long tightness);
//     TRetBLEND blend_fill_splinepoints(double kappa, double amax, double vmax);
//     double blend_Ivontacc(double tp, double dist, double amax, double vmax);
//     TRetBLEND blend_Ivontdec(double tp, double dist, double amax, double vmax, double* I);

 public:

    CLMBase() : _maximumVelocity(20), _activatePositionController(true), _isInitialized(false) {}

    /// Initialize the parameters for the linear movements.
    /// This is in the case you want to initialize it manually
    /// @note If you do not call it, moveRobotLinearTo() will do it for you automatically
    void initLM();

	/// @param wait has to be true with new implementation of movLM2P
    void movLM(double X, double Y, double Z, 
		 double Al, double Be, double Ga,      
		 bool exactflag, double vmax, bool wait=true, int tolerance = 100, long timeout = TM_ENDLESS);	

/**
 * Old version of movLM2P which uses L-Command (only 4 splines)
 */
	void movLM2PwithL(double X1, double Y1, double Z1, double Al1, double Be1,
		double Ga1, double X2, double Y2, double Z2, double Al2, double Be2,
		double Ga2, bool exactflag, double vmax, bool wait=false,
		int tolerance = 100, long timeout = TM_ENDLESS);

    void movLM2P4D(double X1, double Y1, double Z1, 
		 double Al1, double Be1, double Ga1,      
				  double X2, double Y2, double Z2, 
		 double Al2, double Be2, double Ga2, 
				  bool exactflag, double vmax, bool wait=false, int tolerance = 100, long timeout = TM_ENDLESS);	

/**
 * New version of movLM2P with multiple splines.
 *
 * @author Jonas Haller
 * @param X1	X coordinate of actual position
 * @param Y1	Y coordinate of actual position
 * @param Z1	Z coordinate of actual position
 * @param Ph1	Phi angle of actual position
 * @param Th1	Theta angle of actual position
 * @param Ps1	Psi angle of actual position
 * @param X2	X coordinate of target position
 * @param Y2	Y coordinate of target position
 * @param Z2	Z coordinate of target position
 * @param Ph2	Phi angle of target position
 * @param Th2	Theta angle of target position
 * @param Ps2	Psi angle of target position
 * @param exactflag	activate the position controller after the movement
 * @param vmax	maximum velocity of the movement in mm/s
 * @param wait	param for legacy reasons only, has to be true
 * @param tolerance	tolerance for all motor encoders
 * @param timeout	timeout for linear movement in ms
 * @throws NoSolutionException	if no solution found for IK
 * @throws JointSpeedException	if joint speed too high
 * @throws WaitParameterException	if wait set to false
 * @return void
 */
    void movLM2P(double X1, double Y1, double Z1, double Al1, double Be1,
    	double Ga1, double X2, double Y2, double Z2, double Al2, double Be2,
    	double Ga2, bool exactflag, double vmax, bool wait=true,
    	int tolerance = 100, long timeout = TM_ENDLESS);

	void   setMaximumLinearVelocity(double maximumVelocity);
	double getMaximumLinearVelocity() const;

	///
	/// Re-Activate the position controller after the linear movement.
	/// \note This can result in a small movement after the movement
	void setActivatePositionController(bool activate);

	///
	/// Check if the position controller will be activated after the linear movement
	bool getActivatePositionController();

	/// @param waitUntilReached has to be true with new implementation of movLM2P
	void moveRobotLinearTo(
		double x,   double y,     double z, 
		double phi, double theta, double psi,
		bool   waitUntilReached = true, 
		int    waitTimeout = TM_ENDLESS);

        /// This method does the same as the one above and is mainly provided for convenience
        /// \note You can call this function in python using tuples:
        ///       Example: katana.moveRobotLinearTo( (x,y,z,phi,theta,psi) )
        /// \note If the size of the container is smaller than 6, it will throw an exception!
        void moveRobotLinearTo(
		std::vector<double> coordinates,
		bool   waitUntilReached = true, 
		int    waitTimeout = TM_ENDLESS);

//     TRetBLEND movLMBlend(double X1, double Y1, double Z1, 
// 		 double Al1, double Be1, double Ga1,      
// 				  double X2, double Y2, double Z2, 
// 		 double Al2, double Be2, double Ga2,
// 				  double vmax, long tightness);


};

/********************************************************************************/
#endif //_IKBASE_H_
/********************************************************************************/
