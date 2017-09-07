#ifndef canvas_Utilities_DebugMacros_h
#define canvas_Utilities_DebugMacros_h
// vim: set sw=2 expandtab :

#include "canvas/Utilities/fwd.h"

#include <iostream>

namespace art {

struct DebugValue {

  DebugValue();

  int
  operator()()
  {
    return value_;
  }

  char const*
  cvalue_;

  int
  value_;

};

struct DebugTasksValue {

  DebugTasksValue();

  int
  operator()()
  {
    return value_;
  }

  char const*
  cvalue_;

  int
  value_;

};

extern DebugValue debugit;
extern DebugTasksValue debugTasks;

}

#define FDEBUG(LEVEL) if ((LEVEL) <= art::debugit()) std::cerr
#define TDEBUG(LEVEL) if ((LEVEL) <= art::debugTasks()) std::cerr

#endif /* canvas_Utilities_DebugMacros_h */

// Local Variables:
// mode: c++
// End:
