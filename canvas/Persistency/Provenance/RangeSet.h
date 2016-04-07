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

    static RangeSet invalid()
    {
      return RangeSet{};
    }

    static RangeSet for_run(RunNumber_t const r)
    {
      return RangeSet{r};
    }

    static RangeSet for_subrun(SubRunID const srid)
    {
      return RangeSet{srid.run(), {EventRange::for_subrun(srid.subRun())}};
    }

    static RangeSet for_run(RunID const rid)
    {
      return for_run(rid.run());
    }

    explicit RangeSet(RunNumber_t const r,
                      std::vector<EventRange> const& eventRanges)
      : run_{r}
      , ranges_{eventRanges}
    {
      collapse();
    }

    RunNumber_t run() const { return run_; }
    std::vector<EventRange> const& ranges() const { return ranges_; }
    bool is_valid() const;
    bool is_sorted() const;
    bool is_collapsed() const { return isCollapsed_; }
    std::string to_compact_string() const;
    bool has_disjoint_ranges() const;

    bool empty() const { return ranges_.empty(); }
    auto begin() const { return ranges_.begin(); }
    auto end() const { return ranges_.end(); }

    unsigned checksum() const {return checksum_;}

    template <typename ... ARGS>
    void emplace_range(ARGS&& ... args)
    {
      ranges_.emplace_back(args...);
      isCollapsed_ = false;
    }

    void set_run(RunNumber_t const r) { run_ = r; }
    decltype(auto) front() { return ranges_.front(); }
    decltype(auto) back() { return ranges_.back(); }
    RangeSet& collapse();
    RangeSet& merge(RangeSet const& other);
    void sort() { cet::sort_all(ranges_); }
    void pop_front() { ranges_.erase(std::begin(ranges_)); }
    void clear() { ranges_.clear(); }

    static constexpr unsigned invalidChecksum() { return std::numeric_limits<unsigned>::max(); }

  private:

    explicit RangeSet() = default;

    explicit RangeSet(RunNumber_t const r)
      : run_{r}
    {}

    RunNumber_t run_ {IDNumber<Level::Run>::invalid()};
    std::vector<EventRange> ranges_ {};
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

  inline bool are_same(RangeSet const& l,
                       RangeSet const& r)
  {
    return l == r;
  }

  bool are_disjoint(RangeSet const& l, RangeSet const& r);

}

#endif

// Local variables:
// mode: c++
// End:
