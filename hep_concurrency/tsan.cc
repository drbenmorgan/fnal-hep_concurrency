#include "hep_concurrency/tsan.h"

#include <cstddef>
#include <cstdint>
//#include <thread>

#if defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(__MACH__) && defined(__APPPLE__)
#include <pthread.h>
#endif

#include <x86intrin.h>

using namespace std;

namespace hep {
  namespace concurrency {

    int intentionalDataRace_{0};

    thread_local int ignoreBalance_{0};

    // thread::id
    // getThreadID()
    //{
    //  auto tid = this_thread::get_id();
    //  return tid;
    //}

    uint64_t
    getThreadID()
    {
      uint64_t tid = 0;
#if defined(__linux__)
      tid = syscall(SYS_gettid);
#elif defined(__MACH__) && defined(__APPPLE__)
      // This is deprecated as of maxOS 10.12.
      // tid = syscall(SYS_thread_selfid);
      pthread_threadid_np(NULL, &tid);
#endif
      return tid;
    }

    unsigned long long
    getTSC()
    {
      return __rdtsc();
    }

    unsigned long long
    getTSCP(unsigned& cpuidx)
    {
      return __rdtscp(&cpuidx);
    }

  } // namespace concurrency
} // namespace hep
