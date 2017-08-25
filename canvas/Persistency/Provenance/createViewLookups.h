#ifndef canvas_Persistency_Provenance_createViewLookups_h
#define canvas_Persistency_Provenance_createViewLookups_h

#include "canvas/Persistency/Provenance/ProductList.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

#include <utility>

namespace art {
  ViewLookup_t createViewLookups(ProductList const& products);

  namespace root {
    // With ROOT, we have access to the list of base classes for a
    // given type, enabling us to produce a lookup of the same form as
    // a product lookup.
    ViewLookup_t createViewLookups(ProductList const& products);
  }
}

#endif /* canvas_Persistency_Provenance_createViewLookups_h */

// Local Variables:
// mode: c++
// End:
