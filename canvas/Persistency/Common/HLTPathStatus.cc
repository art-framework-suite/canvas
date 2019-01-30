#include "canvas/Persistency/Common/HLTPathStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTenums.h"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;

#define ATOMIC_PTR_CAST_PSTATUS_LVAL(V1)                                       \
  reinterpret_cast<atomic<decltype(status_)>*>((decltype(status_)*)&(V1))

#define ATOMIC_LOAD_PSTATUS_LVAL(V2)                                           \
  atomic_load(ATOMIC_PTR_CAST_PSTATUS_LVAL(V2))

#define CAST_STATUS_RVAL(RVAL) ((decltype(status_))RVAL)

#define ATOMIC_STORE_PSTATUS_LVAL(V2, VAL)                                     \
  atomic_store(ATOMIC_PTR_CAST_PSTATUS_LVAL(V2), CAST_STATUS_RVAL((VAL)))

namespace art {

  HLTPathStatus::~HLTPathStatus() {}

  HLTPathStatus::HLTPathStatus()
  {
    ATOMIC_STORE_PSTATUS_LVAL(status_, CAST_STATUS_RVAL(hlt::Ready));
  }

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state)
  {
    assert(state < hlt::HLTState::N_STATES);
    ATOMIC_STORE_PSTATUS_LVAL(status_, CAST_STATUS_RVAL(state));
  }

  HLTPathStatus::HLTPathStatus(hlt::HLTState const state, size_t const index)
  {
    ATOMIC_STORE_PSTATUS_LVAL(
      status_, ((CAST_STATUS_RVAL(index) << 2) | CAST_STATUS_RVAL(state)));
    assert(state < hlt::HLTState::N_STATES);
    assert(index < (1 << 14));
  }

  HLTPathStatus::HLTPathStatus(HLTPathStatus const& rhs)
  {
    ATOMIC_STORE_PSTATUS_LVAL(status_, ATOMIC_LOAD_PSTATUS_LVAL(rhs.status_));
  }

  HLTPathStatus::HLTPathStatus(HLTPathStatus&& rhs)
  {
    ATOMIC_STORE_PSTATUS_LVAL(status_,
                              move(ATOMIC_LOAD_PSTATUS_LVAL(rhs.status_)));
  }

  HLTPathStatus&
  HLTPathStatus::operator=(HLTPathStatus const& rhs)
  {
    if (this != &rhs) {
      ATOMIC_STORE_PSTATUS_LVAL(status_, ATOMIC_LOAD_PSTATUS_LVAL(rhs.status_));
    }
    return *this;
  }

  HLTPathStatus&
  HLTPathStatus::operator=(HLTPathStatus&& rhs)
  {
    ATOMIC_STORE_PSTATUS_LVAL(status_,
                              move(ATOMIC_LOAD_PSTATUS_LVAL(rhs.status_)));
    return *this;
  }

  hlt::HLTState
  HLTPathStatus::state() const
  {
    auto ret =
      static_cast<hlt::HLTState>(ATOMIC_LOAD_PSTATUS_LVAL(status_) & 0x03);
    return ret;
  }

  unsigned
  HLTPathStatus::index() const
  {
    auto ret = (static_cast<unsigned>(ATOMIC_LOAD_PSTATUS_LVAL(status_) >> 2));
    return ret;
  }

  void
  HLTPathStatus::reset()
  {
    ATOMIC_STORE_PSTATUS_LVAL(status_, 0);
  }

  bool
  HLTPathStatus::wasrun() const
  {
    auto ret = state() != hlt::Ready;
    return ret;
  }

  bool
  HLTPathStatus::accept() const
  {
    auto ret = !wasrun() || (state() == hlt::Pass);
    return ret;
  }

  bool
  HLTPathStatus::error() const
  {
    auto ret = state() == hlt::Exception;
    return ret;
  }

} // namespace art
