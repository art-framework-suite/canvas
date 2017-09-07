#ifndef canvas_Persistency_Provenance_BranchDescriptionStreamer_h
#define canvas_Persistency_Provenance_BranchDescriptionStreamer_h

#include "TBuffer.h"
#include "TClassStreamer.h"

namespace art {
  class BranchDescription;
  namespace detail {
    class BranchDescriptionStreamer : public TClassStreamer {
    public:
      void operator()(TBuffer&, void* objp) override;
      static void fluffRootTransients(BranchDescription const&);
    };

    void setBranchDescriptionStreamer();
  }
}

#endif /* canvas_Persistency_Provenance_BranchDescriptionStreamer_h */

// Local Variables:
// mode: c++
// End:
