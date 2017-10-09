#include "canvas/Persistency/Common/HLTPathStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTenums.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

using namespace std;

namespace art {

  HLTPathStatus::~HLTPathStatus() {}

  HLTPathStatus::HLTPathStatus() : status_(hlt::Ready) {}

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state) : status_(state)
  {
    assert(state < 4);
  }

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state, size_t const index)
    : status_((static_cast<uint16_t>(index) << 2) |
              static_cast<uint16_t>(state))
  {
    assert(state < 4);
    assert(index < (1 << 14));
  }

  hlt::HLTState
  HLTPathStatus::state() const
  {
    return static_cast<hlt::HLTState>(status_ & 0x03);
  }

  unsigned
  HLTPathStatus::index() const
  {
    return (static_cast<unsigned>(status_ >> 2));
  }

  void
  HLTPathStatus::reset()
  {
    status_ = 0;
  }

  bool
  HLTPathStatus::wasrun() const
  {
    return state() != hlt::Ready;
  }

  bool
  HLTPathStatus::accept() const
  {
    return !wasrun() || (state() == hlt::Pass);
  }

  bool
  HLTPathStatus::error() const
  {
    return state() == hlt::Exception;
  }

} // namespace art
