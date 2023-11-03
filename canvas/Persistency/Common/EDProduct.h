#ifndef canvas_Persistency_Common_EDProduct_h
#define canvas_Persistency_Common_EDProduct_h

// ======================================================================
//
// EDProduct: The base class of each type that will be inserted into the
//            Event.
//
// ======================================================================

#include "canvas/Persistency/Common/types.h"

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace art {
  class EDProduct;
  class InputTag;
  class SubRunID;
}

// ======================================================================

class art::EDProduct {
public:
  virtual ~EDProduct();

  bool
  isPresent() const
  {
    return isPresent_();
  }

  // We use vector<void*> so as to keep the type information out
  // of the EDProduct class.
  virtual std::vector<void const*> getView() const;
  virtual std::type_info const* typeInfo() const;

  void const* getElementAddress(std::type_info const& toType,
                                unsigned long index) const;

  std::vector<void const*> getElementAddresses(
    std::type_info const& toType,
    std::vector<unsigned long> const& indices) const;

  virtual std::string
  productSize() const
  {
    return "-";
  }

  product_typeids_t
  getTypeIDs() const
  {
    return do_getTypeIDs();
  }

  std::unique_ptr<EDProduct>
  makePartner(std::type_info const& wanted_type) const
  {
    return do_makePartner(wanted_type);
  }

  unsigned
  getRangeSetID() const
  {
    return do_getRangeSetID();
  }

  void
  setRangeSetID(unsigned const id)
  {
    do_setRangeSetID(id);
  }

  void
  combine(EDProduct* p)
  {
    do_combine(p);
  }

  std::unique_ptr<EDProduct>
  createEmptySampledProduct(InputTag const& tag) const
  {
    return do_createEmptySampledProduct(tag);
  }

  void
  insertIfSampledProduct(std::string const& dataset,
                         SubRunID const& id,
                         std::unique_ptr<EDProduct> product)
  {
    return do_insertIfSampledProduct(dataset, id, std::move(product));
  }

private:
  virtual product_typeids_t do_getTypeIDs() const = 0;
  virtual std::unique_ptr<EDProduct> do_makePartner(
    std::type_info const& wanted_type) const = 0;

  virtual unsigned do_getRangeSetID() const = 0;
  virtual void do_setRangeSetID(unsigned) = 0;
  virtual void do_combine(EDProduct const*) = 0;

  virtual void const* do_getElementAddress(std::type_info const& toType,
                                           unsigned long index) const = 0;

  virtual std::vector<void const*> do_getElementAddresses(
    std::type_info const& toType,
    std::vector<unsigned long> const& indices) const = 0;

  virtual std::unique_ptr<EDProduct> do_createEmptySampledProduct(
    InputTag const& tag) const = 0;

  virtual void do_insertIfSampledProduct(
    std::string const& dataset,
    SubRunID const& id,
    std::unique_ptr<EDProduct> product) = 0;

  virtual bool isPresent_() const = 0;
  virtual std::type_info const* typeInfo_() const = 0;

}; // EDProduct

// ======================================================================

#endif /* canvas_Persistency_Common_EDProduct_h */

// Local Variables:
// mode: c++
// End:
