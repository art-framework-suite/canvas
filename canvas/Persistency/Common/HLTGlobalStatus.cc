#include "canvas/Persistency/Common/HLTGlobalStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTenums.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace literals::string_literals;

namespace art {

  HLTGlobalStatus::~HLTGlobalStatus() = default;

  HLTGlobalStatus::HLTGlobalStatus(std::size_t const n /* = 0 */) : paths_(n) {}

  HLTGlobalStatus::HLTGlobalStatus(HLTGlobalStatus const&) = default;
  HLTGlobalStatus::HLTGlobalStatus(HLTGlobalStatus&&) = default;

  HLTGlobalStatus& HLTGlobalStatus::operator=(HLTGlobalStatus const&) = default;
  HLTGlobalStatus& HLTGlobalStatus::operator=(HLTGlobalStatus&&) = default;

  std::size_t
  HLTGlobalStatus::size() const
  {
    return paths_.size();
  }

  void
  HLTGlobalStatus::reset()
  {
    std::for_each(begin(paths_), end(paths_), [](auto& path_status) {
      path_status.reset();
    });
  }

  bool
  HLTGlobalStatus::wasrun() const
  {
    return std::any_of(begin(paths_), end(paths_), [](auto const& path_status) {
      return path_status.state() != hlt::Ready;
    });
  }

  bool
  HLTGlobalStatus::accept() const
  {
    if (empty(paths_)) {
      return true;
    }

    bool at_least_one_ran = false;
    bool at_least_one_accepted = false;

    for (auto const& path_status : paths_) {
      auto const s(path_status.state());
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
    return std::any_of(begin(paths_), end(paths_), [](auto const& path_status) {
      return path_status.state() == hlt::Exception;
    });
  }

  HLTPathStatus const&
  HLTGlobalStatus::at(unsigned const i) const
  {
    return paths_.at(i);
  }

  HLTPathStatus&
  HLTGlobalStatus::at(unsigned const i)
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
    atomic_thread_fence(memory_order_seq_cst);
  }

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
