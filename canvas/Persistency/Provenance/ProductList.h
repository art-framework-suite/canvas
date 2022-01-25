#ifndef canvas_Persistency_Provenance_ProductList_h
#define canvas_Persistency_Provenance_ProductList_h
////////////////////////////////////////////////////////////////////////
// ProductList
//
// This is a very-badly-named typedef; please make it go away soon.
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/BranchKey.h"

#include <map>

namespace art {
  using ProductList = std::map<BranchKey, BranchDescription>;

  ProductDescriptions
  make_product_descriptions(ProductList const& productList);
}

#endif /* canvas_Persistency_Provenance_ProductList_h */

// Local Variables:
// mode: c++
// End:
