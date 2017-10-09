#include "canvas/Utilities/DebugMacros.h"
// vim: set sw=2 expandtab :

#include <cstdlib>

namespace art {

  DebugValue::DebugValue()
    : cvalue_(getenv("PROC_DEBUG"))
    , value_((cvalue_ == nullptr) ? 0 : atoi(cvalue_))
  {}

  DebugTasksValue::DebugTasksValue()
    : cvalue_(getenv("ART_DEBUG_TASKS"))
    , value_((cvalue_ == nullptr) ? 0 : atoi(cvalue_))
  {}

  DebugValue debugit;
  DebugTasksValue debugTasks;

} // namespace art
