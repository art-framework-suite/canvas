#ifndef canvas_Persistency_Provenance_TypeLabel_h
#define canvas_Persistency_Provenance_TypeLabel_h

#include <string>
#include <tuple>
#include "canvas/Utilities/TypeID.h"

namespace art {

  class TypeLabel {
  public:

    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              std::string const& emulatedMod = {}) :
      typeID_{itemtype},
      productInstanceName_{instanceName},
      emulatedModule_{emulatedMod}
    {}

    auto const& typeID() const { return typeID_; }
    std::string className() const { return typeID_.className(); }
    std::string friendlyClassName() const { return typeID_.friendlyClassName(); }
    std::string const& emulatedModule() const { return emulatedModule_; }
    std::string const& productInstanceName() const { return productInstanceName_; }
    bool hasEmulatedModule() const { return !emulatedModule_.empty(); }

  private:
    TypeID      typeID_;
    std::string productInstanceName_;
    std::string emulatedModule_;
    friend bool operator<(TypeLabel const& a, TypeLabel const& b);
  };

  // Types with the same friendlyClassName are in the same equivalence
  // class for the purposes of this comparison.
  inline
  bool operator<(TypeLabel const& a, TypeLabel const& b)
  {
    auto const& a_class_name = a.className();
    auto const& b_class_name = b.className();
    return std::tie(a.emulatedModule_, a.productInstanceName_, a_class_name) <
           std::tie(b.emulatedModule_, b.productInstanceName_, b_class_name);
  }
}



#endif /* canvas_Persistency_Provenance_TypeLabel_h */

// Local Variables:
// mode: c++
// End:
