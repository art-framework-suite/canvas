#include "canvas/Persistency/Provenance/ProductID.h"

#include <ostream>
#include <tuple>

namespace art {
  std::ostream& operator<<(std::ostream& os, ProductID const& id)
  {
    os << id.processIndex() << ":" << id.productIndex();
    return os;
  }

  bool operator<(ProductID const& lh, ProductID const& rh)
  {
    return std::tie(lh.processIndex_, lh.productIndex_) <
           std::tie(rh.processIndex_, rh.productIndex_);
  }
}
