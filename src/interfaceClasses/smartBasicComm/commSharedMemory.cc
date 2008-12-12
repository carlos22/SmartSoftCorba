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

#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>

#include "smartSoft.hh"

#include "commSharedMemory.hh"

#include "smartSharedMemoryC.hh"

using namespace Smart;

const key_t CommSharedMemoryBase::_key_base = 0x1000;

CommSharedMemoryBase::CommSharedMemoryBase()
: shm(0),
  management_fields(0),
  _shm_key(0),
  _shm_size(0),
  _sem_key(0),
  _random_value(0)
{
}

CommSharedMemoryBase::CommSharedMemoryBase(const CommSharedMemoryBase &source)
: shm(0),
  management_fields(0),
  _shm_key(0),
  _shm_size(0),
  _sem_key(0),
  _random_value(0)
{
  if(source.shm) _attach(source._shm_key, source._shm_size, source._sem_key, source.management_fields->random_value);
}

CommSharedMemoryBase::~CommSharedMemoryBase()
{
  _detach();
}

CommSharedMemoryBase &CommSharedMemoryBase::operator=(const CommSharedMemoryBase &source)
{
  _detach();
  if(source.shm) _attach(source._shm_key, source._shm_size, source._sem_key, source.management_fields->random_value);
  return *this;
}

void CommSharedMemoryBase::get(CORBA::Any &a) const
{
  SmartIDL::SharedMemory idl_shm;

  _ace_mutex.acquire();

  idl_shm.valid        = (shm!=0);
  idl_shm.shm_key      = _shm_key;
  idl_shm.shm_size     = _shm_size;
  idl_shm.sem_key      = _sem_key;
  idl_shm.random_value = _random_value;

  _ace_mutex.release();

  a <<= idl_shm;
}

void CommSharedMemoryBase::set(const CORBA::Any &a)
{
  // proper locking is done by _detach() and _attach()
  _detach();
  SmartIDL::SharedMemory *idl_shm;
  if(a >>= idl_shm)
  {
    if(idl_shm->valid) _attach(idl_shm->shm_key, idl_shm->shm_size, idl_shm->sem_key, idl_shm->random_value);
  }
}

int CommSharedMemoryBase::alloc(size_t size)
{
#ifndef ACE_LACKS_SYSV_SHMEM

  _detach();

  size += sizeof(ManagementFields);

  key_t sem_key = _key_base;
  while(true)
  {
    if(_ace_mutex.open(sem_key, ACE_SV_Semaphore_Complex::ACE_CREATE, 0, 1, IPC_EXCL | ACE_DEFAULT_FILE_PERMS)>=0)
    {
      break;
    }
    else
    {
      if((errno==ENOMEM) || (errno==ENOSPC))
      {
        perror("CommSharedMemoryBase::alloc() { _ace_mutex.open() }");
        return -1;
      }
      // for any other error (see semget(2)), we have to try 
      // again with a different key

      // sem_key is already in use, try next
      ++sem_key;

      // handle overflow, when key becomes negative or zero (less than _key_base in any case)
      if(sem_key<_key_base)
      {
        std::cerr << "CommSharedMemory: failed to find usable semaphore key." << std::endl;
        return -1;
      }
    }
  }

  key_t shm_key = _key_base;
  while(true)
  {
    if(_ace_shm.open(shm_key, size, ACE_Shared_Memory_SV::ACE_CREATE | IPC_EXCL)==0)
    {
      break;
    }
    else
    {
      if((errno==ENOMEM) || (errno==ENOSPC) || (errno==EINVAL))
      {
        perror("CommSharedMemoryBase::alloc() { _ace_shm.open() }");
        _ace_mutex.remove();
        return -1;
      }
      // for any other error (see shmget(2)), we have to try
      // again with a different key

      // shm_key already in use, try next
      ++shm_key;

      // handle overflow, when key becomes negative or zero (less than _key_base in any case)
      if(shm_key<_key_base)
      {
        std::cerr << "CommSharedMemory: failed to find usable shared memory key." << std::endl;
        _ace_mutex.remove();
        return -1;
      }
    }
  }

  char *p = reinterpret_cast<char*>(_ace_shm.malloc());
  if(p==0)
  {
    // failure mapping shared memory 
    _ace_mutex.remove();
    _ace_shm.remove();
    _ace_shm.close();
    return -1;
  }

  _shm_key = shm_key;
  _shm_size = size;
  _sem_key = sem_key;

  shm = p + sizeof(ManagementFields);

  management_fields = reinterpret_cast<ManagementFields*>(p);
  management_fields->ref_count = 1;
  management_fields->random_value = _random();
  _random_value = management_fields->random_value;
  
  _ace_mutex.release();  

  return 0;

#else

  return -1;

#endif
}

