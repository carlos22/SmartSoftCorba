// --------------------------------------------------------------------------
//
//  Copyright (C) 2003 Boris Kluge
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Basic Communication Classes".
//  It provides basic standardized data types for communication between
//  different components in the mobile robotics context. These classes
//  are designed to be used in conjunction with the SmartSoft Communication
//  Library.
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
//  (partly based on work by Christian Schlegel and Pablo d'Angelo)
//
// --------------------------------------------------------------------------

#ifndef SMART_COMM_SHARED_MEMORY_HH
#define SMART_COMM_SHARED_MEMORY_HH

#include <string>
#include <iostream>

#include <ace/Shared_Memory_SV.h>
#include <ace/SV_Semaphore_Complex.h>
#include <tao/corba.h>

namespace Smart {

/**
  Implementation of the shared memory communication class.
  This base class deals with untyped (void pointer) shared memory,
  the inheriting template instances deal with type conversion.
  The base class implements reference counting, such that you will
  have to allocate the shared memory once by calling \c alloc(), and the
  last instance referencing this shared memory segment will remove it from
  within its destructor.
  \sa CommSharedMemory
 */
class CommSharedMemoryBase
{
public:
  /**
    Destruction of instances is allowed to anybody.
   */
  ~CommSharedMemoryBase();

  /**
    Convert the reference to the shared memory segment into a CORBA::Any object.
    Note that you may run into problems if you delete the CommSharedMemoryBase object
    after converting it to CORBA::Any and before the CORBA::Any is converted back
    to a CommSharedMemoryBase object, since the reference counter will drop to zero
    then. Ensure that you have a local copy for the period of communication with
    other components.
   */
  void get(CORBA::Any &a) const;

  /**
    Convert a CORBA::Any object into a reference to a shared memory segment.
    See \c get(CORBA::Any&) for some problems which may arise.
    If the shared memory segment to which the source of the CORBA::Any object
    refered has been deleted (e.g. if the last CommSharedMemoryBase object refering
    to it ceased to exist), its shared memory key may have been reused such that we 
    would succeed to attach to a wrong segment. To avoid this, at the start of the
    shared memory segments created by CommSharedMemoryBase a random value is stored,
    and a copy of it is transmitted within the CORBA::Any object. If the values mismatch,
    we will refuse to attach to the shared memory segment.
   */
  void set(const CORBA::Any &a);

  /**
    Ensure exclusive access to the shared memory segment.
    Note that the underlying semaphore acquire will not fail due to a
    destruction of the corresponding semaphore, since this instance
    ensures a reference counter greater than zero, which prevents the
    semaphore from being removed.
   */
  void lock();

  /**
    Try to get exclusive access to the shared memory segment.
    Return 0 on success, -1 if the segment is locked by another thread or process.
   */
  int trylock();

  /**
    Release exclusive access to the shared memory segment.
   */
  void unlock();

  /**
    Check if we are attached to shared memory.
   */
  inline bool is_attached() const { return (shm!=0); }

  /**
    Get the attach count of the segment.
   */
  inline unsigned int get_ref_count() const
  {
    if(management_fields) return management_fields->ref_count;
    return 0;
  }

  /**
    Print information about this shared memory object.
   */
  void print(std::ostream &os = std::cout) const;

protected:
  /**
    Create a null reference to not yet existing shared memory.
   */
  CommSharedMemoryBase();

  /**
    Create a reference to the same shared memory segment as \a source.
   */
  CommSharedMemoryBase(const CommSharedMemoryBase &source);

  /**
    Release the currently referenced shared memory segment (possibly destroying it),
    and refer to the segment \a source refers to.
   */
  CommSharedMemoryBase &operator=(const CommSharedMemoryBase &source);

  /**
    Allocate a new data block of size \a size in shared memory.
    The shared memory where we are currently refering to is released
    and eventually removed.
    On success, zero is returned, and -1 on failure.
   */
  int alloc(size_t size);

  void print_data(std::ostream &os = std::cout) const;

  /**
    Shared memory management fields that prepend the user data
    in the shared memory segment.
   */
  struct ManagementFields
  {
    /**
      A random value.
     */
    long int random_value;
    
    /**
      The reference counter.
     */
    unsigned int ref_count;
  };
  
  /**
    A pointer to the user data in shared memory.
    This pointer is zero if there is no shared memory attached.
    Since there is management information (for example the reference counter)
    at the beginning of the shared memory segment, this pointer will point a little bit
    behind the actual start of the shared memory segment.
   */
  void *shm;

  /**
    If we are attached, this pointer refers to the management data in the shared
    memory segment.
    \sa ManagementFields
   */
  ManagementFields *management_fields;
  
private:
  /**
    Implementation of attaching to existing semaphore and shared memory.
    This method assumes being detached (already or still) when called.
   */
  int _attach(key_t shm_key, size_t shm_size, key_t sem_key, long random_value);
  
  /**
    Detach from shared memory, if attached.
    If this was the last reference to the shared memory, this method
    removes the shared memory segment.
   */
  void _detach();

#ifndef ACE_LACKS_SYSV_SHMEM
  key_t  _shm_key;
  size_t _shm_size;

  key_t  _sem_key;

  /**
    A random value that has to match the random value in the management fields
    of the shared memory. If not, we're attached to the wrong shared memory segment.
    This can happen if a shared memory segment is removed and its key is reused while
    an object refering to it exists only as a CORBA::Any (during that time, the reference
    count drops to zero).
   */
  unsigned int _random_value;
  
  ACE_Shared_Memory_SV _ace_shm;
  ACE_SV_Semaphore_Complex _ace_mutex;

