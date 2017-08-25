#include "canvas/Persistency/Provenance/createViewLookups.h"
// vim: set sw=2:

#include "canvas/Persistency/Provenance/BranchKey.h"
#include "canvas/Persistency/Provenance/TypeTools.h"
#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Utilities/TypeID.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/WrappedClassName.h"

#include <cassert>
#include <unordered_map>

using namespace art;

art::ViewLookup_t
art::createViewLookups(ProductList const& prods)
{
  // This version stores the list of products that support views.
  ViewLookup_t result;
  for (auto const& val: prods) {
    auto const& pd = val.second;
    if (!pd.supportsView()) continue;

    auto const& key = val.first;
    auto const& procName = key.processName_;
    auto const pid = pd.productID();
    auto const bt = key.branchType_;

    result[bt][procName].emplace_back(pid);
  }
  return result;
}


art::root::ViewLookup_t
art::root::createViewLookups(ProductList const& prods)
{
  // This version creates lookups of the same form as
  // createProductLookups does.  It relies on ROOT to provide the list
  // of allowed base classes for a given element of a viewable
  // container.
  ViewLookup_t result;
  for (auto const& val: prods) {
    auto const& pd = val.second;
    if (!pd.supportsView()) continue;

    auto const& key = val.first;
    auto const& procName = key.processName_;
    auto const pid = pd.productID();
    auto const bt = key.branchType_;

    // Look in the class of the product for a typedef named "value_type",
    // if there is one allow lookups by that type name too (and by all
    // of its base class names as well).
    art::TypeWithDict const TY {pd.producedClassName()};
    TClass* const TYc = TY.tClass();
    auto ET = mapped_type_of(TYc);
    if (ET || (ET = value_type_of(TYc))) {
      // The class of the product has a nested type, "mapped_type," or,
      // "value_type," so allow lookups by that type and all of its base
      // types too.
      auto const vtFCN = ET.friendlyClassName();
      result[bt][vtFCN][procName].emplace_back(pid);
      if (ET.category() == art::TypeWithDict::Category::CLASSTYPE) {
        // Repeat this for all public base classes of the value_type.
        std::vector<TClass*> bases;
        art::public_base_classes(ET.tClass(), bases);
        for (auto const BT: bases) {
          auto const btFCN = art::TypeID{BT->GetTypeInfo()}.friendlyClassName();
          result[bt][btFCN][procName].emplace_back(pid);
        }
      }
    }
  }
  return result;
}
