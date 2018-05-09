#include "hep_concurrency/tsan.h"

#include <x86intrin.h>

using namespace std;

namespace hep {
  namespace concurrency {

    int intentionalDataRace_{0};

    thread_local int ignoreBalance_{0};

    thread::id
    getThreadID()
    {
      auto tid = this_thread::get_id();
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
