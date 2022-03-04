#ifndef canvas_Persistency_Provenance_SubRunAuxiliary_h
#define canvas_Persistency_Provenance_SubRunAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <iosfwd>

// Auxiliary subRun data that is persistent

namespace art {

  class SubRunAuxiliary {
  public:
    static constexpr BranchType branch_type = InSubRun;

    SubRunAuxiliary();

    SubRunAuxiliary(SubRunID const& theId,
                    Timestamp const& theTime,
                    Timestamp const& theEndTime);

    SubRunAuxiliary(RunNumber_t const theRun,
                    SubRunNumber_t const theSubRun,
                    Timestamp const& theTime,
                    Timestamp const& theEndTime);

    SubRunAuxiliary duplicateWith(SubRunID const& id) const;

    void write(std::ostream& os) const;

    ProcessHistoryID const& processHistoryID() const noexcept;
    void setProcessHistoryID(ProcessHistoryID const& phid);

    SubRunID const& id() const noexcept;
    SubRunID const& subRunID() const noexcept;
    RunID const& runID() const noexcept;
    RunNumber_t run() const noexcept;
    SubRunNumber_t subRun() const noexcept;

    Timestamp const& beginTime() const noexcept;
    Timestamp const& endTime() const noexcept;

    void setRangeSetID(unsigned const id) const;
    unsigned rangeSetID() const noexcept;

    bool mergeAuxiliary(SubRunAuxiliary const&);

  private:
    mutable ProcessHistoryID processHistoryID_{};
    mutable unsigned rangeSetID_{-1u};

  public:
    SubRunID id_{};
    Timestamp beginTime_{};
    Timestamp endTime_{};
  };

  std::ostream& operator<<(std::ostream&, SubRunAuxiliary const&);

} // namespace art

#endif /* canvas_Persistency_Provenance_SubRunAuxiliary_h */

// Local Variables:
// mode: c++
// End:
