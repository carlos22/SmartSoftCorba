//--------------------------------------------------------------------------
//  Copyright (C) 2009 Jonas Brich
//
//        brich@mail.hs-ulm.de
//
//	Contact:
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "Katana Mobile Manipulation component".
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

#ifndef __POINT6D_H__
#define __POINT6D_H__

namespace KatanaAPI {

	/**
	 * Class that represents a 6D point also known as pose.
	 */
	class Point6D
	{
		private:
			double x;
			double y;
			double z;
			double phi;
			double theta;
			double psi;

		public:
			Point6D();
			Point6D(double x, double y, double z, double phi, double theta, double psi);

			void setValues(double x, double y, double z, double phi, double theta,
					double psi);

			void setX(double x) {
				this->x = x;
			}

			void setY(double y) {
				this->y = y;
			}

			void setZ(double z) {
				this->z = z;
			}

			void setPhi(double phi) {
				this->phi = phi;
			}

			void setTheta(double theta) {
				this->theta = theta;
			}

			void setPsi(double psi) {
				this->psi = psi;
			}

			double getX() const {
				return x;
			}

			double getY() const {
				return y;
			}

			double getZ() const {
				return z;
			}

			double getPhi() const {
				return phi;
			}

			double getTheta() const {
				return theta;
			}

			double getPsi() const {
				return psi;
			}

	};
}

#endif /* __POINT6D_H__ */
