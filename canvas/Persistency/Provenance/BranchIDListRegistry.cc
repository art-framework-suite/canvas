#include "canvas/Persistency/Provenance/BranchIDListRegistry.h"
#include "cetlib/assert_only_one_thread.h"

art::BranchIDListRegistry* art::BranchIDListRegistry::instance_ = nullptr;

art::BranchIDListRegistry&
art::BranchIDListRegistry::instance()
{
  static BranchIDListRegistry me;
  instance_ = &me;
  return *instance_;
}

// Called on file open.  Seed BranchIDListRegistry for schema
// evolution for products in this file.
void
art::BranchIDListRegistry::resetFromFile(std::unique_ptr<BranchIDLists>&& file_bidlists)
{
  CET_ASSERT_ONLY_ONE_THREAD();
  instance().data_ = std::move(*file_bidlists);
}
