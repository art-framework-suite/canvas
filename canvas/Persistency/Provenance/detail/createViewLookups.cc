#include "canvas/Persistency/Provenance/detail/createViewLookups.h"
// vim: set sw=2:

art::ViewLookup_t
art::detail::createViewLookups(ProductDescriptionsByID const& descriptions)
{
  // This version stores the list of products that support views.
  ViewLookup_t result;
  for (auto const& [pid, pd] : descriptions) {
    if (!pd.supportsView())
      continue;

    auto const& procName = pd.processName();
    result[procName].emplace_back(pid);
  }
  return result;
}
