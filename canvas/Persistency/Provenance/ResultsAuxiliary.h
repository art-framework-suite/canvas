#ifndef canvas_Persistency_Provenance_ResultsAuxiliary_h
#define canvas_Persistency_Provenance_ResultsAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/fwd.h"

#include <iosfwd>
#include <set>

namespace art {
  class ResultsAuxiliary {
  public:
    static constexpr BranchType branch_type = InResults;

    void write(std::ostream& os) const;

    ProcessHistoryID const&
    processHistoryID() const
    {
      return processHistoryID_;
    }

    void
    setProcessHistoryID(ProcessHistoryID const& phid)
    {
      processHistoryID_ = phid;
    }

  private:
    // most recent process that put a RunProduct into this run
    // is the last on the list, this defines what "latest" is
    ProcessHistoryID processHistoryID_{};

    // allEventsProcessHistories_ contains all the ProcessHistoryIDs for all
    // events in this run seen so far.
    std::set<ProcessHistoryID> allEventsProcessHistories_{};
  };

  inline std::ostream&
  operator<<(std::ostream& os, ResultsAuxiliary const& p)
  {
    p.write(os);
    return os;
  }

}
#endif /* canvas_Persistency_Provenance_ResultsAuxiliary_h */

// Local Variables:
// mode: c++
// End:
