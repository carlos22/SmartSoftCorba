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

#ifndef SMART_PROPERTY_HH
#define SMART_PROPERTY_HH

#include <iostream>
#include <stdexcept>
#include <set>
#include <string>

namespace Smart 
{

class PropertyItem;
class PropertySet;

/** Abstract base class for all properties.
 *
 *  This class represents a hierachy of properties. They are organzied
 *  in form of a tree. It's PropertySet subclass represents a node that can
 *  have PropertyItem leafs or other PropertySet's as children.
 *
 *  Each Property instance can only be inserted once. Adding the
 *  same Property at multiple places is not allowed.
 */
class Property
{
public:

  /** A multilevel key for PropertySet.
   *
   *  This class breaks a multilevel key into subkeys.
   */
  class Key
  {
  public:
    Key(const std::string &composite_key);
    Key(const char *composite_key);
    Key(const Key &key, const Key *subkey = 0);
    Key(const std::string &key, const Key &subkey);
    Key &operator=(const Key &key);
    ~Key ();
    bool operator==(const Key &other) const;

    /// return the current part of a key
    inline const std::string &getKey () const { return _key; }

    /// return the next part of the key, or 0 if there is none
    inline const Key *getSubkey () const { return _subkey; }

    /// check if key is empty
    inline bool isEmpty() const { return _key.empty() && (_subkey==0); }

    /// get the full name of this key.
    std::string toString() const;

  private:
    void _assign(const std::string &composite_key);
    void _assign(const Key &key);
    std::string _key;
    Key *_subkey;
  };

  /// Base for all exceptions thrown by Property and subclasses
  class Exception : public std::logic_error
  {
  public:
    Exception(const std::string &msg, const Key &k) 
    : std::logic_error(msg), key(k) {}
    virtual ~Exception() throw() {}
    Key key;
  };

  /// Thrown if a conversion failed
  class ConversionFailed : public Exception
  {
  public:
    ConversionFailed(const std::string &reason)
    : Exception("Property::ConversionFailed: " + reason, "") {}
    virtual ~ConversionFailed() throw() {}
  };

  /// SubProperty not found
  class NoSuchProperty: public Exception
  {
  public:
    NoSuchProperty(const Key &k)
    : Exception("PropertySet::NoSuchProperty", k) {}
    virtual ~NoSuchProperty() throw() {}
  };

  /// a Node in the property didn't have the expected type.
  class WrongType: public Exception
  {
  public:
    WrongType(const Key &k)
    : Exception ("PropertySet::WrongType",k) {}
    virtual ~WrongType() throw() {}
  };

  /** Handler to receive notifications when a property changes.
   */
  class ChangeHandler
  {
  public:
    virtual ~ChangeHandler() {}
    
    virtual void propertyAdded(const Property *origin, const Key *key) {}

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
    virtual void propertyChanged(const Property *origin, const Key *key) {}
    
    virtual void propertyRemoved(const Property *origin, const Key *key) {}

    /** 
     * Called when the property at which this handler is registered is being destroyed.
     */
    virtual void propertyDestroyed(const Property *origin) {}
  };

  /** Create a Property.
   * */
  Property();

  /// dtor
  virtual ~Property();

  /**
    Create a polymorphic copy of this property.
   */
  virtual Property *clone() const = 0;

  /**
    Return the number of sets in this property element.
   */
  virtual unsigned int num_sets() const = 0;

  /**
    Return the number of items in this property element.
   */
  virtual unsigned int num_items() const = 0;

  /** Add a ChangeHandler.
   *
   *  @param handler that will be notified when the value
   *                 of a property changes.
   */
  inline void addChangeHandler(ChangeHandler *handler)
  { 
    _listeners.insert(handler);
  }

  /** Remove a ChangeHandler.
   *
   *  @param handler that would be notified when the value
   *                 of a property changed.
   */
  inline void removeChangeHandler(ChangeHandler *handler)
  { 
    _listeners.erase(handler);
  }

  /** Save the property to a stream.
   *
   *  Prints the property(tree) in xml format.
   */
  virtual void save_xml(std::ostream &os, const std::string &indent = "") const = 0;

  /** Print in a human readable format
   */
  virtual void print(std::ostream &os = std::cout, const std::string &indent = "") const = 0;

protected:

  void _notify_property_added(const Key &subkey);

  /** Notify listener when a change occures.
   *
   *  propagates change notifications up to the root
   */
  void _notify_property_changed(const Key *subkey = 0);

  void _notify_property_removed(const Key &subkey);

private:
  std::set<ChangeHandler*> _listeners;
};

inline std::ostream &operator<<(std::ostream &os, const Property &p)
{
  p.print(os);
  return os;
}

} // namespace Smart

#endif
