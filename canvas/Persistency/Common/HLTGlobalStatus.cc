#include "canvas/Persistency/Common/HLTGlobalStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTPathStatus.h"
#include "canvas/Persistency/Common/HLTenums.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

using namespace std;
using namespace literals::string_literals;

namespace art {

  HLTGlobalStatus::~HLTGlobalStatus() {}

  HLTGlobalStatus::HLTGlobalStatus(std::size_t const n /* = 0 */) : paths_(n) {}

  std::size_t
  HLTGlobalStatus::size() const
  {
    return paths_.size();
  }

  void
  HLTGlobalStatus::reset()
  {
    unsigned const n = paths_.size();
    for (unsigned i = 0; i != n; ++i) {
      paths_[i].reset();
    }
  }

  bool
  HLTGlobalStatus::wasrun() const
  {
    auto const n = paths_.size();
    auto i = n;
    for (i = 0; i != n; ++i) {
      if (paths_[i].state() != hlt::Ready) {
        return true;
      }
    }
    return false;
  }

  bool
  HLTGlobalStatus::accept() const
  {
    bool at_least_one_ran = false;
    bool at_least_one_accepted = false;
    unsigned const n = paths_.size();
    if (n == 0) {
      return true;
    }
    auto i = n;
    for (i = 0; i != n; ++i) {
      auto const s(paths_[i].state());
      if (s != hlt::Ready) {
        at_least_one_ran = true;
        if (s == hlt::Pass) {
          at_least_one_accepted = true;
        }
      }
    }
    if (!at_least_one_ran) {
      return true;
    }
    return at_least_one_accepted;
  }

  bool
  HLTGlobalStatus::error() const
  {
    auto const n = paths_.size();
    for (unsigned i = 0U; i != n; ++i) {
      if (paths_[i].state() == hlt::Exception) {
        return true;
      }
    }
    return false;
  }

  const HLTPathStatus&
  HLTGlobalStatus::at(unsigned const i) const
  {
    return paths_.at(i);
  }

  HLTPathStatus&
  HLTGlobalStatus::at(unsigned const i)
  {
    return paths_.at(i);
  }

  const HLTPathStatus& HLTGlobalStatus::operator[](unsigned const i) const
  {
    return paths_.at(i);
  }

  HLTPathStatus& HLTGlobalStatus::operator[](unsigned const i)
  {
    return paths_.at(i);
  }

  bool
  HLTGlobalStatus::wasrun(unsigned const i) const
  {
    return paths_.at(i).wasrun();
  }

  bool
  HLTGlobalStatus::accept(unsigned const i) const
  {
    return paths_.at(i).accept();
  }

  bool
  HLTGlobalStatus::error(unsigned const i) const
  {
    return paths_.at(i).error();
  }

  hlt::HLTState
  HLTGlobalStatus::state(unsigned const i) const
  {
    return paths_.at(i).state();
  }

  unsigned
  HLTGlobalStatus::index(unsigned const i) const
  {
    return paths_.at(i).index();
  }

  void
  HLTGlobalStatus::reset(unsigned const i)
  {
    paths_.at(i).reset();
  }

  // void
  // HLTGlobalStatus::
  // swap(HLTGlobalStatus& other)
  //{
  //  paths_.swap(other.paths_);
  //}

  // void
  // swap(HLTGlobalStatus& lhs, HLTGlobalStatus& rhs)
  //{
  //  lhs.swap(rhs);
  //}

  ostream&
  operator<<(ostream& ost, const HLTGlobalStatus& hlt)
  {
    vector<string> text{"n"s, "1"s, "0"s, "e"s};
    unsigned const n(hlt.size());
    for (auto i = 0U; i < n; ++i) {
      ost << text.at(hlt.state(i));
    }
    return ost;
  }

} // namespace art
