// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Utilities/Exception.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;

namespace art {

ProductProvenance::
Transients::
~Transients()
{
}

ProductProvenance::
Transients::
Transients()
  : noParentage_{false}
  , parentagePtr_{nullptr}
{
}

ProductProvenance::
Transients::
Transients(Transients const& rhs)
  : noParentage_{rhs.noParentage_}
  , parentagePtr_{rhs.parentagePtr_}
{
}

ProductProvenance::
Transients::
Transients(Transients&& rhs)
  : noParentage_{move(rhs.noParentage_)}
  , parentagePtr_{move(rhs.parentagePtr_)}
{
}

ProductProvenance::Transients&
ProductProvenance::
Transients::
operator=(Transients const& rhs)
{
  if (this != &rhs) {
    noParentage_ = rhs.noParentage_;
    parentagePtr_ = rhs.parentagePtr_;
  }
  return *this;
}

ProductProvenance::Transients&
ProductProvenance::
Transients::
operator=(Transients&& rhs)
{
  noParentage_ = move(rhs.noParentage_);
  parentagePtr_ = move(rhs.parentagePtr_);
  return *this;
}

ProductProvenance::
~ProductProvenance()
{
}

ProductProvenance::
ProductProvenance()
  : productID_{}
  , productStatus_{productstatus::uninitialized()}
  , parentageID_{}
  , transients_{}
{
}

ProductProvenance::
ProductProvenance(ProductID const& bid, ProductStatus const status)
  : productID_{bid}
  , productStatus_{status}
  , parentageID_{}
  , transients_{}
{
}

ProductProvenance::
ProductProvenance(ProductID const& bid, ProductStatus const status, vector<ProductID> const& parents)
  : productID_{bid}
  , productStatus_{status}
  , parentageID_{}
  , transients_{}
{
  transients_.get().parentagePtr_ = make_shared<Parentage>(parents);
  parentageID_ = transients_.get().parentagePtr_->id();
  ParentageRegistry::emplace(parentageID_, *transients_.get().parentagePtr_);
}

ProductProvenance::
ProductProvenance(ProductProvenance const& rhs)
  : productID_{rhs.productID_}
  , productStatus_{rhs.productStatus_}
  , parentageID_{rhs.parentageID_}
  , transients_{rhs.transients_}
{
}

ProductProvenance::
ProductProvenance(ProductProvenance&& rhs)
  : productID_{move(rhs.productID_)}
  , productStatus_{move(rhs.productStatus_)}
  , parentageID_{move(rhs.parentageID_)}
  , transients_{move(rhs.transients_)}
{
}

ProductProvenance&
ProductProvenance::
operator=(ProductProvenance const& rhs)
{
  if (this != &rhs) {
    productID_ = rhs.productID_;
    productStatus_ = rhs.productStatus_;
    parentageID_ = rhs.parentageID_;
    transients_ = rhs.transients_;
  }
  return *this;
}

ProductProvenance&
ProductProvenance::
operator=(ProductProvenance&& rhs)
{
  productID_ = move(rhs.productID_);
  productStatus_ = move(rhs.productStatus_);
  parentageID_ = move(rhs.parentageID_);
  transients_ = move(rhs.transients_);
  return *this;
}

ProductID const&
ProductProvenance::
productID() const
{
  return productID_;
}

ProductStatus const&
ProductProvenance::
productStatus() const
{
  return productStatus_;
}

ParentageID const&
ProductProvenance::
parentageID() const
{
  return parentageID_;
}

void
ProductProvenance::
setStatus(ProductStatus status) const
{
  productStatus_ = status;
}

// Note: This is true for Run, SubRun, and Results products.
bool
ProductProvenance::
noParentage() const
{
  return transients_.get().noParentage_;
}

Parentage const&
ProductProvenance::
parentage() const
{
  if (!transients_.get().parentagePtr_) {
    transients_.get().parentagePtr_ = make_shared<Parentage>();
    ParentageRegistry::get(parentageID_, *transients_.get().parentagePtr_);
  }
  return *transients_.get().parentagePtr_;
}

// Used only by Group::status() to override productstatus::unknown
// with the value of the Wrapper present flag.
// Note: This should never happen! But a mistake in RootDelayedReader
//       would make it happen if there were two or more run/subrun
//       fragments in the file index with the same run/subrun number
//       where the first fragment had a product with an invalid range
//       (because RootOutputFile changed it to invalid to prevent
//       double-counting when combining products), and a later fragment
//       had the same product with a valid range.
//       RootDelayedReader would return the fragment product with the
//       valid range as the combined product, but it forgot to update
//       provenance, so we would have a product where the wrapper
//       present flag was true, but the product provenance status was
//       unknown instead of present.  This has since been fixed.
//       We leave this routine here so old files with the problem can
//       still be read.
void
ProductProvenance::
setPresent() const
{
  if (productstatus::present(productStatus_)) {
    return;
  }
  assert(productstatus::unknown(productStatus_) || productstatus::dummyToPreventDoubleCount(productStatus_));
  setStatus(productstatus::present());
}

// Used only by Group::status() to override productstatus::unknown
// with the value of the Wrapper present flag.
// Note: This should never happen! But a mistake in RootOutputFile
//       would make it happen when it changed a run/subrun product
//       range set to invalid to prevent double-counting when combining
//       products. It would change the product status to unknown,
//       replace it with a dummy object, and write out the dummy with
//       the unknown status in the provenance. This has since been fixed
//       to write out the dummy with status neverCreated.
//       We leave this routine here so old files with the problem can
//       still be read.
void
ProductProvenance::
setNeverCreated() const
{
  assert(productstatus::unknown(productStatus_) || productstatus::dummyToPreventDoubleCount(productStatus_));
  setStatus(productstatus::neverCreated());
}

void
ProductProvenance::
write(ostream& os) const
{
  os << "product ID = " << productID() << '\n';
  os << "product status = " << static_cast<int>(productStatus_) << '\n';
  if (!noParentage()) {
    os << "entry description ID = " << parentageID() << '\n';
  }
}

ostream&
operator<<(ostream& os, ProductProvenance const& p)
{
  p.write(os);
  return os;
}

bool
operator==(ProductProvenance const& a, ProductProvenance const& b)
{
  if (a.noParentage() != b.noParentage()) {
    return false;
  }
  if (a.noParentage()) {
    return (a.productID() == b.productID()) && (a.productStatus() == b.productStatus());
  }
  return (a.productID() == b.productID()) && (a.productStatus() == b.productStatus()) && (a.parentageID() == b.parentageID());
}

bool
operator!=(ProductProvenance const& a, ProductProvenance const& b)
{
  return !(a == b);
}

// FIXME: This is not compatible with operator=.  That is if !(a<b) && !(a>b) does not imply (a==b)!!!
bool
operator<(ProductProvenance const& a, ProductProvenance const& b)
{
  return a.productID() < b.productID();
}

} // namespace art

