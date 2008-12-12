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

#ifndef UTIL_PROPERTYSET_HH
#define UTIL_PROPERTYSET_HH

#include <iterator>


#include <Property.hh>
#include <typeinfo>


namespace UTIL {
    /** A property set
     *
     *  A collection of properties. It doesn't hold any value on it own, but
     *  its children might.
     */
    class PropertySet : public Property
    {
    public:

      typedef size_t size_type;

      /// SubProperty not found
      class NoSuchProperty : public Exception
      {
      public:
	NoSuchProperty(PropertySet * parent, const std::string & key)
	  : Exception("PropertySet::NoSuchProperty",key), parent(parent){};
        virtual ~NoSuchProperty() throw() { }

	PropertySet * parent;

      };

      /** a Node in the property didn't have the expected type.
       *
       *  Thrown when a Property has a unexpected type while traversing the
       *  property tree.
       */
      class WrongType : public Exception
      {
      public:
	WrongType(const std::string & key)
	  : Exception("PropertySet::WrongType", key) {};
        virtual ~WrongType() throw() { };
      };


      /** A multilevel key for PropertySet.
       *
       *  This class breaks a multilevel key into subkeys.
       */
      class Key
      {
      public:
	Key(const std::string &composite_key)
	  : subkey(0)
	  {
	    std::string::size_type dotpos = composite_key.find('.');
	    if(dotpos != std::string::npos)
	    {
	      key = composite_key.substr(0, dotpos);
	      subkey = new Key(composite_key.substr(dotpos+1));
	    } else {
	      key = composite_key;
	    }
	  }
	
	/// convenience constructor, because there is no
	/// automatic conversion from char -> string -> Key
	Key(const char * comp_key)
	  : subkey(0)
	  {
	    std::string composite_key(comp_key);
	    std::string::size_type dotpos = composite_key.find('.');
	    if(dotpos != std::string::npos)
	    {
	      key = composite_key.substr(0, dotpos);
	      subkey = new Key(composite_key.substr(dotpos+1));
	    } else {
	      key = composite_key;
	    }
	  }
	
	Key(Key &y)
	  {
	    key = y.key;
	    if (y.subkey) {
	      subkey = new Key(*y.subkey);
	    } else {
	      subkey = 0;
	    }
	  }
	
	Key & operator=(const Key & y)
	  {
	    key = y.key;
	    if (subkey) {
	      delete subkey;
	    }
	    if (y.subkey) {
	      subkey = new Key(*y.subkey);
	    } else {
	      subkey = 0;
	    }
	    return *this;
	  }
	
	~Key()
	  {
	    if (subkey) {
	      delete subkey;
	    }
	  }

        inline bool operator==(const Key & other) const
          {
            if (! (key == other.key)) return false;
            if ((subkey && other.subkey)) return (*subkey == *other.subkey);
            if ((subkey == 0) && (other.subkey == 0)) return true;
            return false;
          }
	
	/// return the current part of a key
	const std::string &getKey() const
	  { return key; }
	
	/// return the next part of the key, or 0 if there is none
	const Key *getSubkey() const
	  { return subkey; }
	
	// get the full name of this key.
  	std::string toString() const
  	  {
  	    std::string str(key);
  	    if (subkey) {
  	      subkey->addSubKeyPart(str);
  	    }
  	    return str;
  	  }
      protected:
	void addSubKeyPart(std::string & str)
	  {
	    str.append(".").append(key);
	    if (subkey) {
	      subkey->addSubKeyPart(str);
	    }
	  }
	
      private:
	std::string key;
	Key *subkey;
      };

	
      class const_iterator;
      friend class const_iterator;

      /** This iterator allows cycling through a PropertySet.
       *
       *  It supports only forward iteration.
       *
       *  When dereferenced, it returns pair<Key, PropertyItem*>,
       *  where Key is a complete key relative to the PropertySet
       *  from wich the iterator was received.
       */
      class const_iterator
      {
	
	typedef std::input_iterator_tag iterator_category;
	typedef std::pair<std::string, PropertyItem*> value_type;
	typedef size_t difference_type;
	typedef const value_type *pointer;
	typedef const value_type &reference;

