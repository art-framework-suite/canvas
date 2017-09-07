#include "canvas/Persistency/Provenance/ProductIDStreamer.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "canvas/Persistency/Provenance/Compatibility/type_aliases.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/exempt_ptr.h"

#include "TBuffer.h"
#include "TClassRef.h"
#include "TClassStreamer.h"

using namespace std;

namespace art {

ProductIDStreamer::
~ProductIDStreamer()
{
  branchIDLists_ = nullptr;
}

ProductIDStreamer::
ProductIDStreamer(BranchIDLists const* branchIDLists /* = nullptr */)
  : TClassStreamer()
  , branchIDLists_{branchIDLists}
{
}

ProductIDStreamer::
ProductIDStreamer(ProductIDStreamer const& rhs)
  : TClassStreamer(rhs)
  , branchIDLists_{rhs.branchIDLists_}
{
}

void
ProductIDStreamer::
setBranchIDLists(BranchIDLists const* bidLists)
{
  branchIDLists_ = bidLists;
}

TClassStreamer*
ProductIDStreamer::
Generate() const
{
  return new ProductIDStreamer{*this};
}

void
ProductIDStreamer::
operator()(TBuffer& R_b, void* objp)
{
  static TClassRef cl{"art::ProductID"};
  if (R_b.IsReading()) {
    UInt_t start = 0;
    UInt_t count = 0;
    auto version = R_b.ReadVersion(&start, &count);
    if (version >= 10) {
      cl->ReadBuffer(R_b, objp, version, start, count);
      return;
    }
    compatibility::ProcessIndex oldProcessIndex;
    compatibility::ProductIndex oldProductIndex;
    R_b >> oldProcessIndex;
    R_b >> oldProductIndex;
    auto obj = static_cast<ProductID*>(objp);
    // It is okay to return early without setting obj->value_ since
    // it defaults to an invalid value.
    if (branchIDLists_) {
      // The processIndex_ and productIndex_ values
      // are indexed starting at 1, not 0.
      if ((oldProcessIndex == 0) || (oldProductIndex == 0)) {
        return;
      }
      if ((oldProcessIndex <= branchIDLists_->size()) && (oldProductIndex <= (*branchIDLists_)[oldProcessIndex-1].size())) {
        obj->value_ = (*branchIDLists_)[oldProcessIndex-1][oldProductIndex-1];
      }
      return;
    }
    throw Exception{errors::DataCorruption, "ProductID streamer:"}
        << "BranchIDLists not available for converting from obsolete ProductID schema to current one.\n";
  }
  else {
    cl->WriteBuffer(R_b, objp);
  }
}

void
configureProductIDStreamer(cet::exempt_ptr<BranchIDLists const> bidLists)
{
  static TClassRef cl{"art::ProductID"};
  auto st = static_cast<ProductIDStreamer*>(cl->GetStreamer());
  if (st == nullptr) {
    cl->AdoptStreamer(new ProductIDStreamer{bidLists.get()});
  }
  else {
    st->setBranchIDLists(bidLists.get());
  }
}

} // namespace art

