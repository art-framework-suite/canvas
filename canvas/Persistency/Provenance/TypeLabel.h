#ifndef canvas_Persistency_Provenance_TypeLabel_h
#define canvas_Persistency_Provenance_TypeLabel_h

#include <string>
#include "canvas/Utilities/TypeID.h"

namespace art {

  struct TypeLabel {
    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              std::string const& emulatedMod = std::string()) :
      typeID(itemtype),
      productInstanceName(instanceName),
      emulatedModule(emulatedMod)
    {}

    std::string className() const { return typeID.className(); }
    std::string friendlyClassName() const { return typeID.friendlyClassName(); }
    bool hasEmulatedModule() const { return !emulatedModule.empty(); }

    TypeID      typeID;
    std::string productInstanceName;
    std::string emulatedModule;
  };

  // Types with the same friendlyClassName are in the same equivalence
  // class for the purposes of this comparison.
  inline
  bool operator<(TypeLabel const &a, TypeLabel const &b) {
    return (a.emulatedModule != b.emulatedModule) ? a.emulatedModule < b.emulatedModule :
      (a.productInstanceName != b.productInstanceName) ? a.productInstanceName < b.productInstanceName :
      a.className() < b.className();
  }
}



#endif /* canvas_Persistency_Provenance_TypeLabel_h */

// Local Variables:
// mode: c++
// End:
