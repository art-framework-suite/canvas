#ifndef canvas_Persistency_Common_RefCore_h
#define canvas_Persistency_Common_RefCore_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProductID.h"

namespace art {

  class EDProduct;
  class EDProductGetter;

  class RefCore {
  public:
    struct RefCoreTransients {

      RefCoreTransients() noexcept;
      explicit RefCoreTransients(void const* prodPtr,
                                 EDProductGetter const* prodGetter) noexcept;

      mutable void const* itemPtr_{nullptr};
      EDProductGetter const* prodGetter_{nullptr};
    };

    RefCore();
    RefCore(ProductID const& theId,
            void const* prodPtr,
            EDProductGetter const* prodGetter);

    bool isNonnull() const noexcept;
    bool isNull() const noexcept;
    bool operator!() const noexcept;

    // Checks if collection is in memory or available in the Event; no
    // type checking is done.  Actually fetches the collection data
    // product using the productGetter()->getIt() if it has not been
    // fetched yet.
    bool isAvailable() const;

    ProductID id() const noexcept;
    void const* productPtr() const noexcept;

    // Used by isAvailable() to actually read the data product
    // using EDProductGeter::getIt().
    EDProductGetter const* productGetter() const noexcept;

    // Used by RefCoreStreamer on read to set itemPtr_ to the nullptr.
    void setProductPtr(void const* prodPtr) const noexcept;

    // Used by RefCoreStreamer on read.
    // Note: prodGetter_ is actually a Group for art,
    //       and a BranchData or AssnsBranchData for gallery.
    void setProductGetter(EDProductGetter const*) noexcept;
    void swap(RefCore& other);

  private:
    ProductID id_{};
    RefCoreTransients transients_{};
  };

  bool operator==(RefCore const&, RefCore const&);
  bool operator!=(RefCore const&, RefCore const&);
  bool operator<(RefCore const&, RefCore const&);

  void swap(RefCore&, RefCore&);

} // namespace art

#endif /* canvas_Persistency_Common_RefCore_h */

// Local Variables:
// mode: c++
// End:
