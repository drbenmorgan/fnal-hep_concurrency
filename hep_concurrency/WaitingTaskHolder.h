#ifndef hep_concurrency_WaitingTaskHolder_h
#define hep_concurrency_WaitingTaskHolder_h
// -*- C++ -*-
//
// Package:     FWCore/Concurrency
// Class  :     WaitingTaskHolder
// 
/**\class WaitingTaskHolder WaitingTaskHolder.h "WaitingTaskHolder.h"

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  FWCore
//         Created:  Fri, 18 Nov 2016 20:30:42 GMT
//

// system include files
#include <cassert>

// user include files
#include "hep_concurrency/WaitingTask.h"

// forward declarations

namespace hep {
namespace concurrency {
  class WaitingTaskHolder
  {
    
  public:
    WaitingTaskHolder():
    m_task(nullptr) {}
    
    explicit WaitingTaskHolder(hep::concurrency::WaitingTask* iTask):
    m_task(iTask)
    {m_task->increment_ref_count();}
    ~WaitingTaskHolder() {
      if(m_task) {
        doneWaiting(std::exception_ptr{});
      }
    }

    WaitingTaskHolder(const WaitingTaskHolder& iHolder) :
    m_task(iHolder.m_task) {
      m_task->increment_ref_count();
    }

    WaitingTaskHolder(WaitingTaskHolder&& iOther) :
    m_task(iOther.m_task) {
      iOther.m_task = nullptr;
    }
    
    WaitingTaskHolder& operator=(const WaitingTaskHolder& iRHS) {
      WaitingTaskHolder tmp(iRHS);
      std::swap(m_task, tmp.m_task);
      return *this;
    }
    
    // ---------- const member functions ---------------------
    
    // ---------- static member functions --------------------
    
    // ---------- member functions ---------------------------
    void doneWaiting(std::exception_ptr iExcept) {
      if(iExcept) {
        m_task->dependentTaskFailed(iExcept);
      }
      if(0==m_task->decrement_ref_count()){
        tbb::task::spawn(*m_task);
      }
      m_task = nullptr;
    }
    
  private:
    
    // ---------- member data --------------------------------
    WaitingTask* m_task;
  };
} // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_WaitingTaskHolder_h */
