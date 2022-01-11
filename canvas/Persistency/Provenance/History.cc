#include "canvas/Persistency/Provenance/History.h"
// vim: set sw=2 expandtab :

namespace art {

  ProcessHistoryID const&
  History::processHistoryID() const noexcept
  {
    return processHistoryID_;
  }

  void
  History::setProcessHistoryID(ProcessHistoryID const& phid)
  {
    processHistoryID_ = phid;
  }

} // namespace art