	typedef std::map<std::string, Property *>::const_iterator map_iterator;
	typedef std::vector<std::pair<const PropertySet* , map_iterator *> > WalkState;

      public:
	
	// the default iterator is always "past the end"
	const_iterator()
	  : current(0) { };
	
	// creates a iterator that points to the first PropertyItem
	// in the property tree
	const_iterator(const PropertySet & root)
	  {
	    map_iterator * it =
	      new map_iterator(root.children.begin());
	    itStack.push_back(make_pair(&root, it));
	    // search for the first Property
	    findNextItem();
	  }

	/// dtor
	~const_iterator()
	  {
	    while(itStack.size() != 0) {
	      delete itStack.back().second;
	      itStack.pop_back();
	    }
	  }
	
	/// copy ctor
	const_iterator(const const_iterator & other)
	  {
	    current = other.current;
	    // copy traverse stack
	    WalkState::const_iterator it = other.itStack.begin();
	    while (it != other.itStack.end()) {
	      itStack.push_back(
		make_pair(it->first,
			  new map_iterator( *(it->second))
		  ));
		++it;
	    }
	  }

	/// assignment operator
	const_iterator & operator=(const const_iterator & other)
	  {
	    current = other.current;
	    while(itStack.size() != 0) {
	      delete itStack.back().second;
	      itStack.pop_back();
	    }
	    WalkState::const_iterator it = other.itStack.begin();
	    while (it != other.itStack.end()) {
	      itStack.push_back(
		make_pair((*it).first,
			  new map_iterator(*(it->second)) ));
	      ++it;
	    }
	    return *this;
	  }
	
	bool operator==(const const_iterator & x) const
	  { return current == x.current; }
	
	bool operator!=(const const_iterator& x) const
	  { return !(current == x.current); }
	
	std::pair<std::string, const PropertyItem *> operator*() const
	  {
	    if (current==0) {
	      throw std::logic_error("PropertySet iterator past the end!");
	    }
	    std::string key;
	    WalkState::const_iterator it = itStack.begin();
	    key = it->first->getKey();
	    it++;
	    while (it != itStack.end()) {
	      key.append(".").append(it->first->getKey());
	      ++it;
	    }
	    key.append(".").append(current->getKey());
	    return make_pair(key,current);
	  }

//	pointer
//	operator->() const { return &(operator*()); }

	const_iterator& operator++()
	  {
	    if (itStack.size() != 0) {
	      findNextItem();
	    }
	    return *this;
	  }

	const_iterator operator++(int)
	  { const_iterator tmp = *this; ++*this; return tmp; }

      private:
	
	bool findNextItem();

	const PropertyItem * current;
	// stack that saves our traversal state
//	std::vector<std::pair<PropertySet*, std::map<std::string, Property *>::const_iterator > > itStack ;
	WalkState itStack;
      };


      /** ctor */
      PropertySet(const std::string & key, ChangeHandler * changeHandler = 0)
	: Property(key, changeHandler) { };

      /** dtor
       */
      virtual ~PropertySet();

      /** add a Property to the subProperties.
       *
       *  Older properties will be silently overwritten.
       *
       *  @param prop property to insert here. Ownership of this object
       *              is transferred.
       */
      virtual void addChild(Property * prop);


      /** creates a new PropertyItem and returns it.
       *
       *  steps down and creates missing PropertySets and/or Items
       *  when needed.  If the key refers to a already existing
       *  PropertyItem it will be returned.
       *
       *  @throw WrongType if traversal failed
       */
      PropertyItem & createSubPropertyItem(const Key & key);

      /** try to get a subProperty.
       *
       *
       *  @throw NoSuchProperty if no element is known
       *  @throw NotAPropertySet if the item is not a property set.
       */
//        virtual Property & operator[](const std::string & name)
//  	{
//  	  std::map<std::string,Property *>::iterator childIt;
//  	  childIt = children.find(name);
//  	  if(childIt != children.end())
//  	  {
//  	    return *((*childIt).second);
//  	  }
//  	  else throw NoSuchProperty(name);
//  	}

