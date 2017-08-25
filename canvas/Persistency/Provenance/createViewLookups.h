#ifndef canvas_Persistency_Provenance_createViewLookups_h
#define canvas_Persistency_Provenance_createViewLookups_h

#include "canvas/Persistency/Provenance/ProductList.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

#include <utility>

namespace art {
  ViewLookup_t createViewLookups(ProductList const& products);
}

#endif /* canvas_Persistency_Provenance_createViewLookups_h */

// Local Variables:
// mode: c++
// End:
