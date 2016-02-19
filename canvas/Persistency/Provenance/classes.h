#include "canvas/Persistency/Provenance/BranchChildren.h"
#include "canvas/Persistency/Provenance/BranchID.h"
#include "canvas/Persistency/Provenance/EventAuxiliary.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/FileIndex.h"
#include "canvas/Persistency/Provenance/History.h"
#include "canvas/Persistency/Provenance/ParameterSetMap.h"
#include "canvas/Persistency/Provenance/Parentage.h"
#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ProcessHistory.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/ProductRegistry.h"
#include "canvas/Persistency/Provenance/RunAuxiliary.h"
#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
#include "canvas/Persistency/Provenance/ResultsAuxiliary.h"

namespace {
   struct dictionary {
      std::vector<art::ProductProvenance> d1;
      std::vector<art::ProcessConfiguration> d2;
      std::vector<art::ProcessHistory> d3;
      art::ProcessHistory::collection_type d4;
      std::set<art::BranchID> d5;
      std::pair<art::BranchID, std::set<art::BranchID> > d6;
      std::map<art::BranchID, std::set<art::BranchID> > d7;
      art::BranchKey d8;
      art::BranchDescription d9;
      std::pair<art::BranchKey, art::BranchDescription> d10;
      std::map<art::BranchKey, art::BranchDescription> d11;
      std::set<fhicl::ParameterSetID> d12;
      art::ProcessConfigurationID d13a;
      std::set<art::ProcessConfigurationID> d13;
      std::vector<art::BranchID> d14;
      std::set<art::ProcessHistoryID> d15;
      std::vector<art::FileIndex::Element> d16;
      art::ProcessHistoryMap d17;
      std::pair<const art::ProcessHistoryID, art::ProcessHistory> d18;
      art::ParameterSetMap d19;
      std::pair<fhicl::ParameterSetID, art::ParameterSetBlob> d20;
   };
}
