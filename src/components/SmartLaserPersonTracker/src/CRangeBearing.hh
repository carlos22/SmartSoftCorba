// --------------------------------------------------------------------------
//
//  Copyright (C) 2009-2010 Andreas Steck, Matthias Lutz
//
//        lutz@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
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
//----------------------------------------------------------------------------
//
// CREDITS:
// Parts of the code for the EKF were taken from the Mobile Robot Programming Toolkit,
// which is distributed under GPL, and you can find at http://mrpt.sourceforge.net/
/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2009  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   +---------------------------------------------------------------------------+ */


#include <mrpt/core.h>

using namespace mrpt;
using namespace mrpt::bayes;
using namespace mrpt::gui;
using namespace mrpt::math;
using namespace mrpt::slam;
using namespace mrpt::utils;
using namespace std;

#define SENSOR_NOISE_STD	0.5f

#define VEHICLE_INITIAL_X       0.8f
#define VEHICLE_INITIAL_Y       0.0f
#define VEHICLE_INITIAL_VX      0.0f
#define VEHICLE_INITIAL_VY      0.0f

#define TRANSITION_MODEL_STD_XY 0.1f


// ------------------------------------------------------
//              Implementation of the system models as a EKF
// ------------------------------------------------------
class CRangeBearing : public CKalmanFilterCapable
{
public:
        CRangeBearing( );
        CRangeBearing(double x, double y);
        virtual ~CRangeBearing();

        void  doProcess( double DeltaTime, double accumulatedDeltaTime, double observationRange, double observationBearing ,bool personFound);

        void getState( CVectorTemplate<KFTYPE>  &xkk, CMatrixTemplateNumeric<KFTYPE>    &pkk)
        {
                xkk = m_xkk;
                pkk = m_pkk;
        }

        void setMaxCov(double mc) { max_cov = mc; }

 protected:

        float m_person_x ,m_person_y;
        float m_deltaTime;
	float m_accumulatedDeltaTime;
        bool  m_personFound;
        double max_cov;

        /** @name Virtual methods for Kalman Filter implementation
                @{
         */

        /** Must return the action vector u.
          * \param out_u The action vector which will be passed to OnTransitionModel
          */
        void OnGetAction( CVectorTemplate<KFTYPE> &out_u );

        /** Implements the transition model \f$ \hat{x}_{k|k-1} = f( \hat{x}_{k-1|k-1}, u_k ) \f$
          * \param in_u The vector returned by OnGetAction.
          * \param inout_x At input has \f$ \hat{x}_{k-1|k-1} \f$, at output must have \f$ \hat{x}_{k|k-
1} \f$.
          * \param out_skip Set this to true if for some reason you want to skip the prediction step (to
 do not modify either the vector or the covariance). Default:false
          */
        void OnTransitionModel(
                const CVectorTemplate<KFTYPE>& in_u,
                CVectorTemplate<KFTYPE>&       inout_x,
                bool &out_skipPrediction
                 );

        /** Implements the transition Jacobian \f$ \frac{\partial f}{\partial x} \f$
          * \param out_F Must return the Jacobian.
          *  The returned matrix must be \f$N \times N\f$ with N being either the size of the whole stat
e vector or get_vehicle_size().
          */
        void OnTransitionJacobian(
                CMatrixTemplateNumeric<KFTYPE> & out_F
                 );

        /** Implements the transition noise covariance \f$ Q_k \f$
          * \param out_Q Must return the covariance matrix.
          *  The returned matrix must be of the same size than the jacobian from OnTransitionJacobian
          */
        void OnTransitionNoise(
                CMatrixTemplateNumeric<KFTYPE> & out_Q
                 );

/** This is called between the KF prediction step and the update step to allow the application t
o employ the prior distribution of the system state in the detection of observations (data association),
 where applicable.
          * \param out_z A \f$N \times O\f$ matrix, with O being get_observation_size() and N the number
 of "observations": how many observed landmarks for a map, or just one if not applicable.
          * \param out_R2 A vector of size O (O being get_observation_size()) with the *variances* of the sensor noise for each of the observation components. This is constant for all the observations (where N>1), and in the naive Kalman method the value TKF_options::veryLargeR2 is used for unobserved map elements.
          * \param out_data_association An empty vector or, where applicable, a vector where the i'th element corresponds to the position of the observation in the i'th row of out_z within the system state vector, or -1 if it is a new map element and we want to insert it at the end of this KF iteration.
          *  This method will be called just once for each complete KF iteration.
          */
        void OnGetObservations(
                CMatrixTemplateNumeric<KFTYPE> &out_z,
                CVectorTemplate<KFTYPE>        &out_R2,
                vector_int                     &out_data_association
                );

        /** Implements the observation prediction \f$ h_i(x) \f$ and the Jacobians \f$ \frac{\partial h_i}{\partial x} \f$ and (when applicable) \f$ \frac{\partial h_i}{\partial y_i} \f$.
          * \param in_z This is the same matrix returned by OnGetObservations(), passed here for reference.
          * \param in_data_association The vector returned by OnGetObservations(), passed here for reference.
          * \param in_full If set to true, all the Jacobians and predictions must be computed at once. Otherwise, just those for the observation in_obsIdx.
          * \param in_obsIdx If in_full=false, the row of the observation (in in_z and in_data_association) whose innovation & Jacobians are to be returned now.
          * \param out_innov The difference between the expected observation and the real one: \f$ \tilde{y} = z - h(x) \f$. It must be a vector of length O*N (in_full=true) or O (in_full=false), with O=get_observation_size() and N=number of rows in in_z.
          * \param out_Hx One or a vertical stack of \f$ \frac{\partial h_i}{\partial x} \f$.
          * \param out_Hy An empty matrix, or one or a vertical stack of \f$ \frac{\partial h_i}{\partial y_i} \f$.
          *
          *  out_Hx must consist of 1 (in_full=false) or N (in_full=true) vertically stacked \f$O \times V\f$ matrices, and out_Hy must consist of 1 or N \f$O \times F\f$ matrices, with:
          *  - N: number of rows in in_z (number of observed features, or 1 in general).
          *  - O: get_observation_size()
          *  - V: get_vehicle_size()
          *  - F: get_feature_size()
          */
        void OnObservationModelAndJacobians(
                const CMatrixTemplateNumeric<KFTYPE> &in_z,
                const vector_int                     &in_data_association,
                const bool                           &in_full,
                const int                            &in_obsIdx,
                CVectorTemplate<KFTYPE>                  &out_innov,
                CMatrixTemplateNumeric<KFTYPE>       &out_Hx,
                CMatrixTemplateNumeric<KFTYPE>       &out_Hy
                );


        /** Must return the dimension of the "vehicle state": either the full state vector or the "vehicle" part if applicable.
         */
        size_t get_vehicle_size()  const { return 2; /* x y vx vy*/ }

        // get_feature_size Is not implemented since this is not a "mapping" problem. Default method will return 0.

        /** Must return the dimension of each observation (eg, 2 for pixel coordinates, 3 for 3D coordinates,etc).
         */
        size_t get_observation_size()  const { return 2; /* range+bearing */ }

 /** @}
         */

	void OnPostIteration();

};

