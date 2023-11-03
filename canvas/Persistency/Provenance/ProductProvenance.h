#ifndef canvas_Persistency_Provenance_ProductProvenance_h
#define canvas_Persistency_Provenance_ProductProvenance_h
// vim: set sw=2 expandtab :

//  The event dependent portion of the description of a product
//  and how it came into existence, plus the status.

#include "canvas/Persistency/Provenance/Parentage.h"
#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductStatus.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include "canvas/Persistency/Provenance/fwd.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace art {
  class ProductProvenance {
  public:
    struct Transients {
      bool noParentage_{true};
      Parentage parentage_{};
    };

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

    void write(std::ostream&) const;
    ProductID productID() const noexcept;
    ProductStatus productStatus() const noexcept;
    ParentageID const& parentageID() const noexcept;
    Parentage const& parentage() const;
    // Note: This is true for Run, SubRun, and Results products.
    bool noParentage() const noexcept;

  private:
    ProductID productID_{};
    ProductStatus productStatus_{productstatus::uninitialized()};
    ParentageID parentageID_{};
    mutable Transient<Transients> transients_{};
  };

  using ProductProvenances = std::vector<ProductProvenance>;

  std::ostream& operator<<(std::ostream& os, ProductProvenance const& p);
  bool operator==(ProductProvenance const& a,
                  ProductProvenance const& b) noexcept;
  bool operator!=(ProductProvenance const& a,
                  ProductProvenance const& b) noexcept;
  bool operator<(ProductProvenance const& a,
                 ProductProvenance const& b) noexcept;

} // namespace art

#endif /* canvas_Persistency_Provenance_ProductProvenance_h */

// Local Variables:
// mode: c++
// End:
