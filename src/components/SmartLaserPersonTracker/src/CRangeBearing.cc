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


#include "CRangeBearing.hh"


CRangeBearing::CRangeBearing()
{
        //KF_options.method = kfEKFNaive;
        KF_options.method = kfEKFAlaDavison;

        // INIT KF STATE
        m_xkk.resize(2,0);      // State: (x,y,v,w)
        m_xkk[0]= VEHICLE_INITIAL_X;
        m_xkk[1]= VEHICLE_INITIAL_Y;

        // Initial cov:  Large uncertainty
        m_pkk.setSize(2,2);
        m_pkk.unit();
        m_pkk(0,0)= square( 0.5f );
        m_pkk(1,1)= square( 0.5f );

        max_cov = 0.64;
}


CRangeBearing::CRangeBearing(double x, double y)
{
        //KF_options.method = kfEKFNaive;
        KF_options.method = kfEKFAlaDavison;

        // INIT KF STATE
        m_xkk.resize(2,0);      // State: (x,y,v,w)
        m_xkk[0]= x;
        m_xkk[1]= y;

        // Initial cov:  Large uncertainty
        m_pkk.setSize(2,2);
        m_pkk.unit();
        m_pkk(0,0)= square( 0.5f );
        m_pkk(1,1)= square( 0.5f );

        max_cov = 0.64;
}

CRangeBearing::~CRangeBearing()
{

}


void  CRangeBearing::doProcess( double DeltaTime, double accumulatedDeltaTime,  double person_x, double person_y, bool personFound)
{
        m_deltaTime = (float)DeltaTime;
        m_accumulatedDeltaTime = (float)accumulatedDeltaTime;
        m_person_x = (float)person_x;
        m_person_y = (float) person_y;
        m_personFound = personFound;

        //cout<<"m_DeltaTime: "<<m_deltaTime<<endl;
        //cout<<"DeltaTime: "<<DeltaTime<<endl;
	//cout<<"obsBearing: "<<m_obsBearing<<endl;
	//cout<<"obsRange: "<<m_obsRange<<endl;

        runOneKalmanIteration();
}


/** Must return the action vector u.
  * \param out_u The action vector which will be passed to OnTransitionModel
  */
void CRangeBearing::OnGetAction( CVectorTemplate<KFTYPE> &u )
{
        u.resize(1);
        u[0] = m_deltaTime;
}

/** Implements the transition model \f$ \hat{x}_{k|k-1} = f( \hat{x}_{k-1|k-1}, u_k ) \f$
  * \param in_u The vector returned by OnGetAction.
  * \param inout_x At input has \f$ \hat{x}_{k-1|k-1} \f$, at output must have \f$ \hat{x}_{k|k-1} \f$.
  * \param out_skip Set this to true if for some reason you want to skip the prediction step (to do not modify either the vector or the covariance). Default:false
  */
void CRangeBearing::OnTransitionModel(
        const CVectorTemplate<KFTYPE>& in_u,
        CVectorTemplate<KFTYPE>&       inout_x,
        bool &out_skipPrediction
         )
{
        // in_u[0] : Delta time
        // in_out_x: [0]:x  [1]:y  [2]:vx  [3]: vy

/* test 7.9.2009 18:00
        inout_x[0] += in_u[0] * inout_x[2];
        inout_x[1] += in_u[0] * inout_x[3];
*/

}


/** Implements the transition Jacobian \f$ \frac{\partial f}{\partial x} \f$
  * \param out_F Must return the Jacobian.
  *  The returned matrix must be \f$N \times N\f$ with N being either the size of the whole state vector
 or get_vehicle_size().
  */
void CRangeBearing::OnTransitionJacobian(
        CMatrixTemplateNumeric<KFTYPE> & F
         )
{

        F.unit(2);
        /*if(m_personFound == true){
	F(0,0) = m_accumulatedDeltaTime;
        F(1,1) = m_accumulatedDeltaTime;
	}else{
	F(0,0) = m_deltaTime;
	F(1,1) = m_deltaTime;
        }
*/
}

