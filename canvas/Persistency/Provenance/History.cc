#include "canvas/Persistency/Provenance/History.h"
// vim: set sw=2 expandtab :

namespace art {

History::
~History()
{
}

History::
History()
  : eventSelections_{}
  , branchListIndexes_{}
  , processHistoryID_{}
{
}

History::size_type
History::
size() const
{
  return eventSelections_.size();
}

void
History::
addEventSelectionEntry(EventSelectionID const& eventSelection)
{
  eventSelections_.push_back(eventSelection);
}

void
History::
addBranchListIndexEntry(BranchListIndex const& branchListIndex)
{
  branchListIndexes_.push_back(branchListIndex);
}

EventSelectionID const&
History::
getEventSelectionID(History::size_type i) const
{
  return eventSelections_[i];
}

EventSelectionIDVector const&
History::
eventSelectionIDs() const
{
  return eventSelections_;
}

ProcessHistoryID const&
History::
processHistoryID() const
{
  return processHistoryID_;
}

void
History::
setProcessHistoryID(ProcessHistoryID const& phid) const
{
  processHistoryID_ = phid;
}

BranchListIndexes const&
History::
branchListIndexes() const
{
  return branchListIndexes_;
}

BranchListIndexes&
History::
branchListIndexes()
{
  return branchListIndexes_;
}

} // namespace art

