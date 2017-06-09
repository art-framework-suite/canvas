#ifndef canvas_Persistency_Provenance_ProductID_h
#define canvas_Persistency_Provenance_ProductID_h

//=====================================================================
// ProductID: A unique identifier for each EDProduct within a process.
//=====================================================================

#include "canvas/Persistency/Provenance/BranchID.h"
#include <iosfwd>

namespace art {

  typedef unsigned short ProcessIndex;
  typedef unsigned short ProductIndex;

  class ProductID {
  public:

    using value_type = BranchID::value_type;

    ProductID() = default;
    ProductID(value_type const value);

    bool isValid() const {return value_ != 0;}
    auto value() const {return value_;}

  private:
    value_type value_{};
  };

  inline
  bool operator==(ProductID const& lh, ProductID const& rh) {
    return lh.value() == rh.value();
  }
  inline
  bool operator!=(ProductID const& lh, ProductID const& rh) {
    return !(lh == rh);
  }

  inline
  bool operator<(ProductID const& lh, ProductID const& rh)
  {
    return lh.value() < rh.value();
  }

  std::ostream&
  operator<<(std::ostream& os, ProductID const& id);
}
#endif /* canvas_Persistency_Provenance_ProductID_h */

// Local Variables:
// mode: c++
// End:
