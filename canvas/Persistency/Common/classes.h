#include "canvas/Persistency/Common/BoolCache.h"
#include "canvas/Persistency/Common/ConstPtrCache.h"
#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Common/HLTGlobalStatus.h"
#include "canvas/Persistency/Common/HLTPathStatus.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/PtrVectorBase.h"
#include "canvas/Persistency/Common/RNGsnapshot.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Common/TriggerResults.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "cpp0x/utility"
#include <vector>

namespace {
  struct dictionary {
    art::Wrapper<art::HLTPathStatus> dummyx16;
    art::Wrapper<std::vector<art::HLTPathStatus> > dummyx17;
    art::Wrapper<art::HLTGlobalStatus> dummyx18;
    art::Wrapper<art::TriggerResults>  dummyx19;
    art::Ptr<int>                      dummypi;
    art::PtrVector<int>                dummypvi;
    art::Wrapper<art::PtrVector<int> > dummypviw;

    std::pair<art::RefCore, size_t> prs;
    std::vector<std::pair<art::RefCore, size_t> > vprs;
  };
}  // namespace
