#include "hep_concurrency/tsan.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <x86intrin.h>

namespace hep {
  namespace concurrency {

    int intentionalDataRace_{0};

    long
    getThreadID()
    {
      long tid = 0;
#if defined(__linux__)
      tid = syscall(SYS_gettid);
#elif defined(__MACH__) && defined(__APPPLE__)
      tid = syscall(SYS_thread_selfid);
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
