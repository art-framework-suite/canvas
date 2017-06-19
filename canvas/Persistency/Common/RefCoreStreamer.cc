#include "canvas/Persistency/Common/RefCoreStreamer.h"

#include "canvas/Persistency/Common/EDProductGetterFinder.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include "TBuffer.h"
#include "TClassRef.h"

namespace art {

  void
  RefCoreStreamer::operator()(TBuffer& R_b, void* objp) {
    static TClassRef cl{"art::RefCore"};
    if (R_b.IsReading()) {
      cl->ReadBuffer(R_b, objp);
      auto obj = static_cast<RefCore*>(objp);
      if (finder_ && obj->id().isValid()) { // Do not attempt to fluff empty RefCore
        EDProductGetter const* edProductGetter = finder_->getEDProductGetter(obj->id());
        obj->setProductGetter(edProductGetter);
      } else {
        obj->setProductGetter(nullptr);
      }
      obj->setProductPtr(nullptr);
    } else {
      cl->WriteBuffer(R_b, objp);
    }
  }

  void configureRefCoreStreamer(cet::exempt_ptr<EDProductGetterFinder const> finder) {
    static TClassRef cl{"art::RefCore"};
    auto st = static_cast<RefCoreStreamer*>(cl->GetStreamer());
    if (st == nullptr) {
      cl->AdoptStreamer(new RefCoreStreamer{finder});
    } else {
      st->setEDProductGetterFinder(finder);
    }
  }
}
