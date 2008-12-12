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

#include "PropertyFactory.hh"

#include "ACEXML/common/FileCharStream.h"
#include "ACEXML/common/StrCharStream.h"
#include "ACEXML/parser/parser/Parser.h"

using namespace UTIL;
using namespace std;

PropertySet * PropertyFactory::readXMLFile(const std::string & filename)
{
  ACEXML_FileCharStream *fstm = new ACEXML_FileCharStream ();
  if (fstm->open (filename.c_str()) != 0) {
    cerr << "error opening file";
    throw FileNotFound(filename);
  }

  PropertyHandler handler;

  ACEXML_Parser parser;
  ACEXML_InputSource input(fstm);

  parser.setContentHandler (&handler);
  parser.setDTDHandler (&handler);
  parser.setErrorHandler (&handler);
  parser.setEntityResolver (&handler);

  try {
    parser.parse (&input);
  } catch (ACEXML_SAXException & ex) {
    cerr << "exception during parsing" << endl;
    throw ParseError("error during XML parsing");
  }
  return handler.getRoot();
}

PropertyHandler::PropertyHandler()
  : mode(NONE), root(0)
{
}

PropertyHandler::~PropertyHandler()
{
}

void PropertyHandler::startElement (const ACEXML_Char *namespaceURI,
				    const ACEXML_Char *localName,
				    const ACEXML_Char *qName,
				    ACEXML_Attributes *alist)
  throw (ACEXML_SAXException)
{
  // we always have a name
  const ACEXML_Char * key = 0;
  if (alist != 0)
    for (size_t i = 0; i < alist->getLength (); ++i)
    {
      if (strcmp(alist->getQName(i), "name") == 0) {
	key = alist->getValue(i);
      }
    }
  if (strcmp(localName,"propertyset") == 0) {
    if (key) {
      PropertySet * set = new PropertySet(key);
      parent.push(set);
      if (!root){
	root = set;
      }
      mode = SET;
      return;
    } else {
      cerr << "Error, invalid Set: name missing" << endl;
    }
  } else if (strcmp(localName,"property") == 0) {
    if (key) {
      cPropItem = new PropertyItem(key,"");
      mode = ITEM;
      return;
    } else {
      cerr << "Error, invalid item: name missing" << endl;
    }
  }
  mode = ERROR;
}

void PropertyHandler::endElement (const ACEXML_Char *namespaceURI,
				  const ACEXML_Char *localName,
				  const ACEXML_Char *qName)
  throw(ACEXML_SAXException)
{
  if (strcmp(localName,"propertyset") == 0) {
    PropertySet * set = parent.top();
    parent.pop();
    if (parent.size() != 0) {
      parent.top()->addChild(set);
    }
    mode = NONE;
  } else if (strcmp(localName,"property") == 0) {
    parent.top()->addChild(cPropItem);
    mode = NONE;
  }
}

void PropertyHandler::characters (const ACEXML_Char *ch,
				  int start,
				  int length)
  throw (ACEXML_SAXException)

{
  if (mode == ITEM) {
    cPropItem->setString(ch);
  }
}