void CommSharedMemoryBase::lock()
{
#ifndef ACE_LACKS_SYSV_SHMEM
  if(shm)
  {
    _ace_mutex.acquire();
  }
#endif
}

int CommSharedMemoryBase::trylock()
{
#ifndef ACE_LACKS_SYSV_SHMEM
  if(shm)
  {
    return _ace_mutex.tryacquire();
  }
#endif
  return 0;
}

void CommSharedMemoryBase::unlock()
{
#ifndef ACE_LACKS_SYSV_SHMEM
  if(shm)
  {
    _ace_mutex.release();
  }
#endif
}

void CommSharedMemoryBase::print(std::ostream &os) const
{
  os << "CommSharedMemoryBase(";
  print_data(os);
  os << ")";
}

void CommSharedMemoryBase::print_data(std::ostream &os) const
{
#ifdef ACE_LACKS_SYSV_SHMEM
  os << "unsupported";
#else
  os << "shm_key=" << _shm_key << ",shm_size=" << _shm_size << ",sem_key=" << _sem_key;
  if(shm)
  {
    os << ",nref=" << management_fields->ref_count << ",rand=" << management_fields->random_value;
  }
#endif
}


int CommSharedMemoryBase::_attach(key_t shm_key, size_t shm_size, key_t sem_key, long random_value)
{
#ifndef ACE_LACKS_SYSV_SHMEM
  if(_ace_mutex.open(sem_key)!=0)
  {
    // failure
    return -1;
  }
  
  if(_ace_mutex.acquire()!=0)
  {
    // failure
    _ace_mutex.close();
    return -1;
  }

  if(_ace_shm.open(shm_key, shm_size)!=0)
  {
    // failure
    _ace_mutex.release();
    _ace_mutex.close();
    return -1;
  }

  char *p = reinterpret_cast<char*>(_ace_shm.malloc());
  if(p==0)
  {
    // failure
    _ace_mutex.release();
    _ace_mutex.close();
    _ace_shm.close();
    return -1;
  }

  management_fields = reinterpret_cast<ManagementFields*>(p);
  if(management_fields->random_value != random_value)
  {
    // failure
    std::cerr << "WARNING: shared memory random value mismatch." << std::endl;
    _ace_mutex.release();
    _ace_mutex.close();
    _ace_shm.close();
    management_fields = 0;
    return -1;
  }

  _shm_key = shm_key;
  _shm_size = shm_size;
  _sem_key = sem_key;
  _random_value = random_value;

  shm = p + sizeof(ManagementFields);
  management_fields->ref_count += 1;

  _ace_mutex.release();

  return 0;
  
#else

  return -1;
  
#endif
}

void CommSharedMemoryBase::_detach()
{
#ifndef ACE_LACKS_SYSV_SHMEM
  if(shm)
  {
    _ace_mutex.acquire();

    management_fields->ref_count -= 1;
    const bool remove = (management_fields->ref_count == 0);

    if(remove)
    {
      _ace_shm.remove();
      _ace_shm.close();
      _ace_mutex.remove();
    }
    else
    {
      _ace_shm.close();
      _ace_mutex.release();
      _ace_mutex.close();
    }

    _shm_key = 0;
    _shm_size = 0;
    _sem_key = 0;
    _random_value = 0;
    shm = 0;
    management_fields = 0;
  }
#endif
}


long int CommSharedMemoryBase::_random() const
{
  static bool started = false;
  static CHS::SmartMutex mutex;

  mutex.acquire();
  if(!started)
  {
    timeval tv;
    gettimeofday(&tv,0);
    srand(tv.tv_sec ^ tv.tv_usec);
    started = true;
  }
  const long int r = lrand48();
  mutex.release();
  return r;
}

