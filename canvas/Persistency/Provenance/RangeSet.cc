#include "canvas/Persistency/Provenance/RangeSet.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/EventRange.h"
#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/crc32.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace art {

namespace {

bool
disjoint(vector<EventRange> const& ranges)
{
  if (ranges.size() < 2ull) {
    return true;
  }
  auto it = ranges.cbegin();
  auto const end = ranges.cend();
  for (auto nxt = next(it); nxt != end; ++it, ++nxt) {
    if (!it->is_disjoint(*nxt)) {
      return false;
    }
  }
  return true;
}

} // unnamed namespace

RangeSet
RangeSet::
invalid()
{
  return RangeSet{};
}

RangeSet
RangeSet::
forRun(RunID const rid)
{
  return RangeSet{rid.run(), true};
}

RangeSet
RangeSet::
forSubRun(SubRunID const srid)
{
  return RangeSet{srid.run(), {EventRange::forSubRun(srid.subRun())}};
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
~RangeSet()
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet()
  : run_{IDNumber<Level::Run>::invalid()}
  , ranges_()
  , fullRun_{false}
  , isCollapsed_{false}
  , checksum_{invalidChecksum()}
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet(RunNumber_t const r, bool const fullRun)
  : run_{r}
  , ranges_()
  , fullRun_{fullRun}
  , isCollapsed_{fullRun}
  , checksum_{cet::crc32{to_compact_string()}.digest()}
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet(RunNumber_t const r)
  : RangeSet{r, {}}
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet(RunNumber_t const r, vector<EventRange> const& eventRanges)
  : run_{r}
  , ranges_{eventRanges}
  , fullRun_{false}
  , isCollapsed_{false}
  , checksum_{invalidChecksum()}
{
  sort();
  collapse();
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet(RangeSet const& rhs)
  : run_{rhs.run_}
  , ranges_(rhs.ranges_)
  , fullRun_{rhs.fullRun_}
  , isCollapsed_{rhs.isCollapsed_}
  , checksum_{rhs.checksum_}
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet::
RangeSet(RangeSet&& rhs)
  : run_{move(rhs.run_)}
  , ranges_(move(rhs.ranges_))
  , fullRun_{move(rhs.fullRun_)}
  , isCollapsed_{move(rhs.isCollapsed_)}
  , checksum_{move(rhs.checksum_)}
{
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet&
RangeSet::
operator=(RangeSet const& rhs)
{
  if (this != &rhs) {
    run_ = rhs.run_;
    ranges_ = rhs.ranges_;
    fullRun_ = rhs.fullRun_;
    isCollapsed_ = rhs.isCollapsed_;
    checksum_ = rhs.checksum_;
  }
  return *this;
}

// Cannot be noexcept because ranges_ is a vector.
RangeSet&
RangeSet::
operator=(RangeSet&& rhs)
{
  run_ = move(rhs.run_);
  ranges_ = move(rhs.ranges_);
  fullRun_ = move(rhs.fullRun_);
  isCollapsed_ = move(rhs.isCollapsed_);
  checksum_ = move(rhs.checksum_);
  return *this;
}

RunNumber_t
RangeSet::
run() const
{
  return run_;
}

vector<EventRange> const&
RangeSet::
ranges() const
{
  return ranges_;
}

bool
RangeSet::
contains(RunNumber_t const r, SubRunNumber_t const s, EventNumber_t const e) const
{
  if (run_ != r) {
    return false;
  }
  for (auto const& range : ranges_) {
    if (range.contains(s, e)) {
      return true;
    }
  }
  return false;
}

bool
RangeSet::
is_valid() const
{
  return run_ != IDNumber<Level::Run>::invalid();
}

bool
RangeSet::
is_full_run() const
{
  return fullRun_;
}

bool
RangeSet::
is_full_subRun() const
{
  return (ranges_.size() == 1ull) && ranges_.front().is_full_subRun();
}

bool
RangeSet::
is_sorted() const
{
  return std::is_sorted(ranges_.cbegin(), ranges_.cend());
}

bool
RangeSet::
is_collapsed() const
{
  return isCollapsed_;
}

string
RangeSet::
to_compact_string() const
{
  using namespace std;
  string s{to_string(run_)};
  if (!ranges_.empty()) {
    s += ":";
  }
  for (auto const& r : ranges_) {
    s += to_string(r.subRun());
    s += "[";
    s += to_string(r.begin());
    s += ",";
    s += to_string(r.end());
    s += ")";
  }
  return s;
}

bool
RangeSet::
has_disjoint_ranges() const
{
  if (isCollapsed_ || is_sorted()) {
    return (ranges_.size() < 2ull) ? true : disjoint(ranges_);
  }
  RangeSet tmp{*this};
  tmp.sort();
  tmp.collapse();
  return tmp.has_disjoint_ranges();
}

bool
RangeSet::
empty() const
{
  return ranges_.empty();
}

RangeSet::const_iterator
RangeSet::
begin() const
{
  return ranges_.cbegin();
}

RangeSet::const_iterator
RangeSet::
end() const
{
  return ranges_.cend();
}

size_t
RangeSet::
begin_idx() const
{
  return 0;
}

size_t
RangeSet::
end_idx() const
{
  return static_cast<size_t>(ranges_.cend() - ranges_.cbegin());
}

unsigned
RangeSet::
checksum() const
{
  // Could cache checksums to improve performance when necessary.
  return checksum_ = cet::crc32{to_compact_string()}.digest();
}

size_t
RangeSet::
next_subrun_or_end(size_t b) const
{
  if (b == end_idx()) {
    return end_idx();
  }
  auto const sr = ranges_.at(b).subRun();
  auto pos = find_if(ranges_.cbegin() + b, ranges_.cend(), [sr](auto const& range) { return range.subRun() != sr; });
  return static_cast<size_t>(pos - ranges_.cbegin());
}

EventRange&
RangeSet::
front()
{
  return ranges_.front();
}

EventRange&
RangeSet::
back()
{
  return ranges_.back();
}

EventRange&
RangeSet::
at(size_t idx)
{
  return ranges_.at(idx);
}

vector<EventRange>
RangeSet::
extract_ranges(size_t const b, size_t const e)
{
  vector<EventRange> result;
  if (!ranges_.empty() && (e >= 1) && (e <= ranges_.size())) {
    copy(ranges_.cbegin() + b, ranges_.cbegin() + e, back_inserter(result));
  }
  return result;
}

void
RangeSet::
assign_ranges(RangeSet const& rs, size_t const b, size_t const e)
{
  require_not_full_run();
  if (!rs.ranges_.empty() && (e >= 1) && (e <= rs.ranges_.size())) {
    ranges_.assign(rs.ranges_.cbegin() + b, rs.ranges_.cbegin() + e);
  }
}

RangeSet&
RangeSet::
collapse()
{
  if (isCollapsed_) {
    return *this;
  }
  if (ranges_.size() < 2) {
    isCollapsed_ = true;
    return *this;
  }
  if (!is_sorted()) {
    throw Exception(errors::LogicError, "RangeSet::collapse()")
        << "A range set must be sorted before it is collapsed.\n";
  }
  auto processing = ranges_;
  decltype(ranges_) result;
  result.reserve(ranges_.size());
  result.push_back(ranges_.front());
  for (auto ir = (ranges_.cbegin() + 1), e = ranges_.cend(); ir != e; ++ir) {
    auto const& r = *ir;
    auto& back = result.back();
    if (back.is_adjacent(r)) {
      back.merge(r);
    }
    else if (back.is_disjoint(r)) {
      result.push_back(r);
    }
    else {
      throw Exception(errors::EventRangeOverlap)
          << "Attempt to merge event ranges that both contain one or more of the same events\n"
          << " Run: " << run_ << '\n'
          << "  " << back << "  vs.\n"
          << "  " << r;
    }
  }
  swap(ranges_, result);
  isCollapsed_ = true;
  return *this;
}

RangeSet&
RangeSet::
merge(RangeSet const& other)
{
  if (!other.is_valid()) {
    return *this;
  }
  if (!is_valid()) {
    run_ = other.run();
  }
  vector<EventRange> merged;
  std::merge(ranges_.cbegin(), ranges_.cend(),
             other.ranges_.cbegin(), other.ranges_.cend(),
             back_inserter(merged));
  unique(merged.begin(), merged.end());
  swap(ranges_, merged);
  isCollapsed_ = false;
  collapse();
  return *this;
}

// For a range [1,6) split into [1,3) and [3,6) the specified
// event number is the new 'end' of the left range (3).
pair<size_t, bool>
RangeSet::
split_range(SubRunNumber_t const s, EventNumber_t const e)
{
  require_not_full_run();
  bool did_split = false;
  auto result = ranges_.end();
  auto foundRange = find_if(ranges_.cbegin(), ranges_.cend(), [s, e](auto const & r) { return r.contains(s, e); });
  // Split only if:
  // - the range is found (i.e. the event is contained by the found range)
  // - the range is valid
  // - the size of the range is greater than 1
  if ((foundRange != ranges_.cend()) && foundRange->is_valid() && (foundRange->size() > 1ull)) {
    auto const begin = foundRange->begin();
    auto const end = foundRange->end();
    auto leftIt = ranges_.emplace(foundRange, s, begin, e);
    result = next(leftIt);
    EventRange right{s, e, end};
    swap(*result, right);
    did_split = true;
  }
  return make_pair(static_cast<size_t>(result - ranges_.cbegin()), did_split);
}

void
RangeSet::
set_run(RunNumber_t const r)
{
  run_ = r;
}

void
RangeSet::
sort()
{
  cet::sort_all(ranges_);
}

void
RangeSet::
clear()
{
  ranges_.clear();
}

void
RangeSet::
require_not_full_run()
{
  if (fullRun_) {
    throw Exception(errors::LogicError, "RangeSet::require_not_full_run")
        << "\nA RangeSet created using RangeSet::forRun cannot be modified.\n";
  }
}

bool
operator==(RangeSet const& l, RangeSet const& r)
{
  if (!(l.is_valid() && r.is_valid())) {
    return false;
  }
  return (l.run() == r.run()) && (l.ranges() == r.ranges());
}

bool
same_ranges(RangeSet const& l, RangeSet const& r)
{
  return l == r;
}

bool
disjoint_ranges(RangeSet const& l, RangeSet const& r)
{
  if (!(l.is_valid() && r.is_valid())) {
    return false;
  }
  if (l == r) {
    return false;
  }
  // If either RangeSet by itself is not disjoint, return false
  if (!l.has_disjoint_ranges() || !r.has_disjoint_ranges()) {
    return false;
  }
  if (l.run() != r.run()) {
    return true;  // Can't imagine that anyone would be presented with
  }
  // two RangeSets from different runs.  But just in case....
  RangeSet ltmp{l};
  RangeSet rtmp{r};
  auto const& lranges = ltmp.collapse().ranges();
  auto const& rranges = rtmp.collapse().ranges();
  vector<EventRange> merged;
  merge(lranges.cbegin(), lranges.cend(),
        rranges.cbegin(), rranges.cend(),
        back_inserter(merged));
  return disjoint(merged);
}

bool
overlapping_ranges(RangeSet const& l, RangeSet const& r)
{
  if (!(l.is_valid() && r.is_valid())) {
    return false;
  }
  return !same_ranges(l, r) && !disjoint_ranges(l, r);
}

ostream&
operator<<(ostream& os, RangeSet const& rs)
{
  os << " Run: " << rs.run();
  if (rs.is_full_run()) {
    os << " (full run)";
  }
  for (auto const& er : rs.ranges()) {
    os << "\n  " << er;
  }
  return os;
}

} // namespace art

