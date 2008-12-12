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

#ifndef _PROPERTYFACTORY_HH
#define _PROPERTYFACTORY_HH

#include "PropertySet.hh"

#include "ACEXML/common/DefaultHandler.h"

#include <stack>

namespace UTIL {

  /** @internal SAX Handler for parsing a XML property file
   *
   *  @warning doesn't support error handling nonexistant.
   */
  class PropertyHandler : public ACEXML_DefaultHandler
  {
  public:
    /**
       * Default constructor.
       */
    PropertyHandler(void);

    /**
       * destructor.
       */
    virtual ~PropertyHandler (void);

    // Methods inherit from ACEXML_ContentHandler.

      /*
       * Receive notification of the end of a document.
       */
//    virtual void endDocument ()
//      throw(ACEXML_SAXException);

      /*
       * Receive notification of the end of an element.
       */
    virtual void endElement (const ACEXML_Char *namespaceURI,
                             const ACEXML_Char *localName,
                             const ACEXML_Char *qName)
      throw(ACEXML_SAXException);

      /*
       * Receive notification of the beginning of a document.
       */
//  virtual void startDocument ()
//    throw(ACEXML_SAXException);

      /*
       * Receive notification of the beginning of an element.
       */
    virtual void startElement (const ACEXML_Char *namespaceURI,
                               const ACEXML_Char *localName,
                               const ACEXML_Char *qName,
                               ACEXML_Attributes *atts)
      throw (ACEXML_SAXException);


      /*
       * Receive notification about characters
       */
    virtual void characters (const ACEXML_Char *ch,
                             int start,
                             int length)
      throw (ACEXML_SAXException);

//    /*
//     * Receive notification of a recoverable error.
//     */
//    virtual void error (ACEXML_SAXParseException &exception)
//      throw (ACEXML_SAXException);

//    /*
//     * Receive notification of a non-recoverable error.
//     */
//    virtual void fatalError (ACEXML_SAXParseException &exception)
//      throw(ACEXML_SAXException);

//    /*
//     * Receive notification of a warning.
//     */
//    virtual void warning (ACEXML_SAXParseException &exception)
//      throw (ACEXML_SAXException);


    PropertySet * getRoot()
      { return root; };
  private:

    enum Mode { NONE, SET, ITEM, ERROR };

    Mode mode;

    std::stack<PropertySet *>parent;

    PropertySet * root;
    PropertyItem * cPropItem;
  };



  /** A factory class that can generate Property trees from XML files
   */
  class PropertyFactory
  {
  public:
    /// file not found
    class FileNotFound : public std::logic_error 
    { 
    public:
      FileNotFound(const std::string & file)
        : std::logic_error(file)
         { };
    };
    
    /// error while parsing the file.
    class ParseError : public std::logic_error 
    { 
    public:
      ParseError(const std::string & what)
        : std::logic_error(what)
         { };
    };
    
    /** Read properties from an XML file.
     *
     *  @bug error handling is non existant.
     *  @param filename of xml file
     *  @return pointer to the created PropertySet or 0 if the loading failed
     *
     *  @throw FileNotFound
     *  @throw ParseError
     */
    static UTIL::PropertySet * readXMLFile(const std::string & filename);

  };

} // UTIL

#endif // _PROPERTYFACTORY_HH
