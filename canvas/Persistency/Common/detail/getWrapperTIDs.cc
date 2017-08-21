#include "canvas/Persistency/Common/detail/getWrapperTIDs.h"

#include "canvas/Utilities/TypeID.h"
#include "canvas/Persistency/Provenance/TypeTools.h"
#include "canvas/Persistency/Provenance/TypeWithDict.h"

using namespace std;

namespace {
  void maybeThrowLateDictionaryError(art::TypeWithDict const & twd,
                                     std::string const & tname)
  {
    if (!twd) {
      art::throwLateDictionaryError(tname);
    }
  }
}

std::vector<art::TypeID>
art::detail::
getWrapperTIDs(std::string const & productClassName)
{
  std::vector<TypeID> result;
  TypeWithDict const ta(productClassName);
  maybeThrowLateDictionaryError(ta, productClassName);
  auto const twName = art::wrappedClassName(productClassName);
  TypeWithDict const tw(twName);
  maybeThrowLateDictionaryError(tw, twName);
  result.emplace_back(tw.id()); // Wrapper.
  if (ta.category() == TypeWithDict::Category::CLASSTYPE &&
      is_instantiation_of(ta.tClass(), "art::Assns")) {
    auto const tpName = name_of_assns_partner(productClassName);
    TypeWithDict const tp(tpName);
    maybeThrowLateDictionaryError(tp, tpName);
    auto const twpName = art::wrappedClassName(tpName);
    TypeWithDict const twp(twpName);
    maybeThrowLateDictionaryError(twp, twpName);
    result.emplace_back(twp.id()); // Wrapper of partner.
    auto const baseName = name_of_assns_base(productClassName);
    if (!baseName.empty()) {
      TypeWithDict const base(baseName);
      maybeThrowLateDictionaryError(base, baseName);
      auto const basewName = art::wrappedClassName(baseName);
      TypeWithDict const basew(basewName);
      maybeThrowLateDictionaryError(basew, basewName);
      result.emplace_back(basew.id()); // Wrapper of base.
      auto const basepName = name_of_assns_partner(baseName);
      TypeWithDict const basep(basepName);
      maybeThrowLateDictionaryError(basep, basepName);
      auto const basewpName = art::wrappedClassName(basepName);
      TypeWithDict const basewp(basewpName);
      maybeThrowLateDictionaryError(basewp, basewpName);
      result.emplace_back(basewp.id()); // Wrapper of partner's base.
    }
  }
  return result;
}
