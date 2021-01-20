#ifndef canvas_Utilities_DebugMacros_h
#define canvas_Utilities_DebugMacros_h
// vim: set sw=2 expandtab :

#include <atomic>
#include <iostream>

namespace art {
  struct DebugValue {
    DebugValue();

    int
    operator()()
    {
      return value_.load();
    }

    std::atomic<char const*> cvalue_;
    std::atomic<int> value_;
  };

  extern DebugValue debugit;
}

#define FDEBUG(LEVEL)                                                          \
  if ((LEVEL) <= art::debugit())                                               \
  std::cerr

#endif /* canvas_Utilities_DebugMacros_h */

// Local Variables:
// mode: c++
// End:
