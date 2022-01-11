#include "canvas/Persistency/Provenance/History.h"
// vim: set sw=2 expandtab :

namespace art {

  History::History() = default;

  History::History(ProcessHistoryID const& phid)
    : processHistoryID_{phid}
  {}

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
