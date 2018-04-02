#include "hep_concurrency/tsan.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <x86intrin.h>

namespace hep {
  namespace concurrency {

    int intentionalDataRace_{0};

    pthread_t
    getThreadID()
    {
      pthread_t const tid = pthread_self();
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
