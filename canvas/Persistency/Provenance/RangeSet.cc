#include "canvas/Persistency/Provenance/RangeSet.h"
#include "cetlib/crc32.h"

using art::RangeSet;

namespace {

  auto checksum(std::string const& compact_string)
  {
    return cet::crc32{compact_string}.digest();
  }

  bool disjoint(std::vector<art::EventRange> const& ranges)
  {
    if ( ranges.size() < 2ull )
      return true;

    auto it = ranges.cbegin();
    auto const end = ranges.cend();
    for (auto next = std::next(it); next != end; ++it, ++next) {
      if (!it->is_disjoint(*next))
        return false;
    }
    return true;
  }

}

RangeSet
RangeSet::invalid()
{
  return RangeSet{};
}

RangeSet
RangeSet::forRun(RunID const rid)
{
  return RangeSet{rid.run(), true};
}

RangeSet
RangeSet::forSubRun(SubRunID const srid)
{
  return RangeSet{srid.run(), {EventRange::forSubRun(srid.subRun())}};
}

RangeSet::RangeSet(RunNumber_t const r)
  : RangeSet{r,{}}
{}

RangeSet::RangeSet(RunNumber_t const r,
                   std::vector<EventRange> const& eventRanges)
  : run_{r}
  , ranges_{eventRanges}
{
  sort();
  collapse();
}

RangeSet&
RangeSet::collapse()
{
  if (isCollapsed_) {
    checksum_ = ::checksum(to_compact_string());
    return *this;
  }

  if (ranges_.size() < 2) {
    isCollapsed_ = true;
    checksum_ = ::checksum(to_compact_string());
    return *this;
  }

  if (!is_sorted())
    throw art::Exception(art::errors::LogicError,"RangeSet::collapse()")
      << "A range set must be sorted before it is collapsed.\n";

  auto processing = ranges_;
  decltype(ranges_) result;
  while (!processing.empty()) {
    result.push_back(processing.front());
    processing.erase(processing.cbegin());
    decltype(ranges_) remaining;
    for (auto const& r : processing) {
      if (result.back().is_adjacent(r)) {
        result.back().merge(r);
        // Check if newly-formed range has been cached as a
        // remaining range to check
        auto f = std::find(remaining.begin(), remaining.end(), result.back());
        if (f != remaining.end())
          remaining.erase(f);
      }
      else {
        // If the current range is not adjacent to the 'result'
        // range, then cache it for further processing later on.
        remaining.push_back(r);
      }
    }
    std::swap(processing, remaining);
  }
  std::swap(ranges_, result);
  isCollapsed_ = true;
  checksum_ = ::checksum(to_compact_string());
  return *this;
}

void
RangeSet::assign_ranges(RangeSet::const_iterator const b,
                        RangeSet::const_iterator const e)
{
  require_not_full_run();
  ranges_.assign(b,e);
}

RangeSet&
RangeSet::merge(RangeSet const& other)
{
  require_not_full_run();

  if (!other.is_valid())
    return *this;

  if (!is_valid())
    run_ = other.run();

  std::vector<EventRange> merged;
  std::merge(ranges_.begin(), ranges_.end(),
             other.ranges_.begin(), other.ranges_.end(),
             std::back_inserter(merged));
  std::unique(merged.begin(), merged.end());
  std::swap(ranges_, merged);
  isCollapsed_ = false;
  collapse();
  return *this;
}

RangeSet::const_iterator
RangeSet::split_range(SubRunNumber_t const s,
                      EventNumber_t const e)
{
  require_not_full_run();
  auto result = ranges_.end();
  auto foundRange = std::find_if(ranges_.cbegin(), ranges_.cend(),
                                 [s,e](auto const& r) {
                                   return r.contains(s,e);
                                 });

  // Split only if:
  // - the range is found (i.e. the event is contained by the found range)
  // - the range is valid
  // - the size of the range is greater than 1
  if (foundRange != ranges_.cend() &&
      foundRange->is_valid() &&
      foundRange->size() > 1ull) {
    auto const begin = foundRange->begin();
    auto const end = foundRange->end();
    auto leftIt = ranges_.emplace(foundRange, s, begin, e);
    result = std::next(leftIt);
    EventRange right {s, e, end};
    std::swap(*result, right);
  }
  checksum_ = ::checksum(to_compact_string());
  return result;
}

bool
RangeSet::has_disjoint_ranges() const
{
  if (isCollapsed_ || is_sorted() ) {
    return ranges_.size() < 2ull ? true : disjoint(ranges_);
  }

  RangeSet tmp {*this};
  tmp.sort();
  tmp.collapse();
  return tmp.has_disjoint_ranges();
}

bool
RangeSet::is_valid() const
{
  return run_ != IDNumber<Level::Run>::invalid();
}

bool
RangeSet::is_sorted() const
{
  return std::is_sorted(ranges_.cbegin(), ranges_.cend());
}

std::string
RangeSet::to_compact_string() const
{
  using namespace std;
  string s {to_string(run_)};
  if (!ranges_.empty())
    s += ":";
  for (auto const& r: ranges_) {
    s += to_string(r.subRun());
    s += "[";
    s += to_string(r.begin());
    s += ",";
    s += to_string(r.end());
    s += ")";
  }
  return s;
}

// private c'tor
RangeSet::RangeSet(RunNumber_t const r,
                   bool const fullRun)
  : run_{r}
  , fullRun_{fullRun}
  , isCollapsed_{fullRun}
  , checksum_{::checksum(to_compact_string())}
{}

bool
art::disjoint_ranges(RangeSet const& l, RangeSet const& r)
{
  // If either RangeSet by itself is not disjoint, return false
  if (!l.has_disjoint_ranges() || !r.has_disjoint_ranges()) return false;

  if (l.run() != r.run()) return true; // Can't imagine that anyone would be presented with
                                       // two RangeSets from different runs.  But just in case....
  RangeSet ltmp {l};
  RangeSet rtmp {r};
  auto const& lranges = ltmp.collapse().ranges();
  auto const& rranges = rtmp.collapse().ranges();

  std::vector<EventRange> merged;
  std::merge(lranges.begin(), lranges.end(),
             rranges.begin(), rranges.end(),
             std::back_inserter(merged));

  return disjoint(merged);
}
