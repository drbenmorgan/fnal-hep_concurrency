#ifndef hep_concurrency_ThreadSafeOutputFileStream_h
#define hep_concurrency_ThreadSafeOutputFileStream_h

#include "tbb/concurrent_queue.h"

#include <atomic>
#include <fstream>
#include <string>

namespace hep {
  namespace concurrency {
    class ThreadSafeOutputFileStream {
    public:
      ThreadSafeOutputFileStream(std::string const& name);
      ~ThreadSafeOutputFileStream();

      void write(std::string&& msg);
      explicit operator bool() const { return static_cast<bool>(file_); }

    private:
      std::ofstream file_;
      std::atomic<bool> msgBeingLogged_ {false};
      tbb::concurrent_queue<std::string> waitingMessages_ {};
    };
  } // concurrency
} // hep

#endif /* hep_concurrency_ThreadSafeOutputFileStream_h */
