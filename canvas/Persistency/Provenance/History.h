#ifndef canvas_Persistency_Provenance_History_h
#define canvas_Persistency_Provenance_History_h
// vim: set sw=2 expandtab :

//
// Class History represents the processing history of a single Event.
// It includes ordered sequences of elements, each of which contains
// information about a specific 'process' through which the Event has
// passed, with earlier processes at the beginning of the sequence.
//

#include "canvas/Persistency/Provenance/BranchListIndex.h"
#include "canvas/Persistency/Provenance/EventSelectionID.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"

namespace art {

  class History {
  public:
    using size_type = std::size_t;

    ~History();
    History();
    History(History const&) = default;
    History(History&&) = delete;
    History& operator=(History const&) = delete;
    History& operator=(History&&) = delete;

    size_type size() const noexcept;

    void addEventSelectionEntry(EventSelectionID const& eventSelection);
    void addBranchListIndexEntry(BranchListIndex const& branchListIndex);

    EventSelectionID const& getEventSelectionID(size_type i) const noexcept;
    EventSelectionIDVector const& eventSelectionIDs() const noexcept;
    ProcessHistoryID const& processHistoryID() const noexcept;

    BranchListIndexes const& branchListIndexes() const noexcept;
    BranchListIndexes& branchListIndexes() noexcept;
    void setProcessHistoryID(ProcessHistoryID const& phid);

  private:
    EventSelectionIDVector eventSelections_;
    BranchListIndexes branchListIndexes_;
    mutable ProcessHistoryID processHistoryID_;
  };

} // namespace art

#endif /* canvas_Persistency_Provenance_History_h */

// Local Variables:
// mode: c++
// End:
