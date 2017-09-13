#ifndef canvas_Persistency_Provenance_ProductIDStreamer_h
#define canvas_Persistency_Provenance_ProductIDStreamer_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "cetlib/exempt_ptr.h"

#include "TClassStreamer.h"

class TBuffer;

namespace art {

void
configureProductIDStreamer(cet::exempt_ptr<BranchIDLists const> branchIDLists = {});

class ProductIDStreamer : public TClassStreamer {

public: // MEMBER FUNCTIONS -- Special Member Functions

  virtual
  ~ProductIDStreamer();

  explicit
  ProductIDStreamer(BranchIDLists const* branchIDLists = nullptr);

  ProductIDStreamer(ProductIDStreamer const&);

  ProductIDStreamer(ProductIDStreamer&&) = delete;

  ProductIDStreamer&
  operator=(ProductIDStreamer const&) = delete;

  ProductIDStreamer&
  operator=(ProductIDStreamer&&) = delete;

public: // MEMBER FUNCTIONS -- For the use of configureProductIDStreamer

  void
  setBranchIDLists(BranchIDLists const*);

public: // MEMBER FUNCTIONS -- Required by TClassStreamer API

  virtual
  TClassStreamer*
  Generate() const override;

  virtual
  void
  operator()(TBuffer&, void*) override;

private: // MEMBER DATA

  // Note: We do not own this.
  BranchIDLists const*
  branchIDLists_{nullptr};

};

} // namespace art

#endif /* canvas_Persistency_Provenance_ProductIDStreamer_h */

// Local Variables:
// mode: c++
// End:
