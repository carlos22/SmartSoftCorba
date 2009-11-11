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

#ifndef SEM_H_
#define SEM_H_

#include <string>

#include <mrpt/core.h>
using mrpt::synch::CSemaphore;
using std::string;

namespace mmp
{
	/**
	 * This class encapsels all the Semaphores
	 */
	class Sem
	{
		public:
			/**
			 * All Semaphore which are in this class
			 */
			enum SEMTYPE {
				INITOPENRAVE,
				STARTGUI,
			};

		private:
			/**
			 * Semaphore for init of OpenRave, everyone else has to what until this Semaphore is released
			 */
			CSemaphore* initOpenRave;

			/**
			 * If this Semaphore is released the GUI can be started
			 */
			CSemaphore* startGUI;

		public:
			/**
			 * Default Constructor
			 */
			Sem();

			/**
			 * Destructor
			 */
			virtual ~Sem();

			/**
			 * Acquires a Semaphore which has to be specified over the type
			 */
			void acquire(SEMTYPE type);

			/**
			 * Releases a Semaphre which has to be specified over the type
			 */
			void release(SEMTYPE type);
	};

	/**
	 * Sync class for mutex
	 */
	static Sem sem;
}

#endif /* SEM_H_ */
