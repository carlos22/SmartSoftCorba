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

#ifndef SMART_PROPERTY_SET_HH
#define SMART_PROPERTY_SET_HH

#include <map>

#include "smartProperty.hh"
#include "smartPropertyItem.hh"

namespace Smart {

/** A property set
 *
 *  A collection of properties. It doesn't hold any value on it own, but
 *  its children might.
 */
class PropertySet: public Property, private Property::ChangeHandler
{
public:

  /** ctor */
  PropertySet();

  PropertySet(const PropertySet &source);  

  /** dtor
   */
  virtual ~PropertySet();

  PropertySet &operator=(const PropertySet &source);

  /**
    Create a copy of this property set.
   */
  virtual PropertySet *clone() const;

  virtual unsigned int num_sets() const;
  virtual unsigned int num_items() const;

  /** Add a Property to the subproperties.
   *  The value may be anything that can be written to a stream,
   *  or a reference to a property.
   *  Existing properties with the same name are overwritten.
   *  @param key indicates where to set the property value.
   *  @param value property value to set.
   */
  template<class Value>
  inline void set(const Key &key, const Value &value) throw(Exception);

  /**
    Remove a property from this set.
   */
  void remove(const Key &key) throw();

  /**
    Remove all properties from this set.
   */
  inline void clear() { _clear(); }

  /** Return a subProperty.
   *
   *  @param key relative to this PropertySet
   *  @return reference to the given property
   *
   *  @throw NoSuchProperty if the property could not be found.
   *  @throw WrongType if a PropertySet was expected during tree
   *                   traversal but a PropertyItem was found.
   */
  Property &get(const Key &key) throw(Exception);

  /** Return a const subProperty.
   *
   *  @param key relative to this PropertySet
   *  @return reference to the given property
   *
   *  @throw NoSuchProperty if the property could not be found.
   *  @throw WrongType if a PropertySet was expected during tree
   *                   traversal but a PropertyItem was found.
   */
  const Property &get(const Key &key) const throw(Exception);

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
  PropertyItem &getItem(const Key &key) throw (Exception);

  /** get a const PropertyItem.
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
  const PropertyItem &getItem(const Key &key) const throw (Exception);

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
  PropertySet &getSet(const Key &key) throw (Exception);

  /** get a const PropertySet.
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
  const PropertySet &getSet(const Key &key) const throw (Exception);

  std::string getString(const Key &key) const throw(Exception);

  bool tryGetString(const Key &key, std::string &s) const throw();

  int getInteger(const Key &key) const throw(Exception);

  template<class Integer>
  inline bool tryGetInteger(const Key &key, Integer &i) const throw()
  {
    try {
      i = this->getItem(key).getInteger();
      return true;
    } catch(Exception) {}
    return false;
  }

  double getDouble(const Key &key) const throw(Exception);

  template<class Double>
  inline bool tryGetDouble(const Key &key, Double &d) const throw()
  {
    try {
      d = this->getItem(key).getDouble();
      return true;
    } catch(Exception) {}
    return false;
  }

  bool getBool(const Key &key) const throw(Exception);

  bool tryGetBool(const Key &key, bool &b) const throw();

  virtual void save_xml(std::ostream &os = std::cout, const std::string &indent = "") const;
  virtual void print(std::ostream &os = std::cout, const std::string &indent = "") const;

protected:
  void _set_impl(const Key &key, Property *prop) throw(Exception);
  void _clear();
  void _copy(const PropertySet &source);

  /**
    Return the children map of property set \a prop_set.
    This is a workaround for a bug in gcc: protected elements of base
    class are not accessible from other instances of derived classes,
    which does not conform to the ANSI C++ standard.
   */
  inline const std::map<std::string,Property*> *_get_children(const PropertySet *prop_set) const
  {
    return &prop_set->_children;
  }

private:
  std::map<std::string,Property*> _children;
  std::map<const Property*,std::string> _children_inv;

  virtual void propertyAdded(const Property *origin, const Key *subkey);
  virtual void propertyChanged(const Property *origin, const Key *subkey);
  virtual void propertyRemoved(const Property *origin, const Key *subkey);
};

/** Add a Property to the subproperties.
 *
 *  @param prop property to insert here.
 *
 *  @throw NoSuchProperty if the indicated property set could not be found.
 *  @throw WrongType if a PropertySet was expected during tree
 *                   traversal but a PropertyItem was found.
 */
template<>
inline void PropertySet::set(const Key &key, const Property &prop) throw(Exception)
{
  this->_set_impl(key, prop.clone());
}

template<>
inline void PropertySet::set(const Key &key, const PropertySet &prop) throw(Exception)
{
  this->_set_impl(key, prop.clone());
}

template<>
inline void PropertySet::set(const Key &key, const PropertyItem &prop) throw(Exception)
{
  this->_set_impl(key, prop.clone());
}

template<>
inline void PropertySet::set(const Key &key, Property* const &prop) throw(Exception)
{
  this->_set_impl(key, prop);
}

template<>
inline void PropertySet::set(const Key &key, PropertySet* const &prop) throw(Exception)
{
  this->_set_impl(key, prop);
}

template<>
inline void PropertySet::set(const Key &key, PropertyItem* const &prop) throw(Exception)
{
  this->_set_impl(key, prop);
}

template<class Value>
inline void PropertySet::set(const Key &key, const Value &value) throw(Exception)
{
  this->_set_impl(key, new PropertyItem(value));
}

} // namespace Smart

#endif
