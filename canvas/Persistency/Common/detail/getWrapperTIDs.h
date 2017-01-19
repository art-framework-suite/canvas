#ifndef canvas_Persistency_Common_detail_getWrapperTIDs_h
#define canvas_Persistency_Common_detail_getWrapperTIDs_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Utilities/TypeID.h"

#include <vector>

namespace art {
  namespace detail { // Internal use only.
    std::vector<TypeID> getWrapperTIDs(BranchDescription const& bd);
    std::vector<TypeID> getWrapperTIDs(std::string const & productClassName);
  }
}

inline
auto
art::detail::getWrapperTIDs(BranchDescription const & bd)
-> std::vector<TypeID>
{
  return getWrapperTIDs(bd.producedClassName());
}

#endif /* canvas_Persistency_Common_detail_getWrapperTIDs_h */


// Local Variables:
// mode: c++
// End:
