#ifndef canvas_Persistency_Provenance_BranchIDList_h
#define canvas_Persistency_Provenance_BranchIDList_h

/*----------------------------------------------------------------------

BranchIDList:
BranchIDLists:
        one table stored per File
        table BranchIDLists keyed by ProcessInfo::branchListIndex_;
        entry BranchIDList keyed by ProductID::productIndex_;

----------------------------------------------------------------------*/

#include <vector>
#include "canvas/Persistency/Provenance/BranchID.h"

namespace art {
  using BranchIDList = std::vector<BranchID::value_type>;
  using BranchIDLists = std::vector<BranchIDList>;
}
#endif /* canvas_Persistency_Provenance_BranchIDList_h */

// Local Variables:
// mode: c++
// End:
