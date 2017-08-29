#include "canvas/Persistency/Provenance/createViewLookups.h"
// vim: set sw=2:

using namespace art;

art::ViewLookup_t
art::createViewLookups(ProductDescriptions const& descriptions)
{
  // This version stores the list of products that support views.
  ViewLookup_t result;
  for (auto const& pd: descriptions) {
    if (!pd.supportsView()) continue;

    auto const bt = pd.branchType();
    auto const& procName = pd.processName();
    auto const pid = pd.productID();

    result[bt][procName].emplace_back(pid);
  }
  return result;
}
