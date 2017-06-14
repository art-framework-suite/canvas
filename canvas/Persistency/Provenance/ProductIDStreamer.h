#ifndef canvas_Persistency_Common_ProductIDStreamer_h
#define canvas_Persistency_Common_ProductIDStreamer_h

#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "cetlib/exempt_ptr.h"

#include "TClassStreamer.h"

class TBuffer;

namespace art {

  void
  configureProductIDStreamer(cet::exempt_ptr<BranchIDLists const> branchIDLists = {});

  class ProductIDStreamer : public TClassStreamer {
  public:

    explicit ProductIDStreamer(cet::exempt_ptr<BranchIDLists const> branchIDLists = {}) : branchIDLists_{branchIDLists}
    {}

    void setBranchIDLists(cet::exempt_ptr<BranchIDLists const> bidLists)
    {
      branchIDLists_ = bidLists;
    }

    void operator() (TBuffer& R_b, void* objp) override;

  private:
    cet::exempt_ptr<BranchIDLists const> branchIDLists_;
  };
}
#endif /* canvas_Persistency_Common_ProductIDStreamer_h */

// Local Variables:
// mode: c++
// End:
