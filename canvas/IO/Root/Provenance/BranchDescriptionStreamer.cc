#include "canvas/IO/Root/Provenance/BranchDescriptionStreamer.h"
#include "canvas/Persistency/Provenance/BranchDescription.h"

#include "TClass.h"

namespace art {

  void
  detail::BranchDescriptionStreamer::operator()(TBuffer& R_b, void* objp)
  {
    static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
    auto obj = reinterpret_cast<BranchDescription*>(objp);
    if (R_b.IsReading()) {
      cl->ReadBuffer(R_b, obj);
      obj->fluffTransients_();
    }
    else {
      cl->WriteBuffer(R_b, obj);
    }
  }

  void
  detail::setBranchDescriptionStreamer()
  {
    static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
    if (cl->GetStreamer() == nullptr) {
      cl->AdoptStreamer(new BranchDescriptionStreamer);
    }
  }

} // namespace art
