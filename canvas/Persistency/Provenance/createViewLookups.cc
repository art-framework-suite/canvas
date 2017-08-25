#include "canvas/Persistency/Provenance/createViewLookups.h"
// vim: set sw=2:

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
