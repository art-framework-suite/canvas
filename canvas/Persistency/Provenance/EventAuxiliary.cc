#include "canvas/Persistency/Provenance/EventAuxiliary.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <utility>

using namespace std;

namespace art {

EventAuxiliary::
~EventAuxiliary()
{
}

EventAuxiliary::
EventAuxiliary()
  : id_{}
  , time_{}
  , isRealData_{false}
  , experimentType_{Any}
{
}

EventAuxiliary::
EventAuxiliary(EventID const& theId, Timestamp const& theTime, bool isReal, ExperimentType eType /*= Any*/)
  : id_{theId}
  , time_{theTime}
  , isRealData_{isReal}
  , experimentType_{eType}
{
}

EventAuxiliary::
EventAuxiliary(EventAuxiliary const& rhs)
  : id_{rhs.id_}
  , time_{rhs.time_}
  , isRealData_{rhs.isRealData_}
  , experimentType_{rhs.experimentType_}
{
}

EventAuxiliary::
EventAuxiliary(EventAuxiliary&& rhs)
  : id_{move(rhs.id_)}
  , time_{move(rhs.time_)}
  , isRealData_{move(rhs.isRealData_)}
  , experimentType_{move(rhs.experimentType_)}
{
}

EventAuxiliary&
EventAuxiliary::
operator=(EventAuxiliary const& rhs)
{
  if (this != &rhs) {
    id_ = rhs.id_;
    time_ = rhs.time_;
    isRealData_ = rhs.isRealData_;
    experimentType_ = rhs.experimentType_;
  }
  return *this;
}

EventAuxiliary&
EventAuxiliary::
operator=(EventAuxiliary&& rhs)
{
  id_ = move(rhs.id_);
  time_ = move(rhs.time_);
  isRealData_ = move(rhs.isRealData_);
  experimentType_ = move(rhs.experimentType_);
  return *this;
}

Timestamp const&
EventAuxiliary::
time() const noexcept
{
  return time_;
}

EventID const&
EventAuxiliary::
id() const noexcept
{
  return id_;
}

EventID const&
EventAuxiliary::
eventID() const noexcept
{
  return id_;
}

RunID const&
EventAuxiliary::
runID() const noexcept
{
  return id_.runID();
}

SubRunID const&
EventAuxiliary::
subRunID() const noexcept
{
  return id_.subRunID();
}

RunNumber_t
EventAuxiliary::
run() const noexcept
{
  return id_.run();
}

SubRunNumber_t
EventAuxiliary::
subRun() const noexcept
{
  return id_.subRun();
}

EventNumber_t
EventAuxiliary::
event() const noexcept
{
  return id_.event();
}

bool
EventAuxiliary::
isRealData() const noexcept
{
  return isRealData_;
}

EventAuxiliary::ExperimentType
EventAuxiliary::
experimentType() const noexcept
{
  return experimentType_;
}

bool
EventAuxiliary::
operator==(EventAuxiliary const& other) const noexcept
{
  return (id_ == other.id_) && (time_ == other.time_) &&
         (isRealData_ == other.isRealData_) &&
         (experimentType_ == other.experimentType_);
}

void
EventAuxiliary::
write(ostream& os) const
{
  os << id_ << endl;
}

ostream&
operator<<(ostream& os, const EventAuxiliary& p)
{
  p.write(os);
  return os;
}

} // namespace art