  static const key_t _key_base;
#endif

  long int _random() const;
};

inline std::ostream &operator<<(std::ostream &os, const CommSharedMemoryBase &shm)
{
  shm.print(os); return os;
}

/**
  Communication class template which helps you to use shared memory
  where copying the data between components is infeasible.

  Shared memory undermines the communication concepts of SmartSoft:
  shared memory equals passing data by reference, whereby SmartSoft
  passes data by value. Therefore, this class shall be used only if
  it is the only feasible alternative.
  
  The type \c T is required to provide a method
  @code
    static inline std::string identifier() { return "Type_identifier"; }
  @endcode
  which returns a unique identifier for type T.

  If your application exits without properly calling destructors (e.g. if
  the application is killed by a signal), some IPC ressources possibly will
  not be removed properly. On UNIX systems, you can use the \c ipcs command 
  to display existing IPC ressources and \c ipcrm to remove IPC ressources which
  are not needed any more.
 */
template<class T>
class CommSharedMemory: public CommSharedMemoryBase
{
public:
  /**
    Create an instance that is not (yet) attached to shared memory.
   */
  inline CommSharedMemory()
  {
  }
  
  /**
    Create an instance that is attached to the same shared memory as \a source.
   */
  inline CommSharedMemory(const CommSharedMemory &source)
  : CommSharedMemoryBase(source)
  {
  }
  
  /**
    Attach to the same shared memory as \a source.
    If necessary, detach from shared memory before, possibly removing it.
   */
  inline CommSharedMemory &operator=(const CommSharedMemory &source)
  {
    CommSharedMemoryBase::operator=(source);
    return *this;
  }

  inline void get(CORBA::Any &a) const { CommSharedMemoryBase::get(a); }
  inline void set(const CORBA::Any &a) { CommSharedMemoryBase::set(a); }

  /**
    Required by SmartSoft.
    The type \c T is required to provide this method, too.
    Theroretically, we could use \c typeid(T).name(), but the returned 
    strings depend on the compiler (for example, returned strings may be 
    different for gcc2.95.2 and gcc3.2).
   */
  static inline std::string identifier() 
  {
    return std::string("CommSharedMemory<") + T::identifier() + std::string(">"); 
  }

  /**
    Create a shared memory segment for an instance of \c T.
    References to the segment are counted and it will be remove automatically
    as soon as the counter drops to zero.
   */
  inline int alloc() { return CommSharedMemoryBase::alloc(sizeof(T)); }

  /**
    Access method to get a pointer to the data from a const instance.
   */
  inline const T *get() const { return reinterpret_cast<const T*>(shm); }

  /**
    Access method to get a reference to the data from a const instance.
    Thanks to this operator you can use this class the same way you use a 
    pointer to \c const T
    @code
      struct Foo 
      {
        // you must not use dynamic data types here!
        int i;
        // ...
        static inline std::string identifier() { return "Foo"; }
      };

      std::ostream &operator<<(std::ostream &os, const Foo &f)
      {
        os << "Foo(" << f.i;
        //...
        return os << ")";
      }

      // ...

      Smart::CommSharedMemory<Foo> foo = ... // get it from somewhere else
      if(foo.is_attached())
      {
        // the operator is used here:
        std::cout << "Foo=" << *foo << std::endl;
      }
    @endcode
   */
  inline const T &operator*() const { return *(get()); }

  /**
    Access method to get a pointer to the data from a const instance.
    Thanks to this operator you can use this class the same way you use 
    a pointer to \c const T
    @code
      struct Foo 
      {
        // don't use dynamic data types!
        int i;
        char s[20];
        static inline std::string identifier() { return "Foo"; }
      };
      // ...

      void bar(const CommSharedMemory<Foo> &foo)
      {
        int i = 0;
        if(foo.is_attached())
        {
           i = foo->i;
        }
        // ...
      }
    @endcode
   */
  inline const T *operator->() const { return get(); }

  /**
    Method to get a pointer to the shared memory data.
   */
  inline T *get() { return reinterpret_cast<T*>(shm); }

  /**
    Method to access the shared memory data.
    Thanks to this operator you can use this class the same way you use a 
    pointer to \c T
    @code
      struct Foo 
      {
        // you must not use dynamic data types here!
        int i;
        // ...
        static inline std::string identifier() { return "Foo"; }
      };

      std::istream &operator>>(std::istream &is, Foo &f)
      {
        is >> f.i;
        // ...
        return is;
      }

      // ...

      Smart::CommSharedMemory<Foo> foo;
      if(foo.alloc()==0)
      {
        // the operator is used here:
        std::cout << "Input a foo:";
        std::cin >> *foo;
      }
    @endcode
   */
  inline T &operator*() { return *(get()); }

  /**
    Method to access the data.
    Thanks to this operator you can use this class the same way you use 
    a pointer to \c T
    @code
      struct Foo 
      {
        // don't use dynamic data types!
        int i;
        char s[20];
        static inline std::string identifier() { return "Foo"; }
      };
      // ...
      CommSharedMemory<Foo> foo;
      if(foo.alloc()==0)
      {
        foo->i = 42;
        strcpy(foo->s, "The answer");
      }
    @endcode
   */
  inline T *operator->() { return get(); }

  inline void print(std::ostream &os = std::cout) const
  {
    os << "CommSharedMemory<" << T::identifier() << ">(";
    CommSharedMemoryBase::print_data(os);
    os << ")";
  }
};

template<class T>
inline std::ostream &operator<<(std::ostream &os, const CommSharedMemory<T> &shm)
{
  shm.print(os); return os;
}

} // namespace Smart

#endif
