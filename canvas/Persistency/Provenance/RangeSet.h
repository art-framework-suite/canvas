#ifndef canvas_Persistency_Provenance_RangeSet_h
#define canvas_Persistency_Provenance_RangeSet_h

#include "canvas/Persistency/Provenance/EventRange.h"
#include "canvas/Persistency/Provenance/IDNumber.h"
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

    explicit RangeSet(RunNumber_t const r)
      : RangeSet(r,{})
    {}

    explicit RangeSet(RunNumber_t const r,
                      std::vector<EventRange> const& eventRanges)
      : run_{r}
      , ranges_{eventRanges}
    {
      collapse();
    }

    template <typename ... ARGS>
    void emplace_range(ARGS&& ... args)
    {
      ranges_.emplace_back(args...);
      isCollapsed_ = false;
    }

    void set_run(RunNumber_t const r) { run_ = r; }

    auto run() const { return run_; }
    auto const& ranges() const { return ranges_; }

    void pop_front() { ranges_.erase(std::begin(ranges_)); }
    void clear() { ranges_.clear(); }

    decltype(auto) front() { return ranges_.front(); }
    decltype(auto) back() { return ranges_.back(); }
    bool empty() const { return ranges_.empty(); }
    auto begin() const { return ranges_.begin(); }
    auto end() const { return ranges_.end(); }

    unsigned checksum() const {return checksum_;}
    RangeSet& collapse();
    RangeSet& merge(RangeSet const& other);
    bool is_sorted() const { return std::is_sorted(ranges_.cbegin(),
                                                   ranges_.cend()); }
    bool is_collapsed() const { return isCollapsed_; }
    std::string to_compact_string() const;
    bool has_disjoint_ranges() const;
    void sort() { cet::sort_all(ranges_); }

  private:

    static constexpr unsigned invalidChecksum() { return std::numeric_limits<unsigned>::max(); }

    RunNumber_t run_;
    std::vector<EventRange> ranges_;
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
