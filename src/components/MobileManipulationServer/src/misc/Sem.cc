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

#include "misc/Sem.hh"

namespace mmp
{
	/*************************************************

	 Public Methods

	 *************************************************/

	Sem::Sem()
	{
		this->initOpenRave = new CSemaphore(0, 1, "");
		this->startGUI = new CSemaphore(0, 1, "");
	}

	Sem::~Sem()
	{
		delete this->initOpenRave;
		delete this->startGUI;
	}

	void Sem::acquire(SEMTYPE type)
	{
		switch (type)
		{
		case Sem::INITOPENRAVE:
		{
			this->initOpenRave->waitForSignal(0);
			break;
		}
		case Sem::STARTGUI:
		{
			this->startGUI->waitForSignal(0);
			break;
		}
		}
	}

	void Sem::release(SEMTYPE type)
	{
		switch (type)
		{
		case Sem::INITOPENRAVE:
		{
			this->initOpenRave->release(1);
			break;
		}
		case Sem::STARTGUI:
		{
			this->startGUI->release(1);
			break;
		}
		}
	}
}
