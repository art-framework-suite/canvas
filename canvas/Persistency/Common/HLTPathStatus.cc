#include "canvas/Persistency/Common/HLTPathStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTenums.h"
#include "cetlib/pow.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {
  constexpr auto path_size [[maybe_unused]] = cet::pow<14, size_t>(2);
}

namespace art {

  HLTPathStatus::~HLTPathStatus() = default;
  HLTPathStatus::HLTPathStatus() = default;

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state)
    : HLTPathStatus{state, 0}
  {}

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state, size_t const index)
    : status_{
        static_cast<std::uint16_t>(index << 2 | static_cast<size_t>(state))}
  {
    assert(state < hlt::HLTState::N_STATES);
    assert(index < path_size);
  }

  hlt::HLTState
  HLTPathStatus::state() const
  {
    return static_cast<hlt::HLTState>(status_ & 0x03);
  }

  unsigned
  HLTPathStatus::index() const
  {
    return static_cast<unsigned>(status_ >> 2);
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
