// --------------------------------------------------------------------------
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
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
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
//
//
// --------------------------------------------------------------------------


#ifndef _COMMPROPERTYNAMES_HH
#define _COMMPROPERTYNAMES_HH

#include "PropertyStructC.hh"
#include "PropertySet.hh"

#include <string>
#include <cassert>

namespace UTIL {

    /** Communication object used when querying properties
     */
    class CommPropertyName
    {
    private:
      std::string name;

    public:

      /** ctor
       */
      CommPropertyName() {};

      /** ctor
       */
      CommPropertyName(const std::string & key)
	: name(key){};

      /** dtor
       */
      virtual ~CommPropertyName() { };

      void get(CORBA::Any &a) const
        {
          // consuming insertion
          CHS::PropertyNameStruct z;
          z.name = name.c_str();
          a <<= z;
        }

//      void get(CORBA::Any &a) const
//	{
//        a <<= CORBA::string_dup(name.c_str());
//	}

      void set(const CORBA::Any &a)
        {
          CHS::PropertyNameStruct *z;

          a >>= z;

          name = z->name;
        }

//      void set(const CORBA::Any &a)
//	{
//	  const char * ns = 0;
//
//	  a >>= ns;
//	  name = ns;
//	}

      static inline std::string identifier(void) {
        return "UTIL::propertyName";
      };


      /** Add a Name.
       */
      void setKey(const std::string & n)
	{
	  name = n;
	};

      /** Add a Name.
       */
      const std::string & getKey() const
	{
	  return name;
	};
    };

    class CommProperty
    {
    public:

      // required wrapping/unwrapping functions
      /** @internal
       */
      void get(CORBA::Any &a) const
	{
	  CHS::PropertyStruct * ps = new CHS::PropertyStruct;
	
	  ps->key = key.c_str();
	  ps->value = value.c_str();
	
	  // consuming insertion
	  a <<= ps;
	}

      /** @internal
       */
      void set(const CORBA::Any & a)
	{
	  const CHS::PropertyStruct * ps = 0;

	  a >>= ps;
	  key = ps->key;
	  value = ps->value;
	}

      /** @internal
       */
      static inline std::string identifier(void) {
        return "UTIL::property";
      };


      /** @internal
       *  write state to idl struct
       */
      void getStruct(CHS::PropertyStruct & dest) const
	{
	  dest.key = key.c_str();
	  dest.value = value.c_str();
	}

      /** @internal
       *  set state from idl struct
       */
      void setFromStruct(const CHS::PropertyStruct & src)
	{
	  key = src.key;
	  value = src.value;
	};

      const std::string & getKey() const
	{ return key; }

      const std::string & getValue() const
	{ return value; }

      void setProperty(const PropertyItem & item)
        {
          key = item.getKey();
          value = item.getString();
        }

      void setProperty(const std::string & key_, const std::string & value_)
        {
          key = key_;
          value = value_;
        }

    private:
      std::string key;
      std::string value;
    };


    /** Communication object uses when querying properties
     */
    class CommProperties
    {
    public:

      /** ctor
       */
      CommProperties() { };
      /** dtor
       */
      virtual ~CommProperties() {};

      void get(CORBA::Any &a ) const
	{
	  CHS::PropertyStructSequence * ps = new CHS::PropertyStructSequence;
	
	  int len = items.size();
	  ps->length(len);
	  std::vector<std::pair<std::string, std::string> >::const_iterator it = items.begin();
	  for (int i=0; i<len; i++) {
	    (*ps)[i].key = CORBA::string_dup(it->first.c_str());
	    (*ps)[i].value = CORBA::string_dup(it->second.c_str());
	    ++it;
	  }
	  // consuming insertion
	  a <<= ps;
	}

      void set(const CORBA::Any & a)
	{
	  CHS::PropertyStructSequence * ns = 0;

	  a >>= ns;
          assert(ns);
	  // delete old tree
	  items.clear();
	  size_t len = ns->length();
	  for (size_t i = 0; i<len; i++) {
	    items.push_back(make_pair(std::string((*ns)[i].key),
				      std::string((*ns)[i].value)));
	  }
	}

      static inline std::string identifier(void) {
        return "UTIL::properties";
      };


      /** set properties.
       */
      void setProperties(const Property & property)
	{
	  items.clear();

	  try {
	    const PropertySet & set = dynamic_cast<const PropertySet &>(property);
	    PropertySet::const_iterator it;
	    for(it=set.begin(); it !=set.end(); ++it) {
	      const PropertyItem * item = (*it).second;
	      items.push_back(make_pair(item->getCompleteKey(),
					item->getString()));
	    }
	  } catch (std::bad_cast &) {
	    try {
	      const PropertyItem & item = dynamic_cast<const PropertyItem &>(property);
	      items.push_back(make_pair(item.getCompleteKey(),
					item.getString()));
	    } catch (std::bad_cast &) {
	      // shouldn't happen.
	    }
	  }
	};

      /** update properties with values stored in this object.
       *
       *  @param root destination PropertySet
       */
      void updateProperties(PropertySet & root)
	{
	  std::vector<std::pair<std::string, std::string> >::const_iterator it;
	  for(it=items.begin(); it != items.end(); ++it) {
	    PropertySet::Key tmp(it->first);
	    const PropertySet::Key * key = tmp.getSubkey();
//	    std::assert(key != 0);
	    try {
	      root.getItem(*key).setString((*it).second);
	    } catch (PropertySet::NoSuchProperty & e) {
	      std::cerr << "PropertyItem not found: " << it->first << std::endl;
	    }
	  }
	};

      /** creates a PropertySet containing all properties stored in
       *  this communication object.
       */
      PropertySet * getProperties() const
	{
	  if (items.empty()) {
	    return 0;
	  }
	  std::vector<std::pair<std::string, std::string> >::const_iterator it;
	  it = items.begin();
	  PropertySet::Key tmp(it->first);
	  PropertySet * root = new PropertySet(tmp.getKey());
	  const PropertySet::Key * key = tmp.getSubkey();
	  assert(key);
	  for(it=items.begin(); it != items.end(); ++it) {
	    PropertySet::Key tmpk2(it->first);
	    const PropertySet::Key * subkey = tmpk2.getSubkey();
            try {
              root->createSubPropertyItem(*subkey).setString(it->second);
            } catch (PropertySet::WrongType & e) {
              std::cerr << "Tree not compatible: found Set instead of Item" << std::endl;
	    }
	  }
	  return root;
	}
    private:
      std::vector<std::pair<std::string, std::string> > items;
    };
} // UTIL

#endif // _COMMPROPERTYNAMES_HH
