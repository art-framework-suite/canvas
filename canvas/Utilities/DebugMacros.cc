#include "canvas/Utilities/DebugMacros.h"
// vim: set sw=2 expandtab :

#include <cstdlib>

namespace art {

  DebugValue::DebugValue()
  {
    cvalue_ = getenv("PROC_DEBUG");
    value_ = (cvalue_.load() == nullptr) ? 0 : atoi(cvalue_.load());
  }

  DebugValue debugit;

} // namespace art
