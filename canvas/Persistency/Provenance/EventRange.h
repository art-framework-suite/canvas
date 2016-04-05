#ifndef canvas_Persistency_Provenance_EventRange_h
#define canvas_Persistency_Provenance_EventRange_h

#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Utilities/Exception.h"

#include <ostream>
#include <set>
#include <vector>

namespace art {

  class EventRange {
  public:

    explicit EventRange() = default;

    explicit EventRange(SubRunNumber_t const s,
                        EventNumber_t const i,
                        EventNumber_t const j)
      : subrun_{s}
      , begin_{i}
      , end_{j}
    {
      checkOrdering_(begin_, end_);
    }

    bool merge(EventRange const& other)
    {
      bool const mergeable = is_adjacent(other);
      if (mergeable)
        end_ = other.end_;
      return mergeable;
    }

    bool operator<(EventRange const& other) const
    {
      if (subrun_ == other.subrun_) {
        if (begin_ == other.begin_) {
          return end_ < other.end_;
        }
        else {
          return begin_ < other.begin_;
        }
      }
      else {
        return subrun_ < other.subrun_;
      }
    }

    bool operator==(EventRange const& other) const
    {
      return subrun_ == other.subrun_ && begin_ == other.begin_ && end_ == other.end_;
    }

    bool operator!=(EventRange const& other) const
    {
      return !operator==(other);
    }

    void set_end(EventNumber_t const e) {
      checkOrdering_(begin_,e);
      end_ = e;
    }

    bool contains(EventNumber_t const e) const
    {
      return e >= begin_ && e < end_;
    }

    auto subrun() const { return subrun_; }
    bool empty() const { return begin_ == end_; }
    auto begin() const { return begin_; }
    auto end()   const { return end_; }

    static bool are_valid(EventRange const& l, EventRange const& r)
    {
      return l.is_valid() && r.is_valid();
    }

    bool is_valid() const
    {
      using art::is_valid;
      return is_valid(subrun_) && is_valid(begin_) && is_valid(end_);
    }

    bool is_adjacent(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return subrun_ == other.subrun_ && end_ == other.begin_;
    }

    bool is_disjoint(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return (subrun_ == other.subrun_) ? end_ <= other.begin_ : true;
    }

    bool is_same(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return operator==(other);
    }

    // is_same(other) == true:
    //     implies is_subset(other) == true
    //     implies is_superset(other) == true
    bool is_subset(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return subrun_ == other.subrun_ && begin_ >= other.begin_ && end_ <= other.end_;
    }

    bool is_superset(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return subrun_ == other.subrun_ && begin_ <= other.begin_ && end_ >= other.end_;
    }

    bool is_overlapping(EventRange const& other) const
    {
      if (!are_valid(*this, other)) return false;
      return !is_disjoint(other) && !is_subset(other) && !is_superset(other);
    }

  private:

    void checkOrdering_(EventNumber_t const b,
                        EventNumber_t const e)
    {
      if (b > e)
        throw art::Exception(art::errors::LogicError)
          << "The 'begin' value for an EventRange must be less "
          << "than the 'end' value.\n";
    }

    SubRunNumber_t subrun_ { IDNumber<Level::SubRun>::invalid() };
    EventNumber_t begin_ { IDNumber<Level::Event>::invalid() };
    EventNumber_t end_ { IDNumber<Level::Event>::invalid() };
  };

  inline std::ostream& operator<<(std::ostream& os, EventRange const& r)
  {
    os << "SubRun: " << r.subrun() << " Event range: [" << r.begin() << ',' << r.end() << ')';
    return os;
  }

  inline bool are_adjacent(EventRange const& l,
                           EventRange const& r)
  {
    return l.is_adjacent(r);
  }

}

#endif

// Local variables:
// mode: c++
// End:
