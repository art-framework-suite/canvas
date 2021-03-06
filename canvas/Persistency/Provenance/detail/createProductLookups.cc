#include "canvas/Persistency/Provenance/detail/createProductLookups.h"
// vim: set sw=2:

#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/TypeID.h"

#include <algorithm>
#include <string>
#include <unordered_map>

using namespace art;

namespace {

  struct CheapTag {
    std::string label;
    std::string instance;
    std::string process;
  };

  inline bool
  operator==(CheapTag const& left, CheapTag const& right)
  {
    return left.label == right.label && left.instance == right.instance &&
           left.process == right.process;
  }

  class PendingBTLEntry {
  public:
    PendingBTLEntry(std::string const& fcn,
                    std::string const& moduleLabel,
                    std::string const& instanceName,
                    std::string const& procName,
                    ProductID const pid)
      : fcn_{fcn}, ct_{moduleLabel, instanceName, procName}, pid_{pid}
    {}

    std::string const&
    fcn() const noexcept
    {
      return fcn_;
    }
    CheapTag const&
    ct() const noexcept
    {
      return ct_;
    }
    std::string const&
    process() const noexcept
    {
      return ct_.process;
    }
    ProductID
    pid() const
    {
      return pid_;
    }

  private:
    std::string fcn_;
    CheapTag ct_;
    ProductID pid_;
  };
}

art::ProductLookup_t
art::detail::createProductLookups(ProductDescriptionsByID const& descriptions)
{
  // Computing the product lookups does not rely on any ROOT facilities.
  ProductLookup_t result;
  std::vector<PendingBTLEntry> pendingEntries;
  std::unordered_map<ProductID, CheapTag, ProductID::Hash> insertedABVs;
  for (auto const& [pid, pd] : descriptions) {
    auto const& prodFCN = pd.friendlyClassName();
    auto const& procName = pd.processName();
    result[prodFCN][procName].emplace_back(pid);

    // Additional work only for Assns lookup
    auto const& moduleLabel = pd.moduleLabel();
    auto const& instanceName = pd.productInstanceName();
    auto const& className = pd.producedClassName();

    if (!is_assns(className))
      continue;

    auto const baseName = name_of_assns_base(className);
    if (!baseName.empty()) {
      // We're an Assns<A, B, D>, with a base Assns<A, B>.
      pendingEntries.emplace_back(art::friendlyname::friendlyName(baseName),
                                  moduleLabel,
                                  instanceName,
                                  procName,
                                  pid);
    } else {
      // Add our pid to the list of real Assns<A, B, void>
      // products already registered.
      insertedABVs.emplace(pid, CheapTag{moduleLabel, instanceName, procName});
    }
  }

  auto const iend = insertedABVs.cend();
  // Preserve useful ordering, only inserting if we don't already have
  // a *real* Assns<A, B, void> for that module label / instance name
  // combination.
  std::for_each(
    pendingEntries.cbegin(),
    pendingEntries.cend(),
    [&result, &insertedABVs, iend](auto const& pe) {
      auto& pids = result[pe.fcn()][pe.process()];
      if (pids.empty() ||
          !std::any_of(pids.cbegin(),
                       pids.cend(),
                       [&insertedABVs, &iend, &pe](ProductID const pid) {
                         auto i = insertedABVs.find(pid);
                         return i != iend && pe.ct() == i->second;
                       })) {
        pids.emplace_back(pe.pid());
      }
    });

  return result;
}
