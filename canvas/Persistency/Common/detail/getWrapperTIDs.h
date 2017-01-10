#ifndef QKQKQKQKQKQDFIUSA
#define QKQKQKQKQKQDFIUSA

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Utilities/TypeID.h"

#include <vector>

namespace art {
  namespace detail { // Internal use only.
    std::vector<art::TypeID> getWrapperTIDs(BranchDescription const& bd);
  }
}

#endif

