#include "canvas/Persistency/Provenance/RunAuxiliary.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <ostream>
#include <set>
#include <utility>

using namespace std;

namespace art {

  RunAuxiliary::RunAuxiliary() = default;

  RunAuxiliary::RunAuxiliary(RunID const& theId,
                             Timestamp const& theTime,
                             Timestamp const& theEndTime)
    : id_{theId}, beginTime_{theTime}, endTime_{theEndTime}
  {}

  RunAuxiliary::RunAuxiliary(RunNumber_t const& run,
                             Timestamp const& theTime,
                             Timestamp const& theEndTime)
    : RunAuxiliary{RunID{run}, theTime, theEndTime}
  {}

  // private
  RunAuxiliary::RunAuxiliary(RunID const id,
                             Timestamp const beginTime,
                             Timestamp const endTime,
                             ProcessHistoryID const processHistoryID,
                             unsigned const rangeSetID)
    : processHistoryID_{processHistoryID}
    , rangeSetID_{rangeSetID}
    , id_{id}
    , beginTime_{beginTime}
    , endTime_{endTime}
  {}

  RunAuxiliary
  RunAuxiliary::duplicateWith(Timestamp const beginTime,
                              Timestamp const endTime) const
  {
    return RunAuxiliary{
      id_, beginTime, endTime, processHistoryID_, rangeSetID_};
  }

  ProcessHistoryID const&
  RunAuxiliary::processHistoryID() const noexcept
  {
    return processHistoryID_;
  }

  unsigned
  RunAuxiliary::rangeSetID() const noexcept
  {
    return rangeSetID_;
  }

  RunID const&
  RunAuxiliary::id() const noexcept
  {
    return id_;
  }

  Timestamp const&
  RunAuxiliary::beginTime() const noexcept
  {
    return beginTime_;
  }

  Timestamp const&
  RunAuxiliary::endTime() const noexcept
  {
    return endTime_;
  }

  void
  RunAuxiliary::setProcessHistoryID(ProcessHistoryID const& phid)
  {
    processHistoryID_ = phid;
  }

  RunID const&
  RunAuxiliary::runID() const noexcept
  {
    return id_;
  }

  void
  RunAuxiliary::runID(RunID const& id)
  {
    id_ = id;
  }

  RunNumber_t
  RunAuxiliary::run() const noexcept
  {
    return id_.run();
  }

  void
  RunAuxiliary::setRangeSetID(unsigned const id) const
  {
    rangeSetID_ = id;
  }

  void
  RunAuxiliary::write(ostream& os) const
  {
    os << "Process History ID = " << processHistoryID_ << endl;
    os << id_ << endl;
  }

  bool
  RunAuxiliary::mergeAuxiliary(RunAuxiliary const& newAux)
  {
    mergeNewTimestampsIntoThis_(newAux);
    // Keep the process history ID that is in the preexisting principal
    // It may have been updated to include the current process.
    // There is one strange other case where the two ProcessHistoryIDs
    // could be different which should not be important and we just ignore.
    // There could have been previous processes which only dropped products.
    // These processes could have dropped the same branches but had different
    // process names ... Ignore this.
    return id_ == newAux.id();
  }

  void
  RunAuxiliary::mergeNewTimestampsIntoThis_(RunAuxiliary const& newAux)
  {
    if ((beginTime_ == Timestamp::invalidTimestamp()) ||
        (newAux.beginTime() == Timestamp::invalidTimestamp())) {
      beginTime_ = Timestamp::invalidTimestamp();
    } else if (newAux.beginTime() < beginTime_) {
      beginTime_ = newAux.beginTime();
    }
    if ((endTime_ == Timestamp::invalidTimestamp()) ||
        (newAux.endTime() == Timestamp::invalidTimestamp())) {
      endTime_ = Timestamp::invalidTimestamp();
    } else if (newAux.endTime() > endTime_) {
      endTime_ = newAux.endTime();
    }
  }

  ostream&
  operator<<(ostream& os, const RunAuxiliary& p)
  {
    p.write(os);
    return os;
  }

} // namspace art
