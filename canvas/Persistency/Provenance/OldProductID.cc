#include "canvas/Persistency/Provenance/OldProductID.h"
#include <ostream>

namespace art {
  std::ostream&
  operator<<(std::ostream& os, OldProductID const& id) {
    os << id.processIndex() << ":" << id.productIndex();
    return os;
  }

  bool operator<(OldProductID const& lh, OldProductID const& rh) {
    return lh.processIndex() < rh.processIndex() ||
      (lh.processIndex() == rh.processIndex() && lh.productIndex() < rh.productIndex());
  }
}