/** Implements the transition noise covariance \f$ Q_k \f$
  * \param out_Q Must return the covariance matrix.
  *  The returned matrix must be of the same size than the jacobian from OnTransitionJacobian
  */
void CRangeBearing::OnTransitionNoise(
        CMatrixTemplateNumeric<KFTYPE> & Q
         )
{
        Q.unit(2);
        Q(0,0) = square( TRANSITION_MODEL_STD_XY );
        Q(1,1) = square( TRANSITION_MODEL_STD_XY );
}

/** This is called between the KF prediction step and the update step to allow the application to employ
 the prior distribution of the system state in the detection of observations (data association), where a
pplicable.
  * \param out_z A \f$N \times O\f$ matrix, with O being get_observation_size() and N the number of "obs
ervations": how many observed landmarks for a map, or just one if not applicable.
  * \param out_R2 A vector of size O (O being get_observation_size()) with the *variances* of the sensor
 noise for each of the observation components. This is constant for all the observations (where N>1), an
d in the naive Kalman method the value TKF_options::veryLargeR2 is used for unobserved map elements.
  * \param out_data_association An empty vector or, where applicable, a vector where the i'th element co
rresponds to the position of the observation in the i'th row of out_z within the system state vector, or
 -1 if it is a new map element and we want to insert it at the end of this KF iteration.
  *  This method will be called just once for each complete KF iteration.
  */
void CRangeBearing::OnGetObservations(
        CMatrixTemplateNumeric<KFTYPE> &out_z,
        CVectorTemplate<KFTYPE>        &out_R2,
        vector_int                     &out_data_association
        )

{

        if(m_personFound == true)
        {
           out_z.setSize(1,2);
           out_z(0,0) = m_person_x;
           out_z(0,1) = m_person_y;

           out_R2.resize(2);
           out_R2[0] = square( SENSOR_NOISE_STD );
           out_R2[1] = square( SENSOR_NOISE_STD );
           //out_R2[0] = square( 0.2f );
           //out_R2[1] = square( 0.2f );
        }
        else{
           out_z.setSize(0,0);
           out_R2.resize(0);
	}

        out_data_association.clear(); // Not used
}


/*:* Implements the observation prediction \f$ h_i(x) \f$ and the Jacobians \f$ \frac{\partial h_i}{\part
ial x} \f$ and (when applicable) \f$ \frac{\partial h_i}{\partial y_i} \f$.
  * \param in_z This is the same matrix returned by OnGetObservations(), passed here for reference.
  * \param in_data_association The vector returned by OnGetObservations(), passed here for reference.
  * \param in_full If set to true, all the Jacobians and predictions must be computed at once. Otherwise
, just those for the observation in_obsIdx.
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
void CRangeBearing::OnObservationModelAndJacobians(
        const CMatrixTemplateNumeric<KFTYPE> &in_z,
        const vector_int                     &in_data_association,
        const bool                           &in_full,
        const int                            &in_obsIdx,
        CVectorTemplate<KFTYPE>                  &ytilde,
        CMatrixTemplateNumeric<KFTYPE>       &out_Hx,
        CMatrixTemplateNumeric<KFTYPE>       &out_Hy
        )
{
        // predicted bearing:
        KFTYPE x = m_xkk[0];
        KFTYPE y = m_xkk[1];


        ytilde.resize(2);
        ytilde[0] =  in_z(0,0) - x;
        ytilde[1] =  in_z(0,1) - y;

        out_Hx.setSize(2,2);
        out_Hx.zeros();
        out_Hx(0,0) = 1;
        out_Hx(1,1) = 1;

        out_Hy.setSize(0,0); // Not used
}


/**	Implements operations post the kalmanfilter correct step
  */
void CRangeBearing::OnPostIteration(){

   const double min = 0.0635; // 0.0635
   if(m_pkk(0,0) < min || m_pkk(1,1) < min){
    m_pkk.unit();
    m_pkk(0,0) = min;
    m_pkk(1,1) = min;
   }

   const double max = max_cov; // 0.64; // 0.64 OK
   if(m_pkk(0,0) > max || m_pkk(1,1) > max){
    m_pkk.unit();
    m_pkk(0,0) = max;
    m_pkk(1,1) = max;
   }


}

