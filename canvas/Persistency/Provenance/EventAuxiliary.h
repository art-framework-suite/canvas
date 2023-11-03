#ifndef canvas_Persistency_Provenance_EventAuxiliary_h
#define canvas_Persistency_Provenance_EventAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <iosfwd>

namespace art {
  class EventAuxiliary {
  public:
    static constexpr BranchType branch_type = InEvent;
    enum ExperimentType {
      Any = 0,
      Align = 1,
      Calib = 2,
      Cosmic = 3,
      Data = 4,
      Mc = 5,
      Raw = 6,
      Test = 7
    };

    EventAuxiliary();
    EventAuxiliary(EventID const& theId,
                   Timestamp const& theTime,
                   bool isReal,
                   ExperimentType eType = Any,
                   ProcessHistoryID const& phid = {});

    Timestamp const& time() const noexcept;
    EventID const& id() const noexcept;
    EventID const& eventID() const noexcept;
    RunID const& runID() const noexcept;
    SubRunID const& subRunID() const noexcept;
    RunNumber_t run() const noexcept;
    SubRunNumber_t subRun() const noexcept;
    EventNumber_t event() const noexcept;
    bool isRealData() const noexcept;
    EventAuxiliary::ExperimentType experimentType() const noexcept;
    bool operator==(EventAuxiliary const& other) const noexcept;
    void write(std::ostream& os) const;

    ProcessHistoryID const& processHistoryID() const noexcept;
    void setProcessHistoryID(ProcessHistoryID const&);

  private:
    // most recent process that put a RunProduct into this run
    // is the last on the list, this defines what "latest" is
    ProcessHistoryID processHistoryID_{};

    EventID id_{};
    Timestamp time_{};
    bool isRealData_{false};
    ExperimentType experimentType_{Any};
  };
  std::ostream& operator<<(std::ostream&, const EventAuxiliary&);
} // namespace art

#endif /* canvas_Persistency_Provenance_EventAuxiliary_h */

// Local Variables:
// mode: c++
// End:
