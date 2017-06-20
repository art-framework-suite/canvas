#ifndef canvas_Persistency_Common_RefCoreStreamer_h
#define canvas_Persistency_Common_RefCoreStreamer_h

#include "cetlib/exempt_ptr.h"

#include "TClassStreamer.h"

class TBuffer;

namespace art {

  class EDProductGetterFinder;

  void
  configureRefCoreStreamer(cet::exempt_ptr<EDProductGetterFinder const> finder = {});

  class RefCoreStreamer : public TClassStreamer {
  public:

    explicit RefCoreStreamer(cet::exempt_ptr<EDProductGetterFinder const> finder = {})
      : finder_{finder}
    {}

    void setEDProductGetterFinder(cet::exempt_ptr<EDProductGetterFinder const> finder) {
      finder_ = finder;
    }

    void operator() (TBuffer& R_b, void* objp) override;

  private:
    cet::exempt_ptr<EDProductGetterFinder const> finder_;
  };
}
#endif /* canvas_Persistency_Common_RefCoreStreamer_h */

// Local Variables:
// mode: c++
// End:
