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

#ifndef CHS_UTILS_PROPERTYSERVERHANDLER_HH
#define CHS_UTILS_PROPERTYSERVERHANDLER_HH

#include "CommProperty.hh"

#include <smartSoft.hh>

namespace UTIL {

    /** This Handler allows remote access to the properties.
     *
     *  The user must create a Send and a Query Server, and
     *  register this handler with them:
     *
     *  @code
     *  // Create the property Handler
     *  PropertyServerHandler propertyHandler(rootPropertySet);
     *  // create a send server to receive properties
     *  CHS::SendServer<CommProperty> propertyWriter(&component,"property",propertyHandler);
     *  // create a query server to allow retrival of our properties
     *  CHS::QueryServer<CommPropertyName,CommProperties> propertyExporter(&component,"property",propertyHandler);
     *  @endcode
     *
     *  @todo concurrent access to the Property is not threadsafe.
     *        it needs to be protected against concurrent access.
     */
    class PropertyServerHandler : public CHS::QueryServerHandler<CommPropertyName,CommProperties>,
				  public CHS::SendServerHandler<CommProperty>
    {
    public:
      /** constructs a new propertyHandler.
       *
       *  @param root of properties that should be exported by
       *              this Handler.
       */
      PropertyServerHandler(PropertySet & root);

      /** dtor
       */
      virtual ~PropertyServerHandler();

      void handleQuery(CHS::QueryServer<CommPropertyName, CommProperties> & server,
		       const CHS::QueryId id,
		       const CommPropertyName & name) throw();
      void handleSend(const CommProperty &v) throw();

    private:
      PropertySet & root;
    };

} // UTIL


#endif // _PROPERTYSERVERHANDLER_HH
