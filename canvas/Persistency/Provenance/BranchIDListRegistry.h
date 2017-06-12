#ifndef canvas_Persistency_Provenance_BranchIDListRegistry_h
#define canvas_Persistency_Provenance_BranchIDListRegistry_h

//=====================================================================
// BranchIDListRegistry: This is a thread-UNSAFE registry.  Access to
//                       it must be, and is intended to be, serialized.
// ====================================================================

#include "canvas/Persistency/Provenance/BranchIDList.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include <memory>
#include <vector>

namespace art {

  class BranchIDListRegistry {
  public:

    using collection_type = std::vector<BranchIDList>;

    auto size() const { return data_.size(); }
    auto const& data() const { return data_; }

    static BranchIDListRegistry& instance();
    static void resetFromFile(std::unique_ptr<BranchIDLists>&& file_bidlists);

  private:

    BranchIDListRegistry() = default;
    ~BranchIDListRegistry() = default;

    // Not copyable:
    BranchIDListRegistry(BranchIDListRegistry const&) = delete;
    BranchIDListRegistry& operator= (BranchIDListRegistry const&) = delete;

    collection_type data_ {};
    static BranchIDListRegistry* instance_;
  };

}  // art

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Persistency_Provenance_BranchIDListRegistry_h */
