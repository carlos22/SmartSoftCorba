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

#ifndef SMART_QT_QUERY_CLIENT_HH
#define SMART_QT_QUERY_CLIENT_HH

#include "smartSoft.hh"
#include "smartQtSignallingInterface.hh"

namespace Smart {

/**
  An adapter from the CHS::QueryClient to Qt.
 */
template<class Request, class Answer>
class QtQueryClient: 
  public CHS::SmartTask, 
  public CHS::QueryClient<Request,Answer>,
  public QtSignallingInterface
{
public:
  QtQueryClient(CHS::SmartComponent *component, const std::string &port, CHS::WiringSlave* slave, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::QueryClient<Request,Answer>(component, port, slave),
    QtSignallingInterface(client_id),
    _request_semaphore(0)
  {
    this->open(); // start this->svc() in a separate thread
  }

  QtQueryClient(CHS::SmartComponent *component, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::QueryClient<Request,Answer>(component),
    QtSignallingInterface(client_id),
    _request_semaphore(0)
  {
    this->open(); // start this->svc() in a separate thread
  }

  QtQueryClient(CHS::SmartComponent *component, const std::string &server, const std::string &service, unsigned int client_id = 0) throw(CHS::SmartError)
  : CHS::QueryClient<Request,Answer>(component,server,service),
    QtSignallingInterface(client_id),
    _request_semaphore(0)
  {
    this->open(); // start this->svc() in a separate thread
  }

  ~QtQueryClient() throw()
  {
    this->cancel();
  }
  
  int svc()
  {
    int ret = 0;
    _mutex.acquire();
    _cancel = false;
    while(!_cancel)
    {
      _mutex.release();
      _request_semaphore.acquire();
      _mutex.acquire();
      if(!_cancel) 
      {
        RequestQueueEntry entry = _request_queue.front();
        if(entry.discard)
        {
          _mutex.release();
          CHS::QueryClient<Request,Answer>::queryDiscard(entry.id);
          _mutex.acquire();
        }
        else
        {
          _mutex.release();
          Answer ans;
          CHS::StatusCode status = CHS::QueryClient<Request,Answer>::queryReceiveWait(entry.id,ans);
          _mutex.acquire();
          entry = _request_queue.front();
          if(!entry.discard)
          {
            if(status==CHS::SMART_OK)
            {
              _answer_map[entry.id] = ans;
              signal_new_data();
            }
            else
            {
              ret = -1;
              _cancel = true;
            }
          }
        }
        _request_queue.pop_front();
      }
    }
    while(!_request_queue.empty())
    {
      CHS::QueryClient<Request,Answer>::queryDiscard(_request_queue.front().id);
      _request_queue.pop_front();
    }
    _mutex.release();
    return ret;
  }
    
  inline CHS::StatusCode queryRequest(const Request &request, CHS::QueryId &id) throw()
  {
    CHS::StatusCode status;
    _mutex.acquire();
    if(_cancel)
    {
      status = CHS::SMART_ERROR;
    }
    else
    {
      status = CHS::QueryClient<Request,Answer>::queryRequest(request,id);
      if(status==CHS::SMART_OK)
      {
        _request_queue.push_back(RequestQueueEntry(id));
        _request_semaphore.release();
      }
    }
    _mutex.release();
    return status;
  }

  inline CHS::StatusCode queryReceive(const CHS::QueryId id, Answer &answer) throw()
  {
    CHS::StatusCode status = CHS::SMART_ERROR;

    _mutex.acquire();

    /////////////////////////////////////////////
    // try to find ID in answer map
    //
    typename std::map<CHS::QueryId,Answer>::iterator ans_iter = _answer_map.find(id);
    if(ans_iter!=_answer_map.end())
    {
      answer = (*ans_iter).second;
      _answer_map.erase(ans_iter);
      status = CHS::SMART_OK;
    }
    else if(!_cancel)
    {
      /////////////////////////////////////////////
      // try to find ID in request queue
      //
      status = CHS::SMART_WRONGID; // pessimistic initial assumption
      typename std::list<RequestQueueEntry>::iterator req_iter = _request_queue.begin();
      while(req_iter!=_request_queue.end())
      {
        if((*req_iter).id==id)
        {
          if(!(*req_iter).discard)
          {
            status = CHS::SMART_NODATA;
          }
          break;
        }
        ++req_iter;
      }
    }
    // status is ERROR if cancelled

    _mutex.release();

    return status;
  }

  /**
    Intercept blocking method.
   */
  CHS::StatusCode queryReceiveWait(const CHS::QueryId id, Answer &answer) throw()
  {
    std::cerr << "WARNING: tried to call blocking method in Smart::QtQueryClient" << std::endl;
    return CHS::SMART_ERROR;
  }

  CHS::StatusCode queryDiscard(const CHS::QueryId id) throw()
  {
    CHS::StatusCode status = CHS::SMART_ERROR;

    _mutex.acquire();

    /////////////////////////////////////////////
    // try to find ID in answer map
    //
    typename std::map<CHS::QueryId,Answer>::iterator ans_iter = _answer_map.find(id);
    if(ans_iter!=_answer_map.end())
    {
      _answer_map.erase(ans_iter);
      status = CHS::SMART_OK;
    }
    else
    {
      /////////////////////////////////////////////
      // try to find ID in request queue
      //
      _mutex.acquire();
      typename std::list<RequestQueueEntry>::iterator req_iter = _request_queue.begin();
      while(req_iter!=_request_queue.end())
      {
        if((*req_iter).id==id)
        {
          (*req_iter).discard = true;
          status = CHS::SMART_OK;
          break;
        }
        ++req_iter;
      }
      _mutex.release();
    }
    
    _mutex.release();

    return status;
  }

  /**
    Cancel waiting for new data and force the thread to exit.
   */
  inline void cancel() throw()
  {
    _mutex.acquire();
    if(!_cancel)
    {
      _cancel = true;
      _request_semaphore.release();
    }
    _mutex.release();
  }
  
private:

  struct RequestQueueEntry
  {
    inline RequestQueueEntry() : id(0), discard(false) {}
    inline RequestQueueEntry(CHS::QueryId i) : id(i), discard(false) {}
    CHS::QueryId id;
    bool discard;
  };

  std::list<RequestQueueEntry> _request_queue;
  std::map<CHS::QueryId,Answer> _answer_map;
  bool _cancel;

  CHS::SmartSemaphore _request_semaphore;
  CHS::SmartMutex _mutex;
};

} // namespace Smart

#endif
