#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <utility>

using namespace std;

namespace art {

  SubRunAuxiliary::SubRunAuxiliary() = default;

  SubRunAuxiliary::SubRunAuxiliary(SubRunID const& theId,
                                   Timestamp const& theTime,
                                   Timestamp const& theEndTime)
    : id_{theId}, beginTime_{theTime}, endTime_{theEndTime}
  {}

  SubRunAuxiliary::SubRunAuxiliary(RunNumber_t const theRun,
                                   SubRunNumber_t const theSubRun,
                                   Timestamp const& theTime,
                                   Timestamp const& theEndTime)
    : SubRunAuxiliary{SubRunID{theRun, theSubRun}, theTime, theEndTime}
  {}

  ProcessHistoryID const&
  SubRunAuxiliary::processHistoryID() const noexcept
  {
    return processHistoryID_;
  }

  void
  SubRunAuxiliary::setProcessHistoryID(ProcessHistoryID const& phid)
  {
    processHistoryID_ = phid;
  }

  SubRunID const&
  SubRunAuxiliary::id() const noexcept
  {
    return id_;
  }

  SubRunID const&
  SubRunAuxiliary::subRunID() const noexcept
  {
    return id_;
  }

  RunID const&
  SubRunAuxiliary::runID() const noexcept
  {
    return id_.runID();
  }

  RunNumber_t
  SubRunAuxiliary::run() const noexcept
  {
    return id_.run();
  }

  SubRunNumber_t
  SubRunAuxiliary::subRun() const noexcept
  {
    return id_.subRun();
  }

  Timestamp const&
  SubRunAuxiliary::beginTime() const noexcept
  {
    return beginTime_;
  }

  Timestamp const&
  SubRunAuxiliary::endTime() const noexcept
  {
    return endTime_;
  }

  void
  SubRunAuxiliary::setEndTime(Timestamp const& time)
  {
    if (endTime_ == Timestamp::invalidTimestamp()) {
      endTime_ = time;
    }
  }

  void
  SubRunAuxiliary::setRangeSetID(unsigned const id) const
  {
    rangeSetID_ = id;
  }

  unsigned
  SubRunAuxiliary::rangeSetID() const noexcept
  {
    return rangeSetID_;
  }

  void
  SubRunAuxiliary::write(ostream& os) const
  {
    os << "Process History ID = " << processHistoryID_ << endl;
    os << id_ << endl;
  }

  bool
  SubRunAuxiliary::mergeAuxiliary(SubRunAuxiliary const& newAux)
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
    // Keep the process history ID that is in the preexisting principal
    // It may have been updated to include the current process.
    // There is one strange other case where the two ProcessHistoryIDs
    // could be different which should not be important and we just ignore.
    // There could have been previous processes which only dropped products.
    // These processes could have dropped the same branches but had different
    // process names ... Ignore this.
    if (id_ != newAux.id()) {
      return false;
    }
    return true;
  }

  ostream&
  operator<<(ostream& os, SubRunAuxiliary const& p)
  {
    p.write(os);
    return os;
  }

} // namespace art
