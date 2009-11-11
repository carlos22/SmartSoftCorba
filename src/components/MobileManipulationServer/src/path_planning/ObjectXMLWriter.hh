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

#ifndef OBJECTXMLWRITER_H_
#define OBJECTXMLWRITER_H_

#include <fstream>
#include <string>

using std::ofstream;
using std::string;

namespace mmp
{

	class ObjectXMLWriter
	{
		private:
			ofstream stream;

			static const string KINBODY_OPEN;
			static const string KINBODY_OPEN_CLOSE;
			static const string KINBODY_CLOSE;

			static const string BODY_OPEN;
			static const string BODY_OPEN_CLOSE;
			static const string BODY_CLOSE;

			static const string NAME;
			static const string TYPE_DYNAMIC;

			static const string GEOM_OPEN_BOX;
			static const string GEOM_OPEN_CYCLINDER;
			static const string GEOM_CLOSE;

			static const string EXTENTS_OPEN;
			static const string EXTENTS_CLOSE;

			static const string DIFFUSECOLOR_OPEN;
			static const string DIFFUSECOLOR_CLOSE;

			static const string TRANSLATION_OPEN;
			static const string TRANSLATION_CLOSE;

			static const string ROTATIONAXIS_OPEN;
			static const string ROTATIONAXIS_CLOSE;

			static const string RADIUS_OPEN;
			static const string RADIUS_CLOSE;

			static const string HEIGHT_OPEN;
			static const string HEIGHT_CLOSE;
		public:
			ObjectXMLWriter();
			virtual ~ObjectXMLWriter();

			void writeKinBodyBox(const string& file, int name, double x, double y, double z);
	};

}

#endif /* OBJECTXMLWRITER_H_ */
