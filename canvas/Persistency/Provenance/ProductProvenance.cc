#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Utilities/Exception.h"

#include <cassert>
#include <ostream>

namespace art {

  ProductProvenance::ProductProvenance(BranchID const& bid) :
    branchID_{bid}
  {}

  ProductProvenance::ProductProvenance(BranchID const& bid,
                                       ProductStatus const status) :
    branchID_{bid},
    productStatus_{status}
  {}

  ProductProvenance::ProductProvenance(BranchID const& bid,
                                       ProductStatus const status,
                                       ParentageID const& edid) :
    branchID_{bid},
    productStatus_{status},
    parentageID_{edid}
  {}

  ProductProvenance::ProductProvenance(BranchID const& bid,
                                       ProductStatus const status,
                                       std::shared_ptr<Parentage> pPtr) :
    branchID_{bid},
    productStatus_{status},
    parentageID_{pPtr->id()}
  {
    parentagePtr() = pPtr;
    ParentageRegistry::emplace(parentageID_, *pPtr);
  }

  ProductProvenance::ProductProvenance(BranchID const& bid,
                                       ProductStatus const status,
                                       std::vector<BranchID> const& parents) :
    branchID_{bid},
    productStatus_{status}
  {
    parentagePtr() = std::make_shared<Parentage>();
    parentagePtr()->parents() = parents;
    parentageID_ = parentagePtr()->id();
    ParentageRegistry::emplace(parentageID_, *parentagePtr());
  }

  Parentage const&
  ProductProvenance::parentage() const {
    if (!parentagePtr()) {
      parentagePtr() = std::make_shared<Parentage>();
      auto parIt = ParentageRegistry::find(parentageID_);
      if (parIt != ParentageRegistry::cend()) {
        *parentagePtr() = parIt->second;
      }
    }
    return *parentagePtr();
  }

  void
  ProductProvenance::setPresent() const {
    if (productstatus::present(productStatus())) return;
    assert(productstatus::unknown(productStatus()));
    setStatus(productstatus::present());
  }

  void
  ProductProvenance::setNotPresent() const {
    if (productstatus::neverCreated(productStatus())) return;
    if (productstatus::dropped(productStatus())) return;
    assert(productstatus::unknown(productStatus()));
    setStatus(productstatus::neverCreated());
  }

  void
  ProductProvenance::write(std::ostream& os) const {
    os << "branch ID = " << branchID() << '\n';
    os << "product status = " << static_cast<int>(productStatus()) << '\n';
    if (!noParentage()) {
      os << "entry description ID = " << parentageID() << '\n';
    }
  }

  // Only the 'salient attributes' are tested in equality comparison.
  bool
  operator==(ProductProvenance const& a, ProductProvenance const& b) {
    if (a.noParentage() != b.noParentage()) return false;
    if (a.noParentage()) {
      return
        a.branchID() == b.branchID()
        && a.productStatus() == b.productStatus();
    }
    return
      a.branchID() == b.branchID()
      && a.productStatus() == b.productStatus()
      && a.parentageID() == b.parentageID();
  }
}
