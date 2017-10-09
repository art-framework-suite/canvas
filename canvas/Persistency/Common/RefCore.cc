#include "canvas/Persistency/Common/RefCore.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"

#include <algorithm>
#include <cassert>

namespace art {

  RefCore::RefCoreTransients::RefCoreTransients()
    : itemPtr_(nullptr), prodGetter_(nullptr)
  {}

  RefCore::RefCoreTransients::RefCoreTransients(
    void const* prodPtr,
    EDProductGetter const* prodGetter)
    : itemPtr_(prodPtr), prodGetter_(prodGetter)
  {}

  RefCore::RefCore(ProductID const& id,
                   void const* prodPtr,
                   EDProductGetter const* prodGetter)
    : id_(id), transients_(prodPtr, prodGetter)
  {}

  bool
  RefCore::isAvailable() const
  {
    return productPtr() != 0 || (id_.isValid() && productGetter() != 0 &&
                                 productGetter()->getIt() != 0);
  }

  bool
  RefCore::isNonnull() const
  {
    return id_.isValid();
  }

  bool
  RefCore::isNull() const
  {
    return !isNonnull();
  }

  bool RefCore::operator!() const { return isNull(); }

  ProductID
  RefCore::id() const
  {
    return id_;
  }

  void const*
  RefCore::productPtr() const
  {
    return transients_.itemPtr_;
  }

  // Used by isAvailable()
  EDProductGetter const*
  RefCore::productGetter() const
  {
    return transients_.prodGetter_;
  }

  // Used by RefCoreStreamer on read to set itemPtr_
  // to the nullptr.
  void
  RefCore::setProductPtr(void const* prodPtr) const
  {
    transients_.itemPtr_ = prodPtr;
  }

  // Used by RefCoreStreamer on read.
  // Note: prodGetter_ is actually a Group for art,
  //       and a BranchData or AssnsBranchData for gallery.
  void
  RefCore::setProductGetter(EDProductGetter const* prodGetter)
  {
    transients_.prodGetter_ = prodGetter;
  }

  void
  RefCore::swap(RefCore& other)
  {
    std::swap(id_, other.id_);
    std::swap(transients_, other.transients_);
  }

  bool
  operator==(RefCore const& lhs, RefCore const& rhs)
  {
    return lhs.id() == rhs.id();
  }

  bool
  operator!=(RefCore const& lhs, RefCore const& rhs)
  {
    return !(lhs == rhs);
  }

  bool
  operator<(RefCore const& lhs, RefCore const& rhs)
  {
    return lhs.id() < rhs.id();
  }

  void
  swap(RefCore& lhs, RefCore& rhs)
  {
    lhs.swap(rhs);
  }

} // namespace art
