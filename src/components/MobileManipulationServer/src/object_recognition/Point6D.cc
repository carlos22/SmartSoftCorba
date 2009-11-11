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

#include "object_recognition/Point6D.hh"

namespace KatanaAPI
{

	Point6D::Point6D() :
		x(0.0), y(0.0), z(0.0), phi(0.0), theta(0.0), psi(0.0)
	{
	}

	Point6D::Point6D(double x, double y, double z, double phi, double theta, double psi) :
		x(x), y(y), z(z), phi(phi), theta(theta), psi(psi)
	{
	}

	void Point6D::setValues(double x, double y, double z, double phi, double theta, double psi)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->phi = phi;
		this->theta = theta;
		this->psi = psi;
	}
}
