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

#include "Property.hh"

using namespace UTIL;

Property::~Property() {}

void Property::valueChanged(const std::string & path)
{
  if (listener) {
    listener->propertyChanged(*this,path);
  }
  if (parent) {
    // quite expensive on deep trees
    std::string relpath(key);
    if (path.length() != 0) {
      relpath.append(".").append(path);
    }
    parent->valueChanged(relpath);
  }
}

//======================================================================



//======================================================================


bool PropertyItem::getBool() const
{
  // need to remove whitespace in front
  std::string myval(value);
  std::transform (myval.begin(), myval.end(), myval.begin(), tolower);
  // strip whitespace.
  while((myval.size() > 0) && (myval[0] == ' ' || myval[0] == '\t')){
    myval.erase(0,myval.find_first_not_of(' '));
    myval.erase(0,myval.find_first_not_of('\t'));
  }
  //end of word.
  std::string::size_type eow = myval.find_first_of(' ');
  std::string::size_type eow2 = myval.find_first_of(' ');
  eow = eow > eow2 ? eow : eow2;
  while((myval.size() > eow) && (myval[eow] == ' ' || myval[eow] == '\t')){
    myval.erase(eow,myval.find_first_not_of(' ',eow));
    myval.erase(eow,myval.find_first_not_of('\t',eow));
  }


  if (myval=="yes" || myval=="true" || myval=="y" || myval =="t" || myval=="+" || myval=="1" ) {
    return true;
  } else if (myval=="no" || myval=="false" || myval=="n" || myval=="f" || myval=="-" ||  myval=="0") {
    return false;
  } else {
    throw ConversionFailed(myval + ": not recognized as boolean", getCompleteKey());
  }
}
