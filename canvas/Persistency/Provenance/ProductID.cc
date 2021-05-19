#include "canvas/Persistency/Provenance/ProductID.h"
#include "cetlib/crc32.h"

#include <ostream>

namespace art {

  ProductID::ProductID(std::string const& canonicalProductName)
    : ProductID{toID(canonicalProductName)}
  {}

  void
  ProductID::setID(std::string const& canonicalProductName)
  {
    value_ = toID(canonicalProductName);
  }

  ProductID::value_type
  ProductID::toID(std::string const& canonicalProductName)
  {
    return cet::crc32{canonicalProductName}.digest();
  }

  std::ostream&
  operator<<(std::ostream& os, ProductID const id)
  {
    os << id.value();
    return os;
  }
}
