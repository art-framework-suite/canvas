#ifndef canvas_Persistency_Provenance_RangeSet_h
#define canvas_Persistency_Provenance_RangeSet_h

#include "canvas/Persistency/Provenance/EventRange.h"
#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"

extern "C" {
#include "sqlite3.h"
}

#include <algorithm>
#include <ostream>
#include <set>
#include <vector>

namespace art {

  class RangeSet {
  public:

    static RangeSet invalid();
    static RangeSet forRun(RunID const rid);
    static RangeSet forSubRun(SubRunID const srid);

    explicit RangeSet(RunNumber_t const r);
    explicit RangeSet(RunNumber_t const r, std::vector<EventRange> const& eventRanges);

    using const_iterator = std::vector<EventRange>::const_iterator;

    RunNumber_t run() const { return run_; }
    std::vector<EventRange> const& ranges() const { return ranges_; }
    bool is_valid() const;
    bool is_full_run() const { return fullRun_; }
    bool is_sorted() const;
    bool is_collapsed() const { return isCollapsed_; }
    std::string to_compact_string() const;
    bool has_disjoint_ranges() const;

    bool empty() const { return ranges_.empty(); }
    auto begin() const { return ranges_.begin(); }
    auto end() const { return ranges_.end(); }

    unsigned checksum() const {return checksum_;}

    decltype(auto) front() { return ranges_.front(); }
    decltype(auto) back() { return ranges_.back(); }

    void assign_ranges(const_iterator b, const_iterator e);

    template <typename ... ARGS>
    void emplace_range(ARGS&& ... args);

    RangeSet& collapse();
    RangeSet& merge(RangeSet const& other);
    const_iterator split_range(SubRunNumber_t s,
                               EventNumber_t e);
    void set_run(RunNumber_t const r) { run_ = r; }

    void sort() { cet::sort_all(ranges_); }
    void clear() { ranges_.clear(); }

    static constexpr unsigned invalidChecksum() { return std::numeric_limits<unsigned>::max(); }

  private:

    explicit RangeSet() = default;
    explicit RangeSet(RunNumber_t const r, bool fullRun);

    void require_not_full_run()
    {
      if (fullRun_)
        throw art::Exception(art::errors::LogicError,"RangeSet::require_not_full_run")
          << "\nA RangeSet created using RangeSet::forRun cannot be modified.\n";
    }

    RunNumber_t run_ {IDNumber<Level::Run>::invalid()};
    std::vector<EventRange> ranges_ {};

    // Auxiliary info
    bool fullRun_ {false};
    bool isCollapsed_ {false};
    unsigned checksum_ {invalidChecksum()};
  };

  inline std::ostream& operator<<(std::ostream& os, RangeSet const& rs)
  {
    os << " Run: " << rs.run();
    for (auto const& er : rs.ranges()) {
      os << "\n  " << er;
    }
    return os;
  }

  inline bool operator==(RangeSet const& l,
                         RangeSet const& r)
  {
    return l.run() == r.run() && l.ranges() == r.ranges();
  }

  inline bool same_ranges(RangeSet const& l,
                          RangeSet const& r)
  {
    return l == r;
  }

  bool disjoint_ranges(RangeSet const& l, RangeSet const& r);

  //==========================================================
  // Implementation details

  template <typename ... ARGS>
  void
  RangeSet::emplace_range(ARGS&& ... args)
  {
    require_not_full_run();
    ranges_.emplace_back(std::forward<ARGS>(args)...);
    isCollapsed_ = false;
  }

}

#endif

// Local variables:
// mode: c++
// End:
