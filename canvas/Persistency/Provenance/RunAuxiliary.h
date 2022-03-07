#ifndef canvas_Persistency_Provenance_RunAuxiliary_h
#define canvas_Persistency_Provenance_RunAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <iosfwd>
#include <set>

// Auxiliary run data that is persistent

namespace art {

  class RunAuxiliary {
  public:
    static constexpr BranchType branch_type = InRun;

    RunAuxiliary();

    RunAuxiliary(RunID const& theId,
                 Timestamp const& theTime,
                 Timestamp const& theEndTime);

    RunAuxiliary(RunNumber_t const& run,
                 Timestamp const& theTime,
                 Timestamp const& theEndTime);

    RunAuxiliary duplicateWith(Timestamp beginTime,
                               Timestamp endTime = {}) const;

    void write(std::ostream&) const;
    ProcessHistoryID const& processHistoryID() const noexcept;
    void setProcessHistoryID(ProcessHistoryID const&);
    unsigned rangeSetID() const noexcept;
    void setRangeSetID(unsigned const id) const;

    RunID const& id() const noexcept;
    RunID const& runID() const noexcept;
    void runID(RunID const&);
    RunNumber_t run() const noexcept;

    Timestamp const& beginTime() const noexcept;
    Timestamp const& endTime() const noexcept;

    bool mergeAuxiliary(RunAuxiliary const&);

  private:
    void mergeNewTimestampsIntoThis_(RunAuxiliary const&);
    RunAuxiliary(RunID id,
                 Timestamp beginTime,
                 Timestamp endTime,
                 ProcessHistoryID processHistoryID,
                 unsigned rangeSetID);

    // most recent process that put a RunProduct into this run
    // is the last on the list, this defines what "latest" is
    mutable ProcessHistoryID processHistoryID_{};
    mutable unsigned rangeSetID_{-1u};
    RunID id_{};
    Timestamp beginTime_{};
    Timestamp endTime_{};
  };

  std::ostream& operator<<(std::ostream&, const RunAuxiliary&);

} // namespace art

#endif /* canvas_Persistency_Provenance_RunAuxiliary_h */

// Local Variables:
// mode: c++
// End:
