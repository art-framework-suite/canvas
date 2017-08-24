#include "canvas/Persistency/Provenance/TypeLabel.h"

#include <tuple>

namespace art {

  TypeLabel::TypeLabel(TypeID const& itemtype,
                       std::string const& instanceName,
                       bool const supportsView) :
    typeID_{itemtype},
    productInstanceName_{instanceName},
    supportsView_{supportsView}
  {}

  TypeLabel::TypeLabel(TypeID const& itemtype,
                       std::string const& instanceName,
                       bool const supportsView,
                       std::string emulatedModule) :
    typeID_{itemtype},
    productInstanceName_{instanceName},
    supportsView_{supportsView},
    emulatedModule_{std::make_unique<std::string>(std::move(emulatedModule))}
  {}

  std::string const&
  TypeLabel::emulatedModule() const
  {
    if (!emulatedModule_) {
      throw art::Exception(art::errors::NullPointerError, "TypeLabel::emulatedModule\n")
        << "Attempt to retrieve an emulated module name when one does not exist.\n";
    }
    return *emulatedModule_;
  }

  // Types with the same friendlyClassName are in the same equivalence
  // class for the purposes of this comparison.
  bool operator<(TypeLabel const& a, TypeLabel const& b)
  {
    auto const& a_class_name = a.className();
    auto const& b_class_name = b.className();
    return std::tie(a.emulatedModule_, a.productInstanceName_, a_class_name) <
           std::tie(b.emulatedModule_, b.productInstanceName_, b_class_name);
  }
}
