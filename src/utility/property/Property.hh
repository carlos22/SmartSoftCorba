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

#ifndef UTIL_PROPERTY_HH
#define UTIL_PROPERTY_HH

#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <iostream>

#include <iostream>

/** This namespace contains miscellaneous utility packages */
namespace UTIL {
    /** Base Class for all Properties.
     *
     *  This class represents a hierachy of Properties. They are organzied as
     *  in form of a tree. It's PropertySet subclass represents a node that can
     *  have PropertyItem leafs or other PropertySet's as children.
     *
     *  Each Property instance can only be inserted once. Adding the
     *  same Property at multiple places is not allowed.
     */
    class Property
    {

    public:

      //***************************************
      // exception classes used by Property

      /// base for all exceptions thrown by Property and subclasses
      class Exception : public std::logic_error
      {
      public:
	Exception(const std::string & exName)
	  : std::logic_error(exName)
          {};

	Exception(const std::string & exName,
                  const std::string & key)
	  : std::logic_error(exName),
                 key(key)
          {};

        virtual ~Exception() throw() {};

	std::string key;
      };

      /// Thrown if a conversion failed
      class ConversionFailed : public Exception
      {
      public:
	ConversionFailed(const std::string & reason, const std::string & key)
	  : Exception("Property::ConversionFailed: " + reason, key) {};
        virtual ~ConversionFailed() throw() {};
      };

      /** Handler to receive notifications when a property changes.
       */
      class ChangeHandler
      {
      public:

	/** Called when a Property changes.
         *
         *  Notifications are only guanteed if the value of a property
         *  changed. If (new_value == old_value) a change might or
         *  might not be reported.
	 *
	 *  @param origin Property that reported the change. If the Handler
	 *                was registred to a PropertySet, this will be the
	 *                origin, and not the PropertyItem that was actually
	 *                changed.
	 *
	 * @param path name of property that changed. relative to \e origin */
	virtual void propertyChanged(Property & origin, const std::string & path) = 0;
      };

      /** Create a Property.
       *
       *  @param key name of key
       *  @param changeHandler is notified when the property changes
       * */
      Property(const std::string & key, ChangeHandler * changeHandler = 0)
	: key(key), listener(changeHandler), parent(0)
	{ }

      /// dtor
      virtual ~Property();

      /** get the Key from the root of the property Hierachy
       *  up to this Key.
       *
       *  - root
       *    - sub1
       *     - prop1
       *
       *  prop1.getCompleteKey will return "root.sub1.prop1"
       *
       */
      std::string getCompleteKey() const
	{
	  std::string prefix;
	  prefix = getKey();
	  if (parent) {
	    parent->addKeyName(prefix);
	  }
	  return prefix;
	}

      /** get the Key */
      const std::string & getKey() const
	{ return key; };

      /** set a new ChangeHandler.
       *
       *  The old handler will be overridden.
       *
       *  @param handler that will be notified when the value
       *                 of a property changes. handler = 0
       *                 stops the notifications.
       */
      void setChangeHandler(ChangeHandler * handler)
	{ listener = handler; }

      /** Save the property to a stream.
       *
       *  Prints the property(tree) in xml format.
       */
      virtual void printXML(std::ostream & o, int indent=0) const = 0;

      /** Print in a human readable format
       */
      virtual void print(std::ostream & o, int indent=0) const = 0;

      /** Save properties to a string */
//      virtual void save(std::ostream & o) const = 0;

      /** Load data from a stream */
//      virtual void load(std::istream & i) = 0;

      /** Set our parent
       *
       *  @todo why cant this be protected?
       *        I thought the access control in c++ is on class and not on
       *        object level.
       */
      void setParent(Property * p)
	{ parent = p; }

    protected:
      /** Notify listener when a change occures.
       *
       *  propagates change notifications up to the root
       */
      virtual void valueChanged(const std::string & path);

      std::string key;
      ChangeHandler * listener;

      // pointer to our parent
      Property * parent;

    private:
      void addKeyName(std::string & str) const
	{
	  str.insert(0,key + '.');
	  if (parent) {
	    parent->addKeyName(str);
	  }
	}

//      friend std::ostream & operator<<(std::ostream & o, const Property & p);
    };

    /** This class holds the Property with values.
     *
     *  It can only be a leaf in the property tree.
     */
    class PropertyItem : public Property
    {
    public:
      PropertyItem(const std::string & key,
		  const std::string & value="",
		  ChangeHandler * changeHandler = 0)
	: Property(key, changeHandler), value(value) { };

      virtual ~PropertyItem() { };

      /** compare operator.
       *
       *  checks only key & value, the listener is ignored
       */
      bool operator==(const PropertyItem & other) const
        {
          return ((key == other.key) && (value == value));
        }

      void printXML(std::ostream & o, int indent=0) const
	{
	  std::string spaces(indent,' ');
	  o << spaces << "<property name=\"" << getKey() << "\">" << value << "</property>" << std::endl;
	}

      void print(std::ostream & o, int indent=0) const
	{
	  std::string spaces(indent,' ');
	  o << spaces << getKey() << " = " << value << std::endl;
	}


      // methods to access the value
      // they can throw the InvalidOperation or ConversionFailed exception

      /// return value as a string
      virtual const std::string & getString() const
	{ return value; }
      /** return value as a int
       *  @throw ConversionFailed if value cant be converted to
       *                          an integer
       */
      virtual int getInteger() const
	{
	  const char * start = value.c_str();
	  char * end = 0;
	  // force base 10
	  int i = strtol(start,&end, 10);
	  if (start == end) {
	    throw ConversionFailed(value + ": not an integer", getCompleteKey() );
	  }
	  return i;
	}

      /** return value as a double.
       *  @throw ConversionFailed if value cant be converted to
       *                          double
       */
      virtual double getDouble() const
	{
	  const char * start = value.c_str();
	  char * end = 0;
	  // force base 10
	  double d = strtod(start,&end);
	  if (start == end) {
	    throw ConversionFailed(value + ": not a double", getCompleteKey());
	  }
	  return d;
	}

      /** return value as a bool.
       *
       *  Truth values can be defined in many different formats:
       *    - true  :  yes | y | true  | t | + | 1
       *    - false :  no  | n | false | f | - | 0
       *
       *  @throw ConversionFailed if value cant be converted to
       *                          a bool
       */
      virtual bool getBool() const;

      // set methods
      virtual void setString(const std::string & str)
	{
          if (str != value) {
            // only notify if the value has actually changed.
            value = str;
            valueChanged("");
          }
	};

      virtual void setInteger(int i)
	{
	  std::stringstream ss;
	  ss << i;
	  value = ss.str();
	  valueChanged("");
	}

      virtual void setDouble(double d)
	{
	  std::stringstream ss;
	  ss << d;
	  value = ss.str();
	  valueChanged("");
	}

      virtual void setBool(bool b)
	{
	  value = b ? "true" : "false";
	  valueChanged("");
	}

    private:
      std::string value;
    };

}  // UTIL


#endif // _PROPERTY_HH
