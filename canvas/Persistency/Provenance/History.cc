#include "canvas/Persistency/Provenance/History.h"
// vim: set sw=2 expandtab :

namespace art {

  History::~History() = default;
  History::History() = default;

  History::size_type
  History::size() const noexcept
  {
    return eventSelections_.size();
  }

  void
  History::addEventSelectionEntry(EventSelectionID const& eventSelection)
  {
    eventSelections_.push_back(eventSelection);
  }

  void
  History::addBranchListIndexEntry(BranchListIndex const& branchListIndex)
  {
    branchListIndexes_.push_back(branchListIndex);
  }

  EventSelectionID const&
  History::getEventSelectionID(History::size_type const i) const noexcept
  {
    return eventSelections_[i];
  }

  EventSelectionIDVector const&
  History::eventSelectionIDs() const noexcept
  {
    return eventSelections_;
  }

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

  BranchListIndexes const&
  History::branchListIndexes() const noexcept
  {
    return branchListIndexes_;
  }

  BranchListIndexes&
  History::branchListIndexes() noexcept
  {
    return branchListIndexes_;
  }

} // namespace art
