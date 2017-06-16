#include "canvas/Persistency/Provenance/Compatibility/type_aliases.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductIDStreamer.h"
#include "canvas/Utilities/Exception.h"

#include "TBuffer.h"
#include "TClassRef.h"

namespace art {

  void
  ProductIDStreamer::operator()(TBuffer& R_b, void* objp)
  {
    static TClassRef cl{"art::ProductID"};
    if (R_b.IsReading()) {
      UInt_t start, count;
      auto version = R_b.ReadVersion(&start, &count);
      if (version >= 10) {
        cl->ReadBuffer(R_b, objp, version, start, count);
        return;
      }

      // Extract the data members from the old version of the ProductID
      compatibility::ProcessIndex oldProcessIndex;
      compatibility::ProductIndex oldProductIndex;
      R_b >> oldProcessIndex >> oldProductIndex;

      auto obj = static_cast<ProductID*>(objp);
      // It is okay to return early without setting obj->value_ since
      // it defaults to an invalid value.

      if (branchIDLists_) {

        // The processIndex_ and productIndex_ values are indexed,
        // starting at 1, not 0.
        if (oldProcessIndex == 0 || oldProductIndex == 0) return;

        auto const& data = *branchIDLists_;
        if (oldProcessIndex <= data.size() &&
            oldProductIndex <= data[oldProcessIndex-1].size()) {
          obj->value_ = data[oldProcessIndex-1][oldProductIndex-1];
        }
      }
      else {
        throw Exception{errors::DataCorruption, "ProductID streamer:\n"}
        << "BranchIDLists not available for converting from obsolete ProductID schema to current one.\n";
      }
    }
    else {
      cl->WriteBuffer(R_b, objp);
    }
  }

  void configureProductIDStreamer(cet::exempt_ptr<BranchIDLists const> bidLists)
  {
    static TClassRef cl{"art::ProductID"};
    auto st = static_cast<ProductIDStreamer*>(cl->GetStreamer());
    if (st == nullptr) {
      cl->AdoptStreamer(new ProductIDStreamer{bidLists});
    } else {
      st->setBranchIDLists(bidLists);
    }
  }
}
