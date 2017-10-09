#ifndef canvas_Persistency_Common_RefCore_h
#define canvas_Persistency_Common_RefCore_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProductID.h"

namespace art {

  class EDProduct;
  class EDProductGetter;

  class RefCore {

  public: // TYPES
    struct RefCoreTransients {

      RefCoreTransients();
      explicit RefCoreTransients(void const* prodPtr,
                                 EDProductGetter const* prodGetter);

      mutable void const* itemPtr_;
      EDProductGetter const* prodGetter_;
    };

  public: // MEMBER FUNCTIONS -- Special Member Functions
    RefCore() = default;

    RefCore(ProductID const& theId,
            void const* prodPtr,
            EDProductGetter const* prodGetter);

  public:
    bool isNonnull() const;

    bool isNull() const;

    bool operator!() const;

    // Checks if collection is in memory or available
    // in the Event; no type checking is done.
    // Actually fetches the collection data product
    // using the productGetter()->getIt() if it has not
    // been fetched yet.
    bool isAvailable() const;

    ProductID id() const;

    void const* productPtr() const;

    // Used by isAvailable() to actually read the data product
    // using EDProductGeter::getIt().
    EDProductGetter const* productGetter() const;

    // Used by RefCoreStreamer on read to set itemPtr_
    // to the nullptr.
    void setProductPtr(void const* prodPtr) const;

    // Used by RefCoreStreamer on read.
    // Note: prodGetter_ is actually a Group for art,
    //       and a BranchData or AssnsBranchData for gallery.
    void setProductGetter(EDProductGetter const*);

    void swap(RefCore& other);

  private:
    ProductID id_;
    RefCoreTransients transients_;
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
