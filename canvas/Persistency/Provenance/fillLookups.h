#ifndef canvas_Persistency_Provenance_fillLookups_h
#define canvas_Persistency_Provenance_fillLookups_h

#include "canvas/Persistency/Provenance/ProductList.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

#include <utility>

namespace art {
  namespace detail {
    std::pair<ProductLookup_t, ViewLookup_t> fillLookups(ProductList const& products);
  }
}

#endif /* canvas_Persistency_Provenance_fillLookups_h */

// Local Variables:
// mode: c++
// End:
