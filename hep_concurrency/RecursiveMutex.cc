#include "hep_concurrency/RecursiveMutex.h"
// vim: set sw=2 expandtab :

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

//#include <sys/syscall.h>
//#include <unistd.h>

using namespace std;

namespace hep {
  namespace concurrency {

    // namespace {
    //  thread::id const default_thread_id;
    //} // unnamed namespace

    mutex* RecursiveMutex::heldMutex_{nullptr};
    // map<thread::id const, vector<RecursiveMutex*>>*
    // RecursiveMutex::held_{nullptr};
    map<uint64_t const, vector<RecursiveMutex*>>* RecursiveMutex::held_{
      nullptr};

    void
    RecursiveMutex::startup()
    {
      if (heldMutex_ == nullptr) {
        heldMutex_ = new mutex;
        // held_ = new map<thread::id const, vector<RecursiveMutex*>>;
        held_ = new map<uint64_t const, vector<RecursiveMutex*>>;
      }
    }

    void
    RecursiveMutex::shutdown()
    {
      ANNOTATE_THREAD_IGNORE_BEGIN;
      delete held_;
      held_ = nullptr;
      delete heldMutex_;
      heldMutex_ = nullptr;
      ANNOTATE_THREAD_IGNORE_END;
    }

    struct RecursiveMutexCleaner {
      ~RecursiveMutexCleaner() { RecursiveMutex::shutdown(); }
      RecursiveMutexCleaner() { RecursiveMutex::startup(); }
    } recursiveMutexCleaner_;

    RecursiveMutex::~RecursiveMutex() {}

    RecursiveMutex::RecursiveMutex(std::string const& name /* = "" */)
      //: owner_{default_thread_id}, lockCount_{0}, name_{name}
      : owner_{0}, lockCount_{0}, name_{name}
    {}

    void
    RecursiveMutex::lock(string const& opName)
    {
      (void)opName;
      unsigned tsc_begin_cpuidx = 0;
      auto tsc_begin = getTSCP(tsc_begin_cpuidx);
      unsigned tsc_end_cpuidx = tsc_begin_cpuidx;
      auto tsc_end = tsc_begin;
      auto tid = getThreadID();
      unique_lock<mutex> cvLock{cvMutex_, defer_lock};
      cvLock.lock();
      cv_.wait(cvLock, [this, tid] {
        // Wait until either unowned, or I am the owner.
        // if ((owner_ != default_thread_id) && (owner_ != tid))
        if ((owner_ != 0) && (owner_ != tid)) {
          return false;
        }
        return true;
      });
      ++lockCount_;
      if (lockCount_ == 1) {
        mutex_.lock();
        tsc_end = getTSCP(tsc_end_cpuidx);
        owner_ = tid;
      }
      {
        lock_guard<mutex> heldSentry{*heldMutex_};
        if (lockCount_ == 1) {
          (*held_)[tid].push_back(this);
        }
        if (getenv("ART_DEBUG_RECURSIVE_MUTEX") != nullptr) {
          ostringstream buf;
          buf << "----->"
              << " " << setw(2) << right << (tsc_begin_cpuidx & 0xFF) << left
              << " " << setw(18) << right << tsc_begin << left << " " << setw(6)
              << right << tid << left << "           "
              << " RecursiveMutex::lock  :"
              << " addr: " << hex << &mutex_ << dec << " cnt: " << lockCount_;
          if (tsc_begin_cpuidx == tsc_end_cpuidx) {
            buf << " ticks: " << setw(9) << right << (tsc_end - tsc_begin)
                << left;
          } else {
            buf << " ticks: " << setw(9) << right << "NA" << left;
          }
          buf << " owner: " << setw(6) << right << owner_ << left << " "
              << setw(32) << name_ << " " << setw(32) << opName;
          bool first = true;
          for (auto const& tidAndvheld : *held_) {
            if ((*held_)[tidAndvheld.first].size() == 0) {
              continue;
            }
            if (first) {
              first = false;
            } else {
              buf << setw(195) << " ";
            }
            buf << " " << setw(6) << right << tidAndvheld.first << left;
            for (auto val : (*held_)[tidAndvheld.first]) {
              buf << " " << hex << val << dec << " " << val->name_;
            }
            buf << "\n";
          }
          if (first) {
            buf << "\n";
          }
          cerr << buf.str();
        }
      }
      cv_.notify_one();
    }

