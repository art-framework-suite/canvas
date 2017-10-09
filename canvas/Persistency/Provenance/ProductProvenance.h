#ifndef canvas_Persistency_Provenance_ProductProvenance_h
#define canvas_Persistency_Provenance_ProductProvenance_h
// vim: set sw=2 expandtab :

//  The event dependent portion of the description of a product
//  and how it came into existence, plus the status.

#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductStatus.h"
#include "canvas/Persistency/Provenance/ProvenanceFwd.h"
#include "canvas/Persistency/Provenance/Transient.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace art {

  class ProductProvenance {

  public: // TYPES
    struct Transients {

    public:
      ~Transients();

      Transients();

      Transients(Transients const&);

      Transients(Transients&&);

      Transients& operator=(Transients const&);

      Transients& operator=(Transients&&);

    public:
      bool noParentage_{false};
      std::shared_ptr<Parentage> parentagePtr_{nullptr};
    };

  public: // MEMBER FUNCTIONS -- Special Member Functions
    ~ProductProvenance();

    ProductProvenance();

    ProductProvenance(ProductID const&, ProductStatus);

    ProductProvenance(ProductID const&,
                      ProductStatus,
                      std::vector<ProductID> const& parents);

    ProductProvenance(ProductProvenance const&);

    ProductProvenance(ProductProvenance&&);

    ProductProvenance& operator=(ProductProvenance const&);

    ProductProvenance& operator=(ProductProvenance&&);

  public: // MEMBER FUNCTIONS
    void write(std::ostream&) const;

    ProductID const& productID() const;

    ProductStatus const& productStatus() const;

    ParentageID const& parentageID() const;

    Parentage const& parentage() const;

    void setStatus(ProductStatus status) const;

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
    void setPresent() const;

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
    void setNeverCreated() const;

    // Note: This is true for Run, SubRun, and Results products.
    bool noParentage() const;

  private:
    ProductID productID_{};

    mutable ProductStatus productStatus_{productstatus::uninitialized()};

    ParentageID parentageID_{};

    mutable Transient<Transients> transients_{};
  };

  typedef std::vector<ProductProvenance> ProductProvenances;

  std::ostream& operator<<(std::ostream& os, ProductProvenance const& p);

  bool operator==(ProductProvenance const& a, ProductProvenance const& b);

  bool operator!=(ProductProvenance const& a, ProductProvenance const& b);

  bool operator<(ProductProvenance const& a, ProductProvenance const& b);

} // namespace art

#endif /* canvas_Persistency_Provenance_ProductProvenance_h */

// Local Variables:
// mode: c++
// End:
