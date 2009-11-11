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

#include "path_planning/ObjectXMLWriter.hh"

namespace mmp
{
	const string ObjectXMLWriter::KINBODY_OPEN = "<KinBody ";
	const string ObjectXMLWriter::KINBODY_OPEN_CLOSE = ">";
	const string ObjectXMLWriter::KINBODY_CLOSE = "</KinBody>";

	const string ObjectXMLWriter::BODY_OPEN = "<Body ";
	const string ObjectXMLWriter::BODY_OPEN_CLOSE = ">";
	const string ObjectXMLWriter::BODY_CLOSE = "</Body>";

	const string ObjectXMLWriter::NAME = "name=";
	const string ObjectXMLWriter::TYPE_DYNAMIC = "type=\"dynamic\"";

	const string ObjectXMLWriter::GEOM_OPEN_BOX = "<Geom type=\"box\">";
	const string ObjectXMLWriter::GEOM_OPEN_CYCLINDER  = "Geom type=\"cylinder\">";
	const string ObjectXMLWriter::GEOM_CLOSE = "</Geom>";

	const string ObjectXMLWriter::EXTENTS_OPEN = "<extents>";
	const string ObjectXMLWriter::EXTENTS_CLOSE = "</extents>";

	const string ObjectXMLWriter::DIFFUSECOLOR_OPEN = "<diffuseColor>";
	const string ObjectXMLWriter::DIFFUSECOLOR_CLOSE = "</diffuseColor>";

	const string ObjectXMLWriter::TRANSLATION_OPEN = "<translation>";
	const string ObjectXMLWriter::TRANSLATION_CLOSE = "</translation>";

	const string ObjectXMLWriter::ROTATIONAXIS_OPEN = "<rotationaxis>";
	const string ObjectXMLWriter::ROTATIONAXIS_CLOSE = "</rotationaxis>";

	const string ObjectXMLWriter::RADIUS_OPEN = "<radius>";
	const string ObjectXMLWriter::RADIUS_CLOSE = "</radius>";

	const string ObjectXMLWriter::HEIGHT_OPEN = "<height>";
	const string ObjectXMLWriter::HEIGHT_CLOSE = "</height>";

	ObjectXMLWriter::ObjectXMLWriter()
	{
	}

	ObjectXMLWriter::~ObjectXMLWriter()
	{
	}

	void ObjectXMLWriter::writeKinBodyBox(const string& file, int name, double x, double y, double z) {
		this->stream.open(file.c_str(), std::ios_base::out | std::ios_base::trunc);

		this->stream << KINBODY_OPEN << NAME << "\"object" << name << "\"" << KINBODY_OPEN_CLOSE;
		this->stream << BODY_OPEN << NAME << "\"object" << name << "\" " <<  TYPE_DYNAMIC  << BODY_OPEN_CLOSE;
		this->stream << GEOM_OPEN_BOX;
		this->stream << EXTENTS_OPEN << x / 2 << " " << y / 2 << " " << z / 2 << EXTENTS_CLOSE;
		this->stream << GEOM_CLOSE;
		this->stream << BODY_CLOSE;
		this->stream << KINBODY_CLOSE;

		this->stream.close();
	}
}
