//--------------------------------------------------------------------------
//
//  Copyright (C) 2005 Pablo d'Angelo
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "FAW Visualization Tools".
//  Its goal is offline and online visualization of 2d geometric data
//  and its export to various graphics file formats.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//--------------------------------------------------------------------------

#include "PropertyServerHandler.hh"
#include "CommProperty.hh"

using namespace UTIL;


PropertyServerHandler::PropertyServerHandler(PropertySet & root)
  : root(root)
{

}

/** dtor
 */
PropertyServerHandler::~PropertyServerHandler()
{
}

void
PropertyServerHandler::handleSend(const CommProperty &prop) throw()
{
  PropertySet::Key key(prop.getKey());
  const PropertySet::Key * subkey = key.getSubkey();
  try {
    if (key.getKey() == root.getKey()) {
      if (subkey) {
        root.getItem(*subkey).setString(prop.getValue());
      } else {
        // not possible, root must be a PropertySet
        std::cerr << "Cant change root Property: " << std::endl;
      }
    } else {
      std::cerr << "Unknown root property: " << key.getKey() << std::endl;
    }
  } catch (PropertySet::NoSuchProperty &e) {
    std::cerr << "handleSend(): PropertyItem not found: " << e.what() << std::endl;
  }
}

void
PropertyServerHandler::handleQuery(CHS::QueryServer<CommPropertyName, CommProperties> & server,
				   const CHS::QueryId id,
				   const CommPropertyName & name) throw()
{
  CommProperties props;
  std::cerr << "Received query for " << name.getKey() << std::endl;
  try {
    PropertySet::Key key(name.getKey());
    const PropertySet::Key * subkey = key.getSubkey();
    if (key.getKey() == root.getKey()) {
      if (subkey) {
        props.setProperties(root.getSubProperty(*subkey));
      } else {
        props.setProperties(root);
      }
    } else {
      std::cerr << "Unknown root property: " << key.getKey() << std::endl;
    }
  } catch (PropertySet::NoSuchProperty &e) {
    std::cerr << "No such Property: " << e.what() << std::endl;
  } catch (Property::Exception & e) {
    std::cerr << "Exception while getting property: " << e.what() << std::endl;
  }
  std::cerr << "sending back answer" << std::endl;
  server.answer(id, props);
}
