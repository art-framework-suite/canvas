#ifndef canvas_Persistency_Provenance_createProductLookups_h
#define canvas_Persistency_Provenance_createProductLookups_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

#include <utility>

namespace art {
  ProductLookup_t createProductLookups(ProductDescriptions const& descriptions);
}

#endif /* canvas_Persistency_Provenance_createProductLookups_h */

// Local Variables:
// mode: c++
// End:
