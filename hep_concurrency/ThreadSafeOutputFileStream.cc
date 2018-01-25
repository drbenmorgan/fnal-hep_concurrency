#include "hep_concurrency/ThreadSafeOutputFileStream.h"

#include <sstream>

namespace hep {
  namespace concurrency {

    ThreadSafeOutputFileStream::ThreadSafeOutputFileStream(
      std::string const& name)
      : file_{name}
    {}

    ThreadSafeOutputFileStream::~ThreadSafeOutputFileStream()
    {
      std::string tmp;
      while (waitingMessages_.try_pop(tmp)) {
        file_ << tmp;
      }
      file_.close();
    }

    void
    ThreadSafeOutputFileStream::write(std::string&& msg)
    {
      waitingMessages_.push(std::move(msg));
      bool expected{false};
      if (msgBeingLogged_.compare_exchange_strong(expected, true)) {
        std::string tmp;
        while (waitingMessages_.try_pop(tmp)) {
          file_ << tmp;
        }
        msgBeingLogged_.store(false);
      }
    }

  } // concurrency
} // hep
