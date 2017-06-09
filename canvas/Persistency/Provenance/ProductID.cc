#include "canvas/Persistency/Provenance/ProductID.h"

#include <ostream>

namespace art {
  ProductID::ProductID(value_type const value) :
    value_{value}
  {}

  std::ostream& operator<<(std::ostream& os, ProductID const& id)
  {
    os << id.value();
    return os;
  }
}