      /** return a subProperty.
       *
       *  @param key relative to this PropertySet
       *  @return reference to the given property
       *
       *  @throw NoSuchProperty if the property could not be found.
       *  @throw WrongType if a PropertySet was expected during tree
       *                   traversal but a PropertyItem was found.
       */
      Property & getSubProperty(const Key &key) throw(Exception)
	{
	  std::map<std::string,Property *>::iterator childIt;
  	  childIt = children.find(key.getKey());
  	  if(childIt != children.end())
  	  {
	    if(key.getSubkey())
	    {
	      PropertySet *s = dynamic_cast<PropertySet*>(childIt->second);
	      if(s)
	      {
		return s->getSubProperty(*key.getSubkey());
	      }
	      // expected PropertySet, got PropertyItem
	      else throw WrongType(getCompleteKey() + key.getKey());
	    }
	    else return *(*childIt).second;
	  }
	  throw NoSuchProperty(this, getCompleteKey() + key.getKey());
	}

      /// const version.
      const Property &getSubProperty(const Key & key) const throw(Exception)
        {
          return getSubProperty(key);
        }

      /** get a PropertyItem.
       *
       *  @param key to a PropertyItem.
       *  @return reference to the PropertyItem specified by \e key
       *
       *  @throw NoSuchProperty if the property could not be found.
       *  @throw WrongType if aPropertySet was expected during tree
       *                   traversal but a PropertyItem was found.
       *                   It is also thrown if \e key pointed to a
       *                   PropertySet.
       */
      PropertyItem & getItem(const Key &key) throw(Exception)
	{
	  try
	  {
	    return dynamic_cast<PropertyItem&>(getSubProperty(key));
	  }
	  catch(std::bad_cast & e)
	  {
	    throw WrongType(getCompleteKey() + key.getKey()); // oder passenderes wie NoSuchItem
	  }
	}

      // const version
      const PropertyItem & getItem(const Key &key) const throw(Exception)
        {
          return getItem(key);
        }

      /** get a PropertySet.
       *
       *  @param key to a PropertySet.
       *  @return reference to the PropertySet specified by \e key
       *
       *  @throw NoSuchProperty if the property could not be found.
       *  @throw WrongType if aPropertySet was expected during tree
       *                   traversal but a PropertyItem was found.
       *                   It is also thrown if \e key pointed to a
       *                   PropertyItem.
       */
      PropertySet & getSet(const Key &key) throw(Exception)
	{
	  try
	  {
	    return dynamic_cast<PropertySet&>(getSubProperty(key));
	  }
	  catch(std::bad_cast & e)
	  {
	    throw WrongType(getCompleteKey() + key.getKey()); // oder passenderes wie NoSuchItem
	  }
	}

      // const version
      const PropertySet & getSet(const Key &key) const throw(Exception)
        {
          return getSet(key);
        }

      /// convience function, just calls getItem()
      PropertyItem & operator[](const Key & key)
	{ return getItem(key); }

      /** returns the number of PropertyItems of this Set and all subsets.
       */
      size_type size()
	{
	  size_type count = 0;
	  std::map<std::string,Property *>::const_iterator childIt;
	  for(childIt = children.begin(); childIt != children.end(); ++childIt) {
	    PropertyItem * item = dynamic_cast<PropertyItem *>(childIt->second);
	    if (item) {
	      count++;
	    } else {
	      PropertySet * set = dynamic_cast<PropertySet *>(childIt->second);
	      if (set) {
		count += set->size();
	      } else {
		throw Exception("Unknown Property in tree");
	      }
	    }
	  }
	  return count;
	}


      /** returns an iterator pointing to the end of the PropertySet
       *
       *  Can be used to traverse the whole tree and extract the
       *  PropertyItems in it.
       */
      const_iterator begin() const
	{ return const_iterator(*this); };

      /** returns an iterator pointing to the end of the PropertySet */
      const_iterator end() const
	{ return const_iterator(); };

      virtual void printXML(std::ostream & o, int indent = 0) const;
      virtual void print(std::ostream & o, int indent = 0) const;

    protected:
      std::map<std::string, Property *> children;

    private:
      bool operator==(const PropertySet & other) const;
      // copying not allowed
      PropertySet & operator=(const PropertySet &);
      PropertySet(const PropertySet &);

    };
} // UTIL

#endif // _PROPERTYSET_HH