    void
    RecursiveMutex::unlock(string const& opName)
    {
      unsigned tsc_begin_cpuidx = 0;
      auto tsc_begin = getTSCP(tsc_begin_cpuidx);
      unique_lock<mutex> cvLock{cvMutex_};
      unsigned tsc_end_cpuidx = tsc_begin_cpuidx;
      auto tsc_end = getTSCP(tsc_end_cpuidx);
      auto tid = getThreadID();
      // if ((owner_ == default_thread_id) || (owner_ != tid))
      if ((owner_ == 0) || (owner_ != tid)) {
        // Either the mutex is unowned or is not owned by us, error.
        // if (owner_ == default_thread_id)
        if (owner_ == 0) {
          cerr << "Aborting on attempt to unlock the mutex when it is not "
                  "owned! tid: "
               << tid << "\n";
          abort();
        }
        cerr << "Aborting on attempt to unlock the mutex when we are not the "
                "owner! tid: "
             << tid << " owner_: " << owner_ << "\n";
        abort();
      }
      try {
        --lockCount_;
        if (lockCount_ == 0) {
          lock_guard<mutex> heldSentry{*heldMutex_};
          mutex_.unlock();
          // owner_ = default_thread_id;
          owner_ = 0;
          auto i = (*held_)[tid].size();
          while (i != 0) {
            --i;
            if ((*held_)[tid].at(i) == this) {
              if ((i + 1) != (*held_)[tid].size()) {
                {
                  ostringstream buf;
                  buf << "----->"
                      << " " << setw(2) << right << (tsc_begin_cpuidx & 0xFF)
                      << left << " " << setw(18) << right << tsc_begin << left
                      << " " << setw(6) << right << tid << left << "           "
                      << " RecursiveMutex::unlock:"
                      << " addr: " << hex << &mutex_ << dec
                      << " cnt: " << lockCount_;
                  if (tsc_begin_cpuidx == tsc_end_cpuidx) {
                    buf << " ticks: " << setw(9) << right
                        << (tsc_end - tsc_begin) << left;
                  } else {
                    buf << " ticks: " << setw(9) << right << "NA" << left;
                  }
                  buf << " owner: " << setw(6) << right << owner_ << left << " "
                      << setw(32) << name_ << " " << setw(32) << opName;
                  bool first = true;
                  for (auto const& tidAndvheld : *held_) {
                    if ((*held_)[tidAndvheld.first].size() == 0) {
                      continue;
                    }
                    if (first) {
                      first = false;
                    } else {
                      buf << setw(195) << " ";
                    }
                    buf << " " << setw(6) << right << tidAndvheld.first << left;
                    for (auto val : (*held_)[tidAndvheld.first]) {
                      buf << " " << hex << val << dec << " " << val->name_;
                    }
                    buf << "\n";
                  }
                  if (first) {
                    buf << "\n";
                  }
                  cerr << buf.str();
                }
                ostringstream buf;
                buf << "Aborting on attempt to unlock the mutex "
                    << "when it is not the most recently "
                    << "acquired!"
                    << " tid: " << tid << " &mutex_: " << hex << &mutex_ << dec
                    << " i: " << i
                    << " (*held_)[tid].size(): " << (*held_)[tid].size()
                    << "\n";
                cerr << buf.str();
                abort();
              }
              (*held_)[tid].erase((*held_)[tid].begin() + i);
              break;
            }
          }
          if (getenv("ART_DEBUG_RECURSIVE_MUTEX") != nullptr) {
            ostringstream buf;
            buf << "----->"
                << " " << setw(2) << right << (tsc_begin_cpuidx & 0xFF) << left
                << " " << setw(18) << right << tsc_begin << left << " "
                << setw(6) << right << tid << left << "           "
                << " RecursiveMutex::unlock:"
                << " addr: " << hex << &mutex_ << dec << " cnt: " << lockCount_;
            if (tsc_begin_cpuidx == tsc_end_cpuidx) {
              buf << " ticks: " << setw(9) << right << (tsc_end - tsc_begin)
                  << left;
            } else {
              buf << " ticks: " << setw(9) << right << "NA" << left;
            }
            buf << " owner: " << setw(6) << right << owner_ << left << " "
                << setw(32) << name_ << " " << setw(32) << opName;
            bool first = true;
            for (auto const& tidAndvheld : *held_) {
              if ((*held_)[tidAndvheld.first].size() == 0) {
                continue;
              }
              if (first) {
                first = false;
              } else {
                buf << setw(195) << " ";
              }
              buf << " " << setw(6) << right << tidAndvheld.first << left;
              for (auto val : (*held_)[tidAndvheld.first]) {
                buf << " " << hex << val << dec << " " << val->name_;
              }
              buf << "\n";
            }
            if (first) {
              buf << "\n";
            }
            cerr << buf.str();
          }
        }
      }
      catch (...) {
        cv_.notify_one();
        throw;
      }
      cv_.notify_one();
    }

    RecursiveMutexSentry::~RecursiveMutexSentry()
    {
      mutex_->unlock(*name_);
      delete name_;
      name_ = nullptr;
      mutex_ = nullptr;
    }

    RecursiveMutexSentry::RecursiveMutexSentry(RecursiveMutex& mutex,
                                               string const& name)
      : mutex_{&mutex}, name_{new string(name)}
    {
      mutex_->lock(*name_);
    }

  } // namespace concurrency
} // namespace hep
