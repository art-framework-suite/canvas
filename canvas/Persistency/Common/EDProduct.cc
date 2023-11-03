// ======================================================================
//
// EDProduct: The base class of each type that will be inserted into the
//            Event.
//
// ======================================================================

#include "canvas/Persistency/Common/EDProduct.h"

using art::EDProduct;

EDProduct::~EDProduct() = default;

std::vector<void const*>
EDProduct::getView() const
{ /* should be called only polymorphically */
  return {};
}

std::type_info const*
EDProduct::typeInfo() const
{
  return typeInfo_();
}

void const*
EDProduct::getElementAddress(std::type_info const& toType,
                             unsigned long const index) const
{
  return do_getElementAddress(toType, index);
}

std::vector<void const*>
EDProduct::getElementAddresses(std::type_info const& toType,
                               std::vector<unsigned long> const& indices) const
{
  return do_getElementAddresses(toType, indices);
}

// ======================================================================
